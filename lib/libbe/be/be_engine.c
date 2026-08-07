#include <be/be_engine.h>

#include <core/cstr.h>
#include <core/hash.h>
#include <core/log.h>
#include <core/cli.h>
#include <core/str_builder.h>
#include <core/memory_macros.h>
#include <core/os_util.h>
#include <core/os_thread.h>
#include <core/os_directory.h>
#include <core/os_path.h>
#include <core/wm_utils.h>
#include <core/shader_compiler.h>

/* staic function declaration start */
static BeArenaChunk *be_arena_chunk_alloc(usz size);
/* static function declaration end */

void be_arena_chunk_init_ext(BeArenaChunk *chunk, void *base, usz size)
{
    chunk->next = NULL;
    chunk->count = size;
    chunk->used = 0;
    chunk->base = base;
}

BeArenaChunk *be_arena_chunk_extend(BeArenaChunk *chunk, usz size)
{
    BeArenaChunk *next = be_arena_chunk_alloc(size);
    chunk->next = next;
    return chunk->next;
}

void *be_arena_chunk_get(BeArenaChunk *chunk)
{
    return U8_PTR(chunk->base) + chunk->used;
}

void be_arena_init(BeArena *arena, usz init_size)
{
    arena->first = be_arena_chunk_alloc(init_size);
    arena->last = arena->first;
}

void *be_arena_alloc(BeArena *arena, usz chunk, usz count)
{
    BeArenaChunk *last = arena->last;

    usz alloc_size = chunk * count;
    if (alloc_size > last->count - last->used) {
        usz new_last_size = last->count / 2;
        new_last_size = new_last_size > alloc_size ? new_last_size : alloc_size;
        last = be_arena_chunk_extend(last, new_last_size);
    }

    void *buff = be_arena_chunk_get(last);
    last->used += alloc_size;
    return buff;
}

void be_arena_clear(BeArena *arena)
{
    UNUSED(arena);
}

void be_push_layer(BeEngine *be, BeLayerSpec spec)
{
    CHECK_NULL(spec.id_str);
    CHECK_NULL(spec.on_attach);
    CHECK_NULL(spec.on_detach);
    CHECK_NULL(spec.on_update);
    CHECK_NULL(spec.on_event);

    INFO_LOG("pushing new layer");
    BeLayer layer = {0};
    layer.id            = be_layer_id(spec.id_str);
    layer.context       = BE_CONTEXT(spec.on_attach(be));
    layer.on_detach     = spec.on_detach;
    layer.on_update     = spec.on_update;
    layer.on_event      = spec.on_event;
    mm_array_append(&be->layers, layer);

    INFO_LOG(BE_LAYER_SPEC_FMT, BE_LAYER_SPEC_FMT_ARG(spec));
    INFO_LOG("("STR_FMT") { context: "PTR_FMT" }", spec.id_str, layer.context);
}

u64 be_layer_id(const char *string)
{
    return hash_djb2(string);
}

BeLayer *be_get_layer_by_name(BeEngine *be, char *id_str)
{
    struct be_layer *target = NULL;
    u64 id = be_layer_id(id_str);

    for (EACH_BE_LAYER(layer, be->layers)) {
        if (layer->id == id) {
            target = layer;
        }
    }

    return target;
}

void be_detach_layer_by_name(BeEngine *be, char *id_str)
{
    struct be_layer *layer = be_get_layer_by_name(be, id_str);
    layer->on_detach(be, layer->context);

    {
        TODO("implement mm_array_remove_ref");
        usz remaining = layer - be->layers->data - 1;
        m_move(layer, layer + 1, remaining);
    }
}

BeLayer *be_get_layer_by_id(BeEngine *be, u64 id)
{
    BeLayers layers = be_engine->layers;
    for (usz i = 0; i < layers.count; ++i) {
        BeLayer *layer = &layers.data[i];
        if (!layer->active)
            continue;

        if (layer->id == id) {
            return layer;
        }
    }

    UNREACHABLE();
    return NULL;
}

void be_detach_layer_by_id(BeEngine *be, u64 id)
{
    struct be_layer *layer = be_get_layer_by_id(be, id_str);
    layer->on_detach(be, layer->context);
    usz index = mm_array_index_of(be->layers, layer);
    mm_array_remove_ref(be->layers, layer);
}

static BeArenaChunk *be_arena_chunk_alloc(usz size)
{
    BeArenaChunk *chunk = NULL;
    const usz alloc_size = sizeof(*chunk) + size;
    chunk = m_alloc(BYTE_SIZE, alloc_size);
    void *buffer = U8_PTR(chunk) + sizeof(*chunk);
    be_arena_chunk_init_ext(chunk, buffer, size);

    return chunk;
}

