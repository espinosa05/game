#include "app.h"

#include <core/os.h>
#include <core/os_file.h>


/* static function declaration start */
static void GetAppArena(M_Arena **arena);
static void PrintOptions(const CLI_Opt *opts, usz count);
static void ParseCliArgs(App_Config *conf, const CLI_Args args);
/* static function declaration end */

void App_Init(App_Context *app, const App_ContextCreateInfo *info)
{
    App_Config conf = {0};
    ParseCliArgs(&conf, info->args);
    GetAppArena(&app->memoryArena);

    Scene_Context *mainMenuScene = NULL;
    mainMenuScene = M_ArenaAlloc(app->memoryArena, sizeof(mainMenuScene), 1);
    CreateMainMenuScene(mainMenuScene);

    app->currentScene = mainMenuScene;
    OpenMainWindow(&app->mainWindow);
}

void App_Run(App_Context *app)
{
    UNUSED(app);
}

void App_Cleanup(App_Context *app)
{
    UNUSED(app);
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
        OS_FilePrintf(OS_STDOUT, "-%c, --%s: %s\n", opts[i].shortOpt,
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
                                                        "'--winres help' to get a list of supported resolutions"},
        {0},
    };

    for (usz i = 1; i < args.c; ) {
        CLI_OptResult option = {0};
        option = CLI_GetOpt(cliOptions, ARRAY_SIZE(cliOptions), &i, args);
        if (!CLI_GETOPT_SUCCESS(option)) {
            ERROR_LOG("failed to parse option: %s : (0x%X) %s", args.v[option.optInd], option.errCode, CLI_GetOptStringError(option.errCode));
        }

        switch (option.id) {
        case OPT_HELP:
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

