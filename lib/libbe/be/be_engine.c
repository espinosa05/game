#include <be/be_engine.h>
#include <be/be_app_entry.h>
#include <core/log.h>
#include <core/memory_macros.h>
#include <core/os_util.h>
#include <core/os_thread.h>
#include <core/os_directory.h>
#include <core/os_path.h>
#include <core/wm_utils.h>

/* static function declaration start */
static void compile_shaders(const struct be_app_settings settings, struct m_arena *arena);
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
/* static function declaration end */

#undef UNUSED
#undef IMPL
#undef TODO

#define UNUSED(x) (void)x
#define IMPL()  do { } while (0)
#define TODO(...) do { } while (0)

void be_init_layers(struct be_engine *be_engine, usz layer_count)
{
    struct be_app_layer *buff = m_arena_alloc(&be_engine->permanent_memory, sizeof(struct be_app_layer), layer_count);
    mm_array_init_ext(&be_engine->layers, buff, layer_count);
}

void be_push_layer(struct be_engine *be_engine, struct be_app_layer layer)
{
    mm_array_append(&be_engine->layers, layer);
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

#define SETUP_ARENA_SIZE KB_SIZE
#define PERM_ARENA_SIZE (20*MB_SIZE)
#define MAX_LAYERS USZ(16)

void be_engine_init(struct be_engine *be_engine)
{
    be_engine->close    = FALSE;
    be_engine->dt       = 0;

    os_time_init(&be_engine->frame_start);
    os_time_init(&be_engine->frame_end);

    /* Temporary setup arena */
    struct m_arena init_arena = {0};
    struct m_arena_info init_arena_info = {0};
    init_arena_info.buffer      = (u8[SETUP_ARENA_SIZE]) {0};
    init_arena_info.mem_size    = SETUP_ARENA_SIZE;
    init_arena_info.external    = TRUE;
    m_arena_init(&init_arena, init_arena_info);

    /* initialize context memory */
    static u8 permanent_buffer[PERM_ARENA_SIZE] = {0};
    struct m_arena_info permanent_arena_info = {0};
    permanent_arena_info.buffer     = permanent_buffer;
    permanent_arena_info.mem_size   = sizeof(permanent_buffer);
    permanent_arena_info.external   = TRUE;
    m_arena_init(&be_engine->permanent_memory, permanent_arena_info);

    /* initialize layers */
    mm_array_init_ar(&be_engine->layers, MAX_LAYERS, &be_engine->permanent_memory);

    /* load data from the client application code */
    struct be_app_settings app_settings = BLEEDING_EDGE_DEFAULT_APP_SETTINGS;
    be_app_entry(be_engine, &app_settings);

    mm_array_init_ar(&be_engine->events, BLEEDING_EDGE_EVENT_QUEUE_LENGTH, &be_engine->permanent_memory);

    compile_shaders(app_settings, &init_arena);

    wm_init(&be_engine->wm);
    struct wm_window_info main_window_info = {0};
    main_window_info.initial_title  = app_settings.app_window_title;
    main_window_info.force_size     = TRUE;
    main_window_info.width          = app_settings.app_window_width;
    main_window_info.height         = app_settings.app_window_height;
    main_window_info.x_pos          = X_POS_CENTERED;
    main_window_info.y_pos          = Y_POS_CENTERED;
    wm_window_create(&be_engine->wm, &be_engine->main_window, main_window_info);
    wm_window_show(&be_engine->wm, &be_engine->main_window);
}

void be_engine_run(struct be_engine *be_engine)
{
    while (!should_close(be_engine)) {
        tick_start(be_engine);

        poll_events(be_engine);
        handle_events(be_engine);

        update(be_engine);
        render(be_engine);

        present_image(be_engine);

        transition_layers(be_engine);
        tick_end(be_engine);
    }
}

void be_engine_delete(struct be_engine *be_engine)
{
    wm_shutdown(&be_engine->wm);
}

void be_engine_set_close(volatile struct be_engine *be_engine)
{
    be_engine->close = TRUE;
}

#define SHADER_CACHE_SUB_DIR "shader_cache/"
#define SHADER_SRC_SUB_DIR "shader_src/"

/* almost all resource directory paths are made up of the "root" and one sub directory */
#define RSRC_PATH_PARTS 2

static void compile_shaders(const struct be_app_settings settings, struct m_arena *arena)
{
    ASSERT(settings.app_name, "Provide application name!");

    /* open shader cache */
    struct str_builder app_shader_cache_path = {0};
    str_builder_init_ar(&app_shader_cache_path, RSRC_PATH_PARTS, arena);
    SB_CALL(str_builder_append(&app_shader_cache_path, settings.app_root_path));
    SB_CALL(str_builder_append(&app_shader_cache_path, SHADER_CACHE_SUB_DIR));

    char *shader_cache_dir_name = NULL;
    str_builder_to_cstr_ar(&app_shader_cache_path, &shader_cache_dir_name, arena);
    INFO_LOG("shader_source_dir_name: "STR_FMT, shader_cache_dir_name);

    struct os_dir shader_cache_dir = {0};
    struct os_dir_info shader_cache_dir_info = {0};
    shader_cache_dir_info.path = shader_cache_dir_name;
    shader_cache_dir_info.perm = DPERM_READ;

    if (os_path_exists(shader_cache_dir_name)) {
        os_dir_open(&shader_cache_dir, shader_cache_dir_info);
    } else {
        os_dir_create(&shader_cache_dir, shader_cache_dir_info);
    }

    /* open shader source files */
    struct str_builder app_shader_source_dir_path = {0};
    str_builder_init_ar(&app_shader_source_dir_path, RSRC_PATH_PARTS, arena);
    SB_CALL(str_builder_append(&app_shader_source_dir_path, settings.app_root_path));
    SB_CALL(str_builder_append(&app_shader_source_dir_path, SHADER_SRC_SUB_DIR));

    char *shader_source_dir_name = NULL;
    str_builder_to_cstr_ar(&app_shader_source_dir_path, &shader_source_dir_name, arena);
    INFO_LOG("shader_source_dir_name: "STR_FMT, shader_source_dir_name);

    ASSERT_RT(os_path_exists(shader_source_dir_name), "Corrupted Asset data. Shader directory "STR_QUOT(STR_FMT)" not found."
                                                      "Fix installation before continuing!",
                                                      shader_source_dir_name);

    struct os_dir shader_source_dir = {0};
    struct os_dir_info shader_source_dir_info = {0};
    shader_source_dir_info.path = shader_source_dir_name;
    shader_source_dir_info.perm = DPERM_READ;

    os_dir_open(&shader_source_dir, shader_source_dir_info);

    struct os_paths shader_source_file_paths = {0};
    os_dir_get_file_paths_ar(&shader_source_dir, &shader_source_file_paths, arena);

    /* compile shader source files */
    for (usz i = 0; i < shader_source_file_paths.count; ++i) {
        compile_shader_file(shader_source_file_paths.data[i], shader_cache_dir_name, arena);
    }

    os_dir_close(&shader_cache_dir);
    os_dir_close(&shader_source_dir);
}

static void compile_shader_file(const char *file, const char *cache_path, struct m_arena *arena)
{
    char *shader_name = NULL;
    UNUSED(cache_path);
    os_util_strip_file_extension_ar(file, &shader_name, arena);
    INFO_LOG("compiling shader "STR_FMT, shader_name);
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

