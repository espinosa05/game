#include "scene.h"

#define MAX_SCENE_ENTITY_COUNT 1000

void scene_context_init(struct scene_context *scene, const struct scene_context_info info)
{
    scene->permanent_arena = info.permanent_arena;
    scene->transient_arena = info.transient_arena;
}

