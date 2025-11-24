#ifndef __SCENE_H__
#define __SCENE_H__

#include <core/types.h>
#include <core/sparse_set.h>

struct scene_context {
    struct m_arena transient_arena;
    struct m_arena permanent_arena;

    struct sparse_index_set entity_id_set;
};

struct scene_context_info {
    struct m_arena transient_arena;
    struct m_arena permanent_arena;
};


void scene_context_init(struct scene_context *scene, const struct scene_context_info info);
#endif /* __SCENE_H__ */
