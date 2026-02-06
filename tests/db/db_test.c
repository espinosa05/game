#include "cdb.h"

#include <core/types.h>

u8 buff[KB_SIZE*2] = {0};

struct app_context_info {
    const char      *name;
    struct cli_args args;
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

/* static function declaration start */
static void app_init(struct app_context *app, const struct app_context_info info);
static void app_run(struct app_context *app);
static void app_shutdown(struct app_context *app);

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

#define MAX_REGIONS 3

static void app_init(struct app_context *app, const struct app_context_info info)
{
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

    struct cdb_cmp_type_desc core_cmp_descs[] = {
        { CMP_ID_DT,    NULL, sizeof(usz), 1, },
        { CMP_ID_WINS,  NULL, sizeof(usz), 1, },
    };
    struct cdb_cmp_type_desc base_cmp_descs[] = {
        { CMP_ID_DIR,   NULL,     sizeof(vec3), MAX_ENTITIES },
        { CMP_ID_ACCEL, c_accel,  sizeof(usz),  MAX_ENTITIES },
        { CMP_ID_SPEED, c_speed,  sizeof(usz),  MAX_ENTITIES },
        { CMP_ID_POS,   c_pos,    sizeof(vec3), MAX_ENTITIES },
    };

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
}

static void app_run(struct app_context *app)
{

}

static void app_shutdown(struct app_context *app)
{
    cdb_destroy(&app->cdb);
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




