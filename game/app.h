#ifndef __APP_H__
#define __APP_H__

#include <core/types.h>
#include <core/memory.h>
#include <core/cli.h>

#include "scene.h"
#include "game_scenes.h"

enum scene_ids {
    MENU_SCENE  = 0,
    GAME_SCENE  = 1,
    INTRO_SCENE = 2,

    SCENE_COUNT,
};

struct app_info {
    struct cli_args args;
    const char *name;
};

struct app_config {
    usz         main_window_width;
    usz         main_window_height;
    usz         main_window_state;
    const char  *config_path;
};

struct app_memory {
    struct m_arena transient;
    struct m_arena permanent;
};

struct app {
    struct app_memory       memory;
    struct app_config       config;

    struct scene_context    scene_context;
};

void app_init(struct app *app, const struct app_info info);
void app_run(struct app *app);
void app_cleanup(const struct app app);

#endif /* __APP_H__ */
