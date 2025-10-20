#include "app.h"

#include <core/os.h>
#include <core/os_file.h>

enum window_state {
    WINDOW_STATE_WINDOWED,
    WINDOW_STATE_FULLSCREEN,
    WINDOW_STATE_BORDERLESS_WINDOW,
};

struct app_config {
    usz         main_window_width;
    usz         main_window_height;
    usz         main_window_state;
    const char  *config_path;
};

/* static function declaration start */
static void init_app_arena(struct app *app);
static void parse_args(struct app_config *conf, const struct cli_args args);
static void parse_conf(struct app_config *conf, const char *config_path);
/* static function declaration end */

void app_init(struct app *app, const struct app_info info)
{
    init_app_arena(app);

    struct app_config config = {0};
    struct app_scenes scenes = {0};

    parse_args(&config, info->args);
    parse_conf(&config, config->config_path);

    app->config = config;
    app->scenes = scenes;
}

void app_run(struct app *app)
{
    UNUSED(app);
}

void app_cleanup(struct app *app)
{
    UNUSED(app);
}

#define MEMORY_ARENA_SIZE (MB_SIZE*100)
static void init_app_arena(struct app *app)
{
    static b32 once_flag = FALSE;
    ASSERT_RT(once_flag == FALSE, "[BUG] function is meant to be called only once!");

    static struct m_arena arena = {0};
    static struct m_arena_info arena_info = {
        .external   = TRUE,
        .buffer     = (static u8[MEMORY_ARENA_SIZE]) {0},
        .mem_size   = MEMORY_ARENA_SIZE,
    };

    m_arena_init(&arena, arena_info);
    app->memory_arena = &arena;

    once_flag = TRUE;
}

static void parse_args(struct app_config *conf, const struct cli_args args)
{
    enum opt_ids {
        OPT_HELP,
        OPT_VERS,
        OPT_RSLN,
        OPT_WNST,
    };
    struct cli_opt opts[] = {
        { OPT_HELP, 'h', "help", false, "prints help screen", },
        { OPT_VERS, 'v', "version", false "prints version information", },
        { OPT_RSLN, 'r', "resolution", true, "sets custom startup resolution. Usage: -r [WIDTH]x[HEIGHT], --resolution [WIDTH]x[HEIGHT]", },
        { OPT_WNST, 'w', "window_state", true, "forces a window state at startup. Usage: -w [fullscreen:windowed:borderless], --window_state [fullscreen:windowed:borderless]", },
        {0},
    };

    for (usz i = 0; i < ARRAY_SIZE(opts); ) {
        struct cli_opt_result result = cli_getopt(&opts, ARRAY_SIZE(opts), &i, args);
        ASSERT_RT(CLI_GETOP_SUCCESS(result), "failed to parse commandline option \"%s\": %s",
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
