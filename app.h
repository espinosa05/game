#ifndef __APP_H__
#define __APP_H__

#include <core/cli.h>
#include <core/types.h>

typedef struct {
    CLI_Args    cliArgs;
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
    usz         mainWindowState;
} App_Config;

void App_Init(App_Context *app, const App_ContextCreateInfo *info);
void App_Run(App_Context *app);
void App_Cleanup(App_Context *app);

#ifdef APP_IMPL

#include <core/memory.h>

u8 g_ArenaBuffer[MB_SIZE * 2] = {0};
M_Arena g_Arena = {0};

static void GetGlobalArena(M_Arena **arena)
{
    M_ArenaCreateInfo arenaInfo = {
        .external   = TRUE,
        .buffer     = g_ArenaBuffer,
        .memSize    = ARRAY_SIZE(g_ArenaBuffer),
    };
    M_ArenaInit(&g_Arena, &arenaInfo);
    *arena = &g_Arena;
}

static void DefaultConfig(Config *conf)
{
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
    OS_FileFlush(OS_STDOUT);
}
#undef NULL_ENTRY

enum {OPT_HELP, OPT_CONF,};
static void ParseCliArgs(Config *conf, const CLI_Args args)
{
    CLI_Opt cliOptions[] = {
        {OPT_HELP, 'h', "help",     NO_ARGUMENT,        "prints this screen",},
        {OPT_CONF, 'c', "config"    REQUIRED_ARGUMENT,  "sets a custom config path: '-c CONFIG_FILE', '--config CONFIG_FILE'",},
        {0},
    };

    for (usz i = 0; i < args.c; ) {
        CLI_OptResult option = {0};
        option = CLI_GetOpt(cliOptions, ARRAY_SIZE(cliOptions), &i);
        ASSERT(EMPTY_ERROR_REPORT(option.err), "failed to parse cli argument '%s': %s", args.v[i], option.err.rString);
        switch (option.val) {
        case OPT_HELP:
            PrintOptions(cliOptions, ARRAY_SIZE(cliOptions));
            OS_Exit(OS_EXIT_SUCCESS);
        case OPT_CONF:
            conf->configFilePath = option.arg;
            break;
        default: UNREACHABLE();
        }
    }
}

void App_Init(App_Context *app, const App_ContextCreateInfo *info)
{
    App_Config conf = {0};
    InitAppConfig(&conf, info->args);
    ApplyAppConfig(app, &conf);

    InitAppBackend(app);

    Scene mainMenu = {0};
    InitMainMenuScene(&mainMenu);
}

void App_Run(App_Context *app)
{

}

void App_Cleanup(App_Context *app)
{
    IMPL();
}

#endif /* APP_IMPL */
#endif /* __APP_H__ */
