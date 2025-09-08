#include "app.h"

#include <core/os.h>
#include <core/os_file.h>

enum WindowState {
    WINDOW_STATE_WINDOWED,
    WINDOW_STATE_FULLSCREEN,
    WINDOW_STATE_BORDERLESS_WINDOW,
};

typedef struct {
    usz         mainWindowWidth;
    usz         mainWindowHeight;
    const char  *configPath;
    usz         mainWindowInitState;
} App_Config;

/* static function declaration start */
static void GetAppArena(M_Arena **arena);
static void PrintOptions(const CLI_Opt *opts, usz count);
static void ParseCliArgs(App_Config *conf, const CLI_Args args);
/* static function declaration end */

void App_Init(App_Context *app, const App_ContextCreateInfo *info)
{
    App_Config conf = {0};
    ParseCliArgs(&conf, info->args);
    TODO("ParseConfigFile");
    GetAppArena(&app->memoryArena);

    Kiek_ApplicationVersionHeader version = {
        .major = 0,
        .minor = 0,
        .patch = 1,
    };
    Kiek_VulkanContextCreateInfo kvkInfo = {
        .appName            = info->name,
        .appVersionHeader   = &version,
    };
    Kiek_VulkanStartup(&app->kvk, kvkInfo);
}

void App_Run(App_Context *app)
{
    UNUSED(app);
}

void App_Cleanup(App_Context *app)
{
    Kiek_VulkanShutdown(&app->kvk);
}

static void GetAppArena(M_Arena **arena)
{
    static u8 appArenaBuffer[2*MB_SIZE];
    static M_Arena appArena = {0};
    static b32 onceFlag = FALSE;
    ASSERT_RT(onceFlag == FALSE, "The function was meant to be called once!");

    M_ArenaCreateInfo appArenaInfo = {
        .external   = TRUE,
        .buffer     = appArenaBuffer,
        .memSize    = ARRAY_SIZE(appArenaBuffer),
    };
    M_ArenaInit(&appArena, &appArenaInfo);
    *arena = &appArena;

    onceFlag = TRUE;
}

static void PrintOptions(const CLI_Opt *opts, usz count)
{
    for (usz i = 0; i < count - CLI_OPT_NULL_ENTRY; ++i) {
        LOG("\x20-%c, --%s\t%s\n", opts[i].shortOpt,
                                     opts[i].longOpt,
                                     opts[i].desc);
    }
}

enum {OPT_HELP, OPT_CONF, OPT_WINR,};
static void ParseCliArgs(App_Config *conf, const CLI_Args args)
{
    CLI_Opt cliOptions[] = {
        {OPT_HELP, 'h', "help",     NO_ARGUMENT,        "prints this screen",},
        {OPT_CONF, 'c', "config",   REQUIRED_ARGUMENT,  "sets a custom config path: '-c CONFIG_FILE', '--config CONFIG_FILE'",},
        {OPT_WINR, 'w', "winres",   REQUIRED_ARGUMENT,  "sets a custom resolution: '-w WIDTHxHEIGHT', '--winres WIDTHxHEIGHT', "
                                                        "use '--winres help' to get a list of supported resolutions"},
        {0},
    };

    for (usz i = 1; i < args.c; ) {
        CLI_OptResult option = {0};
        option = CLI_GetOpt(cliOptions, ARRAY_SIZE(cliOptions), &i, args);
        if (!CLI_GETOPT_SUCCESS(option)) {
            ERROR_LOG("failed to parse option: %s : (0x%X) %s", args.v[option.optInd], option.errCode, CLI_GetOptStringError(option.errCode));
            OS_Exit(OS_EXIT_FAILURE);
        }

        switch (option.id) {
        case OPT_HELP:
            LOG("Usage: %s [OPTIONS]\n", *args.v);
            LOG("Options:\n");
            PrintOptions(cliOptions, ARRAY_SIZE(cliOptions));
            OS_Exit(OS_EXIT_SUCCESS);
        case OPT_CONF:
            conf->configPath = option.arg;
            break;
        case OPT_WINR:
            TODO("setting resolution is not supported yet!");
            INFO_LOG("continuing without custom resolution...");
            break;
        default: UNREACHABLE();
        }
    }
}

