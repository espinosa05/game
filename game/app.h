#ifndef __APP_H__
#define __APP_H__

#include <core/types.h>
#include <core/memory.h>
#include <core/cli.h>

struct app_info {
    struct cli_args args;
    const char *name;
};

struct app_scenes {
    struct scene *scene;
    usz count;
};

struct app {
    struct app_scenes   scenes;
    struct m_arena      *memory_arena;
};

void app_init(struct *app, const struct app_info info);
void app_run(struct *app);
void app_cleanup(struct *app);

#endif /* __APP_H__ */
