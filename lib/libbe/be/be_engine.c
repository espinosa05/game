#include <be/be_engine.h>
#include <be/be_app_entry.h>
#include <core/cstr.h>
#include <core/log.h>
#include <core/cli.h>
#include <core/str_builder.h>
#include <core/memory_macros.h>
#include <core/os_util.h>
#include <core/os_thread.h>
#include <core/os_directory.h>
#include <core/os_path.h>
#include <core/wm_utils.h>
#include <core/shader_compiler.h>

struct be_environment {
    char *config_root;
    char *state_root;
    char *storage_root;
    char *cache_root;
};

/* static function declaration start */
static void be_init_layers(struct be_engine *be_engine, usz init_layer_count);

static void compile_shaders(struct be_environment env, struct m_arena *arena);
static usz shader_type_from_string(const char *str);
static void compile_shader_file(const char *file, const char *cache_path, struct m_arena *arena);
static b32 should_close(struct be_engine *be_engine);
static void tick_start(struct be_engine *be_engine);
static void tick_end(struct be_engine *be_engine);
static void poll_events(struct be_engine *be_engine);
static void handle_events(struct be_engine *be_engine);
static void update(struct be_engine *be_engine);
static void render(struct be_engine *be_engine);
static void present_image(struct be_engine *be_engine);
static void transition_layers(struct be_engine *be_engine);
static void load_environment(struct be_environment *env);
static b32 shader_cache_exists(struct be_environment env, struct m_arena *arena);

static void be_core_on_attach(struct be_engine *be);
static void be_core_on_detach(struct be_engine *be);
static void be_core_on_update(struct be_engine *be);
static void be_core_on_event(struct be_engine *be, struct wm_event event);
static void be_core_on_suspend(struct be_engine *be);

static void be_window_on_attach(struct be_engine *be);
static void be_window_on_detach(struct be_engine *be);
static void be_window_on_update(struct be_engine *be);
static void be_window_on_event(struct be_engine *be, struct wm_event event);
static void be_window_on_suspend(struct be_engine *be);

static void be_diag_on_attach(struct be_engine *be);
static void be_diag_on_detach(struct be_engine *be);
static void be_diag_on_update(struct be_engine *be);
static void be_diag_on_event(struct be_engine *be, struct wm_event event);
static void be_diag_on_suspend(struct be_engine *be);
/* static function declaration end */

#undef UNUSED
#undef IMPL
#undef TODO

#define UNUSED(x) (void)x
#define IMPL()  do { } while (0)
#define TODO(...) do { } while (0)

void be_push_layer(struct be_engine *be_engine, struct be_app_layer_spec spec)
{
    CHECK_NULL(spec.on_attach);
    CHECK_NULL(spec.on_detach);
    CHECK_NULL(spec.on_update);
    CHECK_NULL(spec.on_event);
    CHECK_NULL(spec.on_suspend);

    struct be_app_layer layer = {0};
    layer.on_detach     = spec.on_detach;
    layer.on_render     = spec.on_render;
    layer.on_update     = spec.on_update;
    layer.on_event      = spec.on_event;
    layer.on_suspend    = spec.on_suspend;
    mm_array_append(&be_engine->layers, layer);

    spec.on_attach(be_engine);
}

#define BLEEDING_EDGE_EVENT_QUEUE_LENGTH USZ(128)
#define BLEEDING_EDGE_DEFAULT_APP_SETTINGS                      \
            {                                                   \
                .app_name                   = "DEFAULT_APP",    \
                .app_window_title           = "DEFAULT_APP",    \
                .app_window_width           = 1200,             \
                .app_window_height          = 720,              \
                .app_window_target_frame_ms = F64(1000)/60,     \
                .app_sim_thread_count       = 4,                \
                .app_root_path              = "./",             \
            }

void be_engine_init(struct be_engine *be_engine, struct cli_args args)
{
    be_init_layers(be_engine);
    be_push_layer(be_engine, BE_LAYER_SPEC(be_core));
    be_push_layer(be_engine, BE_LAYER_SPEC(be_window));

    be_app_entry(be_engine, &app_settings, args);

    be_push_layer(be_engine, BE_LAYER_SPEC(be_diag));
}

void be_engine_run(struct be_engine *be_engine)
{
    be_update_layers(be_engine);
}

void be_engine_delete(struct be_engine *be_engine)
{
    be_delete_layers(be_engine);
}

void be_engine_set_close(volatile struct be_engine *be_engine)
{
    be_engine->close = TRUE;
}

static void new_allocator_link(struct be_allocator **allocator, usz size)
{
}

static void delete_allocator_link(struct be_allocator *allocator)
{
}

