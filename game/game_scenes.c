#include "game_scenes.h"

#include <core/os.h>
#include <core/log.h>
#include <core/utils.h>

#define BASE_COMPONENT_ID(id) id
enum base_module_components {
    COMPONENT_ID_MOVERS,
    COMPONENT_ID_POSITIONS,

    COMPONENT_ID_AUDIO,
    COMPONENT_ID_TEXTURE,
    COMPONENT_ID_MESH,
};

#define BASE_ENTITY_ID(id) id
enum base_module_entities {
    ENTITY_ID_BLOCK,
    ENTITY_ID_ARMOR_BLOCK,

    ENTITY_ID_ARMOR_SLOPE,
    ENTITY_ID_ARMOR_SLOPE_CORNER,
    ENTITY_ID_ARMOR_SLOPE_CORNER_INV,

    ENTITY_ID_ARMOR_SLOPE_2x1,
    ENTITY_ID_ARMOR_SLOPE_2x1_CORNER,
    ENTITY_ID_ARMOR_SLOPE_2x1_CORNER_INV,

    ENTITY_ID_COMPUTER,
    ENTITY_ID_ASSEMBLER,
};

static void base_game_module_init(struct scene_context *scene)
{
    UNUSED(scene);
}

void init_game_scene(struct scene_context *scene)
{
    INFO_LOG("GAME SYSTEM INIT");

    base_game_module_init(scene);
}

void close_game_scene(struct scene_context *scene)
{
    INFO_LOG("GAME SYSTEM CLOSE");
    UNUSED(scene);
}
