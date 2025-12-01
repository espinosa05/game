#ifndef __SCENE_H__
#define __SCENE_H__

#include <core/types.h>
#include <core/sparse_set.h>
#include "cdb.h"

struct scene_context;

typedef void (*System_Callback) (struct scene_context *context);
struct system_callbacks {
    System_Callback *sys_init;
    System_Callback *sys_tick;
    System_Callback *sys_stop;
    usz count;
};

struct scene_context {
    struct m_arena transient_arena;
    struct m_arena permanent_arena;

    struct sparse_index_set entity_id_set;
    struct sparse_index_set component_id_set;

    struct cdb_context      cmp_database;
    struct system_callbacks systems;
};

struct scene_context_info {
    struct m_arena transient_arena;
    struct m_arena permanent_arena;

};

void scene_context_init(struct scene_context *scene, const struct scene_context_info info);
#endif /* __SCENE_H__ */