static void be_init_layers(struct be_engine *be_engine, usz init_layer_count)
{
    struct be_app_layer *buff = be_alloc_permanent(&be_engine->memory, sizeof(struct be_app_layer), layer_count);
    mm_array_init_ext(&be_engine->layers, buff, layer_count);
}

static void be_allocator_init(struct be_allocator *allocator, usz init_size)
{
}

static void be_allocator_refresh(struct be_allocator *allocator)
{

}

static void *be_alloc_permanent(struct be_memory *memory, usz chunk, usz count)
{

}

static void *be_memory_refresh(struct be_memory *memory)
{
}

static void *be_alloc_transient(struct be_memory *memory, usz chunk, usz count)
{
}

static void be_memory_init(struct be_memory *memory, const struct be_memory_info info)
{
}

#define SHADER_CACHE_SUB_DIR "shader_cache/"
#define SHADER_SRC_SUB_DIR "shader_src/"

/* almost all resource directory paths are made up of the "root" and one sub directory */
#define RSRC_PATH_PART_COUNT 2

static void compile_shaders(struct be_environment env, struct m_arena *arena)
{
    /* build shader cache path */
    struct str_builder app_shader_cache_path = {0};
    str_builder_init_ar(&app_shader_cache_path, RSRC_PATH_PART_COUNT, arena);
    SB_CALL(str_builder_append(&app_shader_cache_path, env.cache_root));
    SB_CALL(str_builder_append(&app_shader_cache_path, SHADER_CACHE_SUB_DIR));

    char *shader_cache_dir_name = NULL;
    str_builder_to_cstr_ar(&app_shader_cache_path, &shader_cache_dir_name, arena);
    os_dir_create(shader_cache_dir_name);

    /* builder shader source path */
    struct str_builder app_shader_source_dir_path = {0};
    str_builder_init_ar(&app_shader_source_dir_path, RSRC_PATH_PART_COUNT, arena);
    SB_CALL(str_builder_append(&app_shader_source_dir_path, env.storage_root));
    SB_CALL(str_builder_append(&app_shader_source_dir_path, SHADER_SRC_SUB_DIR));

    char *shader_source_dir_name = NULL;
    str_builder_to_cstr_ar(&app_shader_source_dir_path, &shader_source_dir_name, arena);
    INFO_LOG(STR_SYM_MSG(shader_source_dir_name));

    ASSERT_RT(os_path_exists(shader_source_dir_name), "Corrupted Asset data. Shader directory "STR_QUOT(STR_FMT)" not found. "
                                                      "Fix installation before continuing!",
                                                      shader_source_dir_name);

    /* open shader source path  */
    struct os_dir shader_source_dir = {0};
    struct os_dir_info shader_source_dir_info = {0};
    shader_source_dir_info.path = shader_source_dir_name;
    shader_source_dir_info.perm = DPERM_READ;
    os_dir_open(&shader_source_dir, shader_source_dir_info);

    /* open shader source files */
    struct os_paths shader_source_file_paths = {0};
    os_dir_get_file_paths_ar(&shader_source_dir, &shader_source_file_paths, arena);

    /* compile shader source files into shader cache */
    for (usz i = 0; i < shader_source_file_paths.count; ++i) {
        compile_shader_file(shader_source_file_paths.data[i], shader_cache_dir_name, arena);
    }

    os_dir_close(&shader_source_dir);
}

#define SHADER_EXT_VERTEX_STR                   "vert"
#define SHADER_EXT_TESSELATION_CONTROL_STR      "tesc"
#define SHADER_EXT_TESSELATION_EVALUATION_STR   "tese"
#define SHADER_EXT_GEOMETRY_STR                 "geom"
#define SHADER_EXT_FRAGMENT_STR                 "frag"
#define SHADER_EXT_COMPUTE_STR                  "comp"

static usz shader_type_from_string(const char *str)
{
    usz type = 0;

    if (cstr_compare(str, SHADER_EXT_VERTEX_STR)) {
        type = SHADER_TYPE_VERTEX;
    } else if (cstr_compare(str, SHADER_EXT_TESSELATION_CONTROL_STR)) {
        type = SHADER_TYPE_TESSELATION_CONTROL;
    } else if (cstr_compare(str, SHADER_EXT_TESSELATION_EVALUATION_STR)) {
        type = SHADER_TYPE_TESSELATION_EVALUATION;
    } else if (cstr_compare(str, SHADER_EXT_GEOMETRY_STR)) {
        type = SHADER_TYPE_GEOMETRY;
    } else if (cstr_compare(str, SHADER_EXT_FRAGMENT_STR)) {
        type = SHADER_TYPE_FRAGMENT;
    } else if (cstr_compare(str, SHADER_EXT_COMPUTE_STR)) {
        type = SHADER_TYPE_COMPUTE;
    } else {
        type = SHADER_TYPE_NULL;
    }

    return type;
}

