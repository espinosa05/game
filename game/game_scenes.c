#include "game_scenes.h"

#include <core/os.h>
#include <core/log.h>
#include <core/utils.h>


void init_game_scene(struct scene_context *scene)
{
    UNUSED(scene);
    INFO_LOG("GAME SYSTEM INIT");
}

void close_game_scene(struct scene_context *scene)
{
    INFO_LOG("GAME SYSTEM CLOSE");
    UNUSED(scene);
}
