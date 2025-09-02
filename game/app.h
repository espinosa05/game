#ifndef __APP_H__
#define __APP_H__

#include <core/types.h>
#include <core/memory.h>

typedef struct {
    CLI_Args    args;
    const char  *name;
} App_ContextCreateInfo;

typedef struct {
    M_Arena         *memoryArena;
    Scene_Context   *currentScene;
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

#endif /* __APP_H__ */