#define ASSET_PATH_PART_COUNT 2
#define FILE_EXTENSION_PART 1

static void compile_shader_file(const char *file, const char *cache_path, struct m_arena *arena)
{
    INFO_LOG("compiling "STR_QUOT(STR_FMT)"...", file);
    struct shader_compiler compiler = {0};
    struct shader_compiler_info compiler_info = {0};
    compiler_info.language = SC_SHADER_LANGUAGE_GLSL;
    compiler_info.target = SC_SHADER_TARGET_SPIRV;
    SC_CALL(sc_init(&compiler, compiler_info));

    char *ext = NULL;
    os_util_get_file_extension_ar(file, &ext, arena);

    usz shader_type = shader_type_from_string(ext);
    ASSERT_RT(shader_type != SHADER_TYPE_NULL, "entry "STR_QUOT(STR_FMT)" found within shader directory "
                                               "is missing shader file extension: "
                                               STR_QUOT(SHADER_EXT_VERTEX_STR)", "
                                               STR_QUOT(SHADER_EXT_TESSELATION_CONTROL_STR)", "
                                               STR_QUOT(SHADER_EXT_TESSELATION_EVALUATION_STR)", "
                                               STR_QUOT(SHADER_EXT_GEOMETRY_STR)", "
                                               STR_QUOT(SHADER_EXT_FRAGMENT_STR)" or "
                                               STR_QUOT(SHADER_EXT_COMPUTE_STR),
                                               file);

    struct string shader_source = {0};
    os_util_read_file_ar(file, &shader_source, arena);

    struct m_buffer binary = {0};
    struct shader_source_info shader_info = {0};
    shader_info.type    = shader_type;
    shader_info.source  = shader_source;
    SC_CALL(sc_compile_source(&compiler, shader_info, &binary, arena));
    SC_CALL(sc_shutdown(&compiler));

    char *binary_file_path = NULL;
    struct str_builder binary_file_path_builder = {0};
    str_builder_init_ar(&binary_file_path_builder, ASSET_PATH_PART_COUNT + FILE_EXTENSION_PART, arena);
    str_builder_append(&binary_file_path_builder, cache_path);
    str_builder_append(&binary_file_path_builder, file);
    str_builder_append(&binary_file_path_builder, SHADER_BINARY_EXT_STR);
    str_builder_to_cstr_ar(&binary_file_path_builder, &binary_file_path, arena);

    INFO_LOG(STR_SYM_MSG(binary_file_path));
    struct os_file binary_file = {0};
    struct os_file_info binary_file_info = {0};
    binary_file_info.path = binary_file_path;
    binary_file_info.perm = FPERM_READ_AND_WRITE;
    OS_FILE_CALL(os_file_create(&binary_file, binary_file_info));
    OS_FILE_CALL(os_file_write_buff(&binary_file, binary.size, &binary));

    os_file_close(&binary_file);
}

static b32 should_close(struct be_engine *be_engine)
{
    return be_engine->close;
}

static void tick_start(struct be_engine *be_engine)
{
    /* get frame start time */
    {
        struct os_time frame_time = {0};
        os_time_get_monotonic(&be_engine->frame_start);
        os_time_get_diff(&frame_time, be_engine->frame_start, be_engine->frame_end);
        be_engine->dt = os_time_get_usec(frame_time);
    }
}

static void tick_end(struct be_engine *be_engine)
{
    os_time_get_monotonic(&be_engine->frame_end);
}

static void poll_events(struct be_engine *be_engine)
{
    struct wm_event event = {0};

    do {
        wm_window_poll_events(&be_engine->wm, &be_engine->main_window, &event);
        if (event.event_type == WM_EVENT_EMPTY_QUEUE)
            break;

        mm_array_append(&be_engine->events, event);
    } while (mm_array_free_space(&be_engine->events));
}

static void handle_events(struct be_engine *be_engine)
{
    struct wm_events events = be_engine->events;
    void (*on_event) (struct be_engine *, struct wm_event) = NULL;
    struct be_app_layers layers = be_engine->layers;

    /* we want every layer to potentially handle every event */
    for (usz i = 0; i < events.count; ++i) {
        for (usz j = 0; j < layers.count; ++j) {
            on_event = layers.data[j].on_event;
            on_event(be_engine, events.data[i]);
        }
    }
}

static void update(struct be_engine *be_engine)
{
    void (*on_update) (struct be_engine *) = NULL;
    struct be_app_layers layers = be_engine->layers;

    for (usz i = 0; i < layers.count; ++i) {
        on_update = layers.data[i].on_update;
        on_update(be_engine);
    }
}

static void render(struct be_engine *be_engine)
{
    void (*on_render) (struct be_engine *) = NULL;
    struct be_app_layers layers = be_engine->layers;

    for (usz i = 0; i < layers.count; ++i) {
        on_render = layers.data[i].on_render;
        on_render(be_engine);
    }
}

