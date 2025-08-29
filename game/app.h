#ifndef __APP_H__
#define __APP_H__

#include <core/cli.h>
#include <core/types.h>
#include <core/memory.h>
#include <core/os.h>
#include <core/os_file.h>

typedef struct {
    CLI_Args    args;
    const char  *name;
} App_ContextCreateInfo;

typedef struct {
    M_Arena *memoryArena;
} App_Context;

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

void App_Init(App_Context *app, const App_ContextCreateInfo *info);
void App_Run(App_Context *app);
void App_Cleanup(App_Context *app);

#ifdef APP_IMPL

static void GetAppArena(M_Arena **arena);
static void PrintOptions(const CLI_Opt *opts, usz count);
static void ParseCliArgs(App_Config *conf, const CLI_Args args);

static void GetAppArena(M_Arena **arena)
{
    static u8 appArenaBuffer[2*MB_SIZE];
    static M_Arena appArena = {0};
    static b32 onceFlag = FALSE;
    ASSERT_RT(onceFlag, "The function was meant to be called once!");

    M_ArenaCreateInfo appArenaInfo = {
        .external   = TRUE,
        .buffer     = appArenaBuffer,
        .memSize    = ARRAY_SIZE(appArenaBuffer),
    };
    M_ArenaInit(&appArena, &appArenaInfo);
    *arena = &appArena;

    onceFlag = TRUE;
}

#define NULL_ENTRY 1
static void PrintOptions(const CLI_Opt *opts, usz count)
{
    count += NULL_ENTRY;
    for (usz i = 0; i < count; ++i) {
        OS_FilePrintf(OS_STDOUT, "-%c, --%s: %s\n", opts[i].shortOpt,
                                                    opts[i].longOpt,
                                                    opts[i].desc);
    }
}
#undef NULL_ENTRY

enum {OPT_HELP, OPT_CONF, OPT_WINR,};
static void ParseCliArgs(App_Config *conf, const CLI_Args args)
{
    CLI_Opt cliOptions[] = {
        {OPT_HELP, 'h', "help",     NO_ARGUMENT,        "prints this screen",},
        {OPT_CONF, 'c', "config",   REQUIRED_ARGUMENT,  "sets a custom config path: '-c CONFIG_FILE', '--config CONFIG_FILE'",},
        {OPT_WINR, 'w', "winres",   REQUIRED_ARGUMENT,  "sets a custom resolution: '-w WIDTHxHEIGHT', '--winres WIDTHxHEIGHT', "
                                                        "'--winsize help' to get a list of supported resolutions"},
        {0},
    };

    for (usz i = 0; i < args.c; ) {
        CLI_OptResult option = {0};
        option = CLI_GetOpt(cliOptions, ARRAY_SIZE(cliOptions), &i, args);
        ASSERT_RT(0 == option.errCode, "failed to parse cli argument '"STR_FMT"'("SZ_FMT"): "STR_FMT,
                                    args.v[i], option.errCode, option.errReport.rString);
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

void App_Init(App_Context *app, const App_ContextCreateInfo *info)
{
    App_Config conf = {0};
    ParseCliArgs(&conf, info->args);
    GetAppArena(&app->memoryArena);
}

void App_Run(App_Context *app)
{
    UNUSED(app);
    IMPL();
}

void App_Cleanup(App_Context *app)
{
    UNUSED(app);
    IMPL();
}

#endif /* APP_IMPL */
#endif /* __APP_H__ */
