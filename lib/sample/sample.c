#include <be/project.h>

#include "menu_layer.h"
#include "settings_layer.h"
#include "game_layer.h"

void be_app_load_layers(struct app *app)
{
    struct m_arena *init_memory = app->memory.permanent;

    be_app_layers_push(&app->layers, menu_layer_get(init_memory));
    be_app_layers_push(&app->layers, settings_layer_get(init_memory));
    be_app_layers_push(&app->layers, game_layer_get(init_memory));
}
