#include "app.h"

#include <core/platform.h>
#include <core/os.h>
#include <core/os_file.h>
#include <core/utils.h>
#include <core/log.h>

enum window_state {
    WINDOW_STATE_WINDOWED,
    WINDOW_STATE_FULLSCREEN,
    WINDOW_STATE_BORDERLESS_WINDOW,
};

/* static function declaration start */
static void default_app_config(struct app_config *conf);
static void init_app_memory(struct app_memory *memory);

static void parse_args(struct app_config *conf, const struct cli_args args);
static void parse_conf(struct app_config *conf, const char *config_path);

static void scene_context_info_init(struct scene_context_info *info, const struct app_memory memory);
/* static function declaration end */

void app_init(struct app *app, const struct app_info info)
{
    struct app_config config = {0};
    default_app_config(&config);

    parse_args(&config, info.args);
    parse_conf(&config, config.config_path);

    struct app_memory memory = {0};
    init_app_memory(&memory);

    struct scene_context scene_context = {0};
    struct scene_context_info scene_context_info = {
        .transient_arena = memory.transient,
        .permanent_arena = memory.permanent,
    };
    scene_context_init(&scene_context, scene_context_info);



    app->config = config;
    app->memory = memory;
    app->scene_context = scene_context;
}

void app_run(struct app *app)
{

}

void app_cleanup(const struct app app)
{
    INFO_LOG("cleaning up!");

    wm_shutdown(&wm);
}

#define DEFAULT_WINDOW_WIDTH 1200
#define DEFAULT_WINDOW_HEIGHT 720
#define DEFAULT_WINDOW_STATE WINDOW_STATE_WINDOWED
#define DEFAULT_CONFIG_PATH "config.yaml"

static void default_app_config(struct app_config *conf)
{
    *conf = (struct app_config) {
        .main_window_width  = DEFAULT_WINDOW_WIDTH,
        .main_window_height = DEFAULT_WINDOW_HEIGHT,
        .main_window_state  = DEFAULT_WINDOW_STATE,
        .config_path        = DEFAULT_CONFIG_PATH,
    };
}

#define PERMANENT_MEMORY_ARENA_SIZE (KB_SIZE*100)
#define TRANSIENT_MEMORY_ARENA_SIZE (KB_SIZE*200)

static struct {
    /* per scene memory */
    u8 ALIGNED(WORD_SIZE) permanent_memory[PERMANENT_MEMORY_ARENA_SIZE];
    /* per scene tick memory */
    u8 ALIGNED(WORD_SIZE) transient_memory[TRANSIENT_MEMORY_ARENA_SIZE];
} g_memory_pools;

#define APP_MEMORY_ARENA_SIZE (KB_SIZE*1)
static void init_app_memory(struct app_memory *memory)
{
    ASSERT_RUN_ONCE();

    /* memory arena that is reset after every tick */
    struct m_arena transient_arena = {0};
    struct m_arena_info transient_arena_info = {
        .external   = TRUE,
        .buffer     = g_memory_pools.transient_memory,
        .mem_size   = TRANSIENT_MEMORY_ARENA_SIZE,
    };
    m_arena_init(&transient_arena, transient_arena_info);

    /* memory arena that's reset for each new scene */
    struct m_arena permanent_arena = {0};
    struct m_arena_info permanent_arena_info = {
        .external   = TRUE,
        .buffer     = g_memory_pools.permanent_memory,
        .mem_size   = PERMANENT_MEMORY_ARENA_SIZE,
    };
    m_arena_init(&permanent_arena, permanent_arena_info);

    memory->transient = transient_arena;
    memory->permanent = permanent_arena;
}

static void parse_args(struct app_config *conf, const struct cli_args args)
{
    UNUSED(conf);

    enum opt_ids {
        OPT_HELP,
        OPT_VERS,
        OPT_RSLN,
        OPT_WNST,
    };

    struct cli_opt opts[] = {
        { OPT_HELP, 'h', "help", false, "prints help screen", },
        { OPT_VERS, 'v', "version", false, "prints version information", },
        { OPT_RSLN, 'r', "resolution", true, "sets custom startup resolution. Usage: -r [WIDTH]x[HEIGHT], --resolution [WIDTH]x[HEIGHT]", },
        { OPT_WNST, 'w', "window_state", true, "forces a window state at startup. Usage: -w [fullscreen:windowed:borderless], --window_state [fullscreen:windowed:borderless]", },
        {0},
    };

    for (usz i = 0; i < args.c; ) {
        struct cli_opt_result result = cli_getopt(opts, ARRAY_SIZE(opts), &i, args);
        ASSERT_RT(CLI_GETOPT_SUCCESS(result), "failed to parse commandline option \"%s\": %s",
                                             args.v[i],
                                             cli_getopt_string_error(result.err_code));

        switch (result.id) {
        case OPT_HELP:
            TODO("OPT_HELP");
            break;
        case OPT_VERS:
            TODO("OPT_VERS");
            break;
        case OPT_RSLN:
            TODO("OPT_RSLN");
            break;
        case OPT_WNST:
            TODO("OPT_WNST");
            break;
        }
    }
}

static void parse_conf(struct app_config *conf, const char *config_path)
{
    UNUSED(conf);
    UNUSED(config_path);
}


static void scene_context_info_init(struct scene_context_info *info,  const struct app_memory memory)
{
    info->transient_arena = memory.transient;
    info->permanent_arena = memory.permanent;
}
