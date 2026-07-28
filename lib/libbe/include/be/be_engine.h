#ifndef __BE_BE_ENGINE_H__
#define __BE_BE_ENGINE_H__

#include <core/wm.h>
#include <core/os.h>
#include <core/memory_arena.h>
#include <core/memory_macros.h>
#include <be/be_app_entry.h>

struct be_app_layers {
    MM_ARRAY_MEMBERS(struct be_app_layer);
};

struct be_allocator_chunk;

struct be_allocator_chunk {
    struct m_arena arena;
    struct be_allocator_chunk *next;
};

struct be_allocator {
    struct be_allocator_chunk *first;
    struct be_allocator_chunk *last;
};

struct be_memory {
    struct be_allocator permanent;
    struct be_allocator transient;
};

struct be_memory_info {
};

struct be_engine {
    struct be_memory memory;
};

void be_push_layer(struct be_engine *be_engine, struct be_app_layer_spec layer);
void be_engine_init(struct be_engine *be_engine, struct cli_args args);
void be_engine_run(struct be_engine *be_engine);
void be_engine_delete(struct be_engine *be_engine);
void be_engine_set_close(volatile struct be_engine *be_engine);

#endif /* __BE_BE_ENGINE_H__ */