static void present_image(struct be_engine *be_engine)
{
    UNUSED(be_engine);
}

static void transition_layers(struct be_engine *be_engine)
{
    UNUSED(be_engine);
    IMPL();
}

#define APP_ROOT_PATH_KEY   "XDG_DATA_HOME"
#define APP_SAVE_PATH_KEY   "XDG_STATE_HOME"
#define APP_STATE_PATH_KEY  "XDG_STATE_HOME"
#define APP_CACHE_PATH_KEY  "XDG_CACHE_HOME"

/* these are all relative to the execution path */
#define APP_CONFIG_ROOT_FALLBACK    "./config/"
#define APP_STATE_ROOT_FALLBACK     "./state/"
#define APP_STORAGE_ROOT_FALLBACK   "./"
#define APP_CACHE_ROOT_FALLBACK     "./cache/"

static void load_environment(struct be_environment *env)
{
    os_get_env(&env->config_root, APP_STATE_PATH_KEY);
    if (!env->config_root) {
        INFO_LOG("$"STR_FMT" not set. setting fallback: "STR_QUOT(STR_FMT)" (relative to executable)", APP_STATE_PATH_KEY, APP_CONFIG_ROOT_FALLBACK);
        env->config_root = APP_CONFIG_ROOT_FALLBACK;
    }

    os_get_env(&env->state_root, APP_STATE_PATH_KEY);
    if (!env->state_root) {
        INFO_LOG("$"STR_FMT" not set. setting fallback: "STR_QUOT(STR_FMT)" (relative to executable)", APP_SAVE_PATH_KEY, APP_STATE_ROOT_FALLBACK);
        env->state_root = APP_STATE_ROOT_FALLBACK;
    }

    os_get_env(&env->storage_root, APP_SAVE_PATH_KEY);
    if (!env->storage_root) {
        INFO_LOG("$"STR_FMT" not set. setting fallback: "STR_QUOT(STR_FMT)" (relative to executable)", APP_SAVE_PATH_KEY, APP_STORAGE_ROOT_FALLBACK);
        env->storage_root = APP_STORAGE_ROOT_FALLBACK;
    }

    os_get_env(&env->cache_root, APP_CACHE_PATH_KEY);
    if (!env->cache_root) {
        INFO_LOG("$"STR_FMT" not set. setting fallback: "STR_QUOT(STR_FMT)" (relative to executable)", APP_CACHE_PATH_KEY, APP_CACHE_ROOT_FALLBACK);
        env->cache_root = APP_CACHE_ROOT_FALLBACK;
    }
}

static b32 shader_cache_exists(struct be_environment env, struct m_arena *arena)
{
    struct str_builder sb = {0};
    str_builder_init_ar(&sb, 2, arena);
    str_builder_append(&sb, env.cache_root);
    str_builder_append(&sb, SHADER_CACHE_SUB_DIR);

    char *cache_path = NULL;
    str_builder_to_cstr_ar(&sb, &cache_path, arena);
    return os_path_exists(cache_path);
}

static void be_core_on_attach(struct be_engine *be)
{
    struct wm wm = {0};
    wm_init(&wm);

    struct wm_window main_window = {0};
    struct wm_window_info main_window_info = {0};
    main_window_info.title  = "test";
    main_window_info.width  = 1200;
    main_window_info.height = 720;
    main_window_info.x_pos  = X_POS_CENTERED;
    main_window_info.y_pos  = Y_POS_CENTERED;
    WM_CALL(wm_window_init(&main_window, main_window_info));

    struct be_memory memory = {0};
    struct be_memory_info memory_info = {0};
    memory_info.transient_init_size = MB(2);
    memory_info.permanent_init_size = MB(14);
    be_memory_init(&memory, memory_info);

    struct be_environment env = {0};
    load_environment(&env);

    struct m_arena init_arena = {0};
    struct m_arena_info init_arena_info = {0};
    init_arena_info.mem_size    = KB(10);
    init_arena_info.buffer      = U8_ARR(KB(10));
    init_arena_info.external    = TRUE;
    m_arena_init(&init_arena, init_arena_info);

    compile_shaders(&env, &init_arena);
}

static void be_core_on_detach(struct be_engine *be)
{

}

static void be_core_on_update(struct be_engine *be)
{

}

static void be_core_on_event(struct be_engine *be, struct wm_event event)
{

}

static void be_core_on_suspend(struct be_engine *be)
{

}

static void be_core_on_resume()
{

}

/*
static void be_window_init(struct be_engine *be)
static void be_window_delete(struct be_engine *be);
static void be_window_on_update(struct be_engine *be);
static void be_window_on_event(struct be_engine *be, struct wm_event event);
static void be_window_suspend(struct be_engine *be);
*/
