#include "cdb.h"

#include <core/types.h>

u8 buff[KB_SIZE*2] = {0};

struct app_context_info {
    const char      *name;
    struct cli_args args;
};

enum core_window_types {
    CORE_WINDOW_TYPE_MAIN_WINDOW = 0,
    CORE_WINDOW_TYPE_PROMPT_WINDOW = 1,
    CORE_WINDOW_TYPE_MESSAGE_WINDOW = 2,

    CORE_WINDOW_TYPE_COUNT,
};

struct app_context {
    struct cdb_context cdb;
    struct wm_window *main_window;
};

enum cmp_ids {
    CMP_ID_DIR,
    CMP_ID_ACC,
    CMP_ID_SPEED,
    CMP_ID_POS,
};

struct app_conf {
    const char **mod_list;
};

/* static function declaration start */
static void app_conf_defaults(struct app_conf *conf);

static void app_init(struct app_context *app, const struct app_context_info info);
static void app_run(struct app_context *app);
static void app_shutdown(struct app_context *app);
static b32 app_should_close(struct app_context *app);

static void load_mods(struct m_array *mod_array, conststruct app_conf conf);
static void init_mods(struct m_array *mod_array);

static void c_pos(struct cdb_context *cdb);
static void c_speed(struct cdb_context *cdb);
static void c_accel(struct cdb_context *cdb);
/* static function declaration end */

int main(int argc, char **argv)
{
    struct app_context app = {0};
    struct app_context_info app_info = {
        .name = __FILE__,
        .args = CLI_ARGS(argc, argv),
    };
    app_init(&app, app_info);
    app_run(&app);
    app_shutdown(&app);

    return EXIT_SUCCESS;
}

static void app_conf_defaults(struct app_conf *conf)
{
    *conf = (struct app_conf) {0};
}

#define MAX_REGIONS 3

static void app_init(struct app_context *app, const struct app_context_info info)
{
    struct app_conf init_conf = {0};
    app_conf_defaults(&init_conf);

    struct m_arena arena = {0};
    struct m_arena_info arena_info = {
        .mem_size   = sizeof(buff),
        .buffer     = buff,
        .external   = TRUE,
    };
    m_arena_init(&arena, arena_info);

    struct cdb_context_info cdb_info = {
        .arena          = arena,
        .region_count   = MAX_REGIONS,
    };
    cdb_init(&app->cdb, cdb_info);

    /* miscalleanous engine components */
    struct cdb_cmp_type_desc core_cmp_descs[] = {
        { CMP_ID_DT,    NULL, sizeof(usz),              1, },
        { CMP_ID_WINS,  NULL, sizeof(struct wm_window), CORE_WINDOW_TYPE_COUNT, },
    };

    /* base game components */
    struct cdb_cmp_type_desc base_cmp_descs[] = {
        { CMP_ID_DIR,   NULL,     sizeof(vec3), MAX_ENTITIES },
        { CMP_ID_ACCEL, c_accel,  sizeof(usz),  MAX_ENTITIES },
        { CMP_ID_SPEED, c_speed,  sizeof(usz),  MAX_ENTITIES },
        { CMP_ID_POS,   c_pos,    sizeof(vec3), MAX_ENTITIES },
    };

    struct m_array mod_array = {0};
    load_mods(&mod_array, init_conf);

    struct cdb_cmp_type_desc *mod_cmp_descs = mod_cmp_descs_array.data;

    struct cdb_cmp_region_info base_region_info = {
        .cmp_types      = base_cmp_descs,
        .cmp_type_count = ARRAY_SIZE(base_cmp_descs),
    };
    struct cdb_cmp_region_info core_region_info = {
        .cmp_types      = core_cmp_descs,
        .cmp_type_count = ARRAY_SIZE(core_cmp_descs),
    };
    cdb_register_cmp_region(&app->cdb, core_region_info);
    cdb_register_cmp_region(&app->cdb, base_region_info);

    init_mods(mod_array);
}

static void app_run(struct app_context *app)
{
    while (!app_should_close(app)) {

    }
}

static void app_shutdown(struct app_context *app)
{
    cdb_destroy(&app->cdb);
}

static b32 app_should_close(struct app_context *app)
{
    return wm_window_should_close(app->main_window);
}

#define STRIP_EXTENSION_A(file, ext) NULL_TERM_BUFF(str_duplicate(file), str_length(file) - CONST_STRLEN(ext))

static void load_mods(struct m_array *mod_array, const struct app_conf conf, struct m_arena *arena)
{
    /* save arena offset */
    ust arena_offset = 0;
    m_arena_save(arena, &arena_offset);

    INFO_LOG("entering mod directory: "STR_FMT, conf.mod_path);
    struct os_dir mod_directory = {0};
    struct os_dir_info mod_directory_info = {
        .path = conf.mod_path,
        .perm = DPERM_READ,
    };
    os_dir_open(&mod_directory, mod_directory_info);

    struct m_array mod_lib_names = {0};
    os_dir_get_file_paths(&mod_directory, &mod_lib_names, OS_RELATIVE_PATH);

    /* array containing all dynamic mod libraries */
    struct m_array mod_libs = {0};
    m_array_init(&mod_libs, sizeof(struct os_library), mod_lib_names.count);

    /* open each library */
    for (EACH_M_ARRAY(name, mod_lib_names, const char *)) {
        const char *stripped_name = NULL;
        cstr_token_ar(name, ".", &stripped_name, NULL, arena);

        struct os_library_info mod_lib_info = {
            .path = conf.mod_path,
            .name = stripped_name,
        };
        INFO_LOG("opening library "STR_QUOT_LIT(STR_FMT)"...", name);
        os_library_open(name, mod_lib_info);
    }

    os_dir_close(&mod_directory);

    /* reset arena pointer */
    m_arena_restore(arena, arena_offset);
}

static void init_mods(struct m_array *mod_array)
{
}

#define XYZ_COUNT 3
void c_pos(struct cdb *cdb)
{
}

static void c_speed(struct cdb_context *cdb)
{
    struct cmp_desc speed_desc = {0};
    cdb_get_cmp_desc(cdb, &speed_desc);

    usz *speed = speed_desc.data;
}

static void c_accel(struct cdb_context *cdb)
{

}

