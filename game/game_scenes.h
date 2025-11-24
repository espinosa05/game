#ifndef __GAME_SCENES_H__
#define __GAME_SCENES_H__

#include "scene.h"

void init_intro_scene(struct scene_context *scene);
void close_intro_scene(struct scene_context *scene);

void init_menu_scene(struct scene_context *scene);
void close_menu_scene(struct scene_context *scene);

void init_game_scene(struct scene_context *scene);
void close_game_scene(struct scene_context *scene);

#endif /* __GAME_SCENES_H__ */
