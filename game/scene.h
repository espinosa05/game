#ifndef __SCENE_H__
#define __SCENE_H__

#include <core/types.h>
#include <core/sparse_set.h>

struct scene_context {
    struct sparse_index_set entity_id_set;
};

struct scene_callbacks {
    void (*init_scene) (struct scene_context *);
    void (*update_scene) (struct scene_context *);
    void (*close_scene) (struct scene_context *);
};

#endif /* __SCENE_H__ */
