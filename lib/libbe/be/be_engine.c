#include <be/be_engine.h>
#include <be/be_core.h>
#include <be/be_window.h>
#include <be/be_render.h>
#include <be/be_app_entry.h>

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
static void init_layers(BeEngine *be, usz init_count);
static void init_overlays(BeEngine *be, usz init_count);
static void add_layer(BeEngine *be, BeLayerSpec spec);
static void add_overlay(BeEngine *be, BeLayerSpec spec);
CORE_INLINE static void update_layers(BeEngine *be);
static void be_transitions_init(BeLayerTransitionQueue *transitions);
CORE_INLINE static void transition_layers(BeEngine *be);
CORE_INLINE static void transition_layer(BeEngine *be, BeLayerTransition transition);
static BeArenaChunk *be_arena_chunk_alloc(usz size);
static void be_frame_time_init(BeFrameTime *frame_time);
CORE_INLINE static void frame_time_start(BeEngine *be);
CORE_INLINE static void frame_time_end(BeEngine *be);
static b32 should_close(BeEngine *be);
static void cleanup_layers(BeEngine *be);
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
    ASSERT(!(alloc_size > last->count - last->used), "Arena-OOM");

#if 0
    if (alloc_size > last->count - last->used) {
        usz new_last_size = last->count / 2;
        new_last_size = new_last_size > alloc_size ? new_last_size : alloc_size;
        last = be_arena_chunk_extend(last, new_last_size);
    }
#endif

    void *buff = be_arena_chunk_get(last);
    INFO_LOG("be_arena allocation: "PTR_FMT, buff);
    last->used += alloc_size;
    return buff;
}

void be_arena_clear(BeArena *arena)
{
    UNUSED(arena);
}

void be_arena_delete(BeArena *arena)
{
    be_arena_clear(arena);
}

void *be_alloc_perm(BeEngine *be, usz chunk, usz count)
{
    return be_arena_alloc(&be->permanent, chunk, count);
}

void *be_alloc_tran(BeEngine *be, usz chunk, usz count)
{
    return be_arena_alloc(&be->transient, chunk, count);
}

void be_push_layer(BeEngine *be, BeLayerSpec spec)
{
    BeLayerTransition push_transition = {0};
    push_transition.type = BE_LAYER_TRANSITION_TYPE_ATTACH_LAYER;
    push_transition.spec = spec;
    mm_queue_enqueue(&be->layer_transitions, push_transition);
    INFO_LOG("PUSH LAYER: "STR_FMT, spec.id_str);
}

void be_push_overlay(BeEngine *be, BeLayerSpec spec)
{
    BeLayerTransition push_transition = {0};
    push_transition.type = BE_LAYER_TRANSITION_TYPE_ATTACH_OVERLAY;
    push_transition.spec = spec;
    mm_queue_enqueue(&be->layer_transitions, push_transition);
    INFO_LOG("PUSH OVERLAY: "STR_FMT, spec.id_str);
}

u64 be_layer_id(const char *string)
{
    return hash_djb2(string);
}

BeLayer *be_get_layer_by_name(BeEngine *be, char *id_str)
{
    BeLayer *target = NULL;
    u64 id = be_layer_id(id_str);

    for (EACH_BE_LAYER(layer, be->layers)) {
        if (layer->id == id) {
            target = layer;
        }
    }

    for (EACH_BE_LAYER(overlay, be->overlays)) {
        if (overlay->id == id) {
            target = overlay;
        }
    }

    return target;
}

void be_detach_layer_by_name(BeEngine *be, char *id_str)
{
    BeLayerTransition transition = {0};
    transition.type = BE_LAYER_TRANSITION_TYPE_DETACH;
    transition.name = id_str;
    mm_queue_enqueue(&be->layer_transitions, transition);
}

BeLayer *be_get_layer_by_id(BeEngine *be, u64 id)
{
    BeLayers layers = be->layers;
    for (usz i = 0; i < layers.count; ++i) {
        BeLayer *layer = &layers.data[i];
        if (!layer->active)
            continue;

        if (layer->id == id) {
            return layer;
        }
    }

    BeLayers overlays = be->overlays;
    for (usz i = 0; i < overlays.count; ++i) {
        BeLayer *overlay = &overlays.data[i];
        if (!overlay->active)
            continue;

        if (overlay->id == id) {
            return overlay;
        }
    }

    UNREACHABLE();
    return NULL;
}

void be_detach_layer_by_id(BeEngine *be, u64 id)
{
    BeLayer *layer = be_get_layer_by_id(be, id);
    layer->on_detach(be, layer->context);
    usz index = mm_array_get_index(&be->layers, layer);
    mm_array_remove(&be->layers, index);
}

#define BLEEDING_EDGE_INIT_LAYER_COUNT 8
#define BLEEDING_EDGE_EVENT_QUEUE_LENGTH USZ(128)

void be_engine_init(BeEngine *be, struct cli_args args)
{
    be->run = TRUE;
    be->dt = F64(0);
    be_frame_time_init(&be->frame_time);
    be_arena_init(&be->permanent, MB(1));
    be_arena_init(&be->transient, MB(1));

    be_transitions_init(&be->layer_transitions);

    init_layers(be, BLEEDING_EDGE_INIT_LAYER_COUNT);
    init_overlays(be, BLEEDING_EDGE_INIT_LAYER_COUNT);

    be_push_layer(be, BE_CORE_LAYER_SPEC);

    be_app_entry(be, args);

    be_push_overlay(be, BE_WINDOW_LAYER_SPEC);
    be_push_overlay(be, BE_RENDER_LAYER_SPEC);
}

void be_engine_run(BeEngine *be)
{

    while (!should_close(be)) {
        frame_time_start(be);

        transition_layers(be);

        update_layers(be);

        frame_time_end(be);
    }

    cleanup_layers(be);
}

void be_engine_delete(BeEngine *be_engine)
{
    UNUSED(be_engine);
}

static void init_layers(BeEngine *be, usz init_count)
{
    BeLayer *buff = be_alloc_perm(be, sizeof(*buff), init_count);
    mm_array_init_ext(&be->layers, buff, init_count);
}

static void init_overlays(BeEngine *be, usz init_count)
{
    BeLayer *buff = be_alloc_perm(be, sizeof(*buff), init_count);
    mm_array_init_ext(&be->overlays, buff, init_count);
}

static BeArenaChunk *be_arena_chunk_alloc(usz size)
{
    BeArenaChunk *chunk = NULL;
    const usz alloc_size = sizeof(*chunk) + size;
    chunk = m_alloc(BYTE_SIZE, alloc_size);
    INFO_LOG("chunk alloc: "PTR_FMT, chunk);
    void *buffer = U8_PTR(chunk) + sizeof(*chunk);
    be_arena_chunk_init_ext(chunk, buffer, size);

    return chunk;
}

static void be_frame_time_init(BeFrameTime *frame_time)
{
    os_time_init(&frame_time->start);
    os_time_init(&frame_time->end);
}

static b32 should_close(BeEngine *be)
{
    return !be->run;
}

static void frame_time_start(BeEngine *be)
{
    os_time_get_monotonic(&be->frame_time.start);
}

static void frame_time_end(BeEngine *be)
{
    os_time_get_monotonic(&be->frame_time.end);

    struct os_time frame_time = {0};
    os_time_get_diff(&frame_time, be->frame_time.start, be->frame_time.end);
    be->dt = os_time_get_msec(frame_time);
}

static void add_layer(BeEngine *be, BeLayerSpec spec)
{
    CHECK_NULL(spec.id_str);
    CHECK_NULL(spec.on_attach);
    CHECK_NULL(spec.on_detach);
//    CHECK_NULL(spec.on_update);
//    CHECK_NULL(spec.on_event);

    BeLayer layer = {0};
    layer.active        = spec.active;
    layer.id            = be_layer_id(spec.id_str);
    layer.context       = BE_CONTEXT(spec.on_attach(be));
    layer.on_detach     = spec.on_detach;
    layer.on_update     = spec.on_update;
    layer.on_event      = spec.on_event;
    mm_array_append(&be->layers, layer);

    INFO_LOG("{ context: "PTR_FMT" } "BE_LAYER_SPEC_FMT, layer.context, BE_LAYER_SPEC_FMT_ARG(spec));
}

static void update_layers(BeEngine *be)
{
    BeLayers layers = be->layers;
    for (EACH_BE_LAYER(layer, layers)) {
        if (LIKELY(layer->active)) {
            layer->on_update(be, layer->context);
        }
    }

    BeLayers overlays = be->overlays;
    for (EACH_BE_LAYER(overlay, overlays)) {
        if (LIKELY(overlay->active)) {
            overlay->on_update(be, overlay->context);
        }
    }
}

static void transition_layers(BeEngine *be)
{
    {
        BeLayerTransitionQueue *transitions = &be->layer_transitions;
        usz length = mm_queue_length(transitions);
        for (usz i = 0; i < length; ++i) {
            INFO_LOG("dequeueing layer "USZ_FMT" of "USZ_FMT, i, length);
            BeLayerTransition transition = {0};
            mm_queue_dequeue(transitions, &transition);
            transition_layer(be, transition);
        }
    }
}

static void transition_layer(BeEngine *be, BeLayerTransition transition)
{
    BeLayer *layer = NULL;

    switch (transition.type) {
    case BE_LAYER_TRANSITION_TYPE_ATTACH_OVERLAY:
        INFO_LOG("BE_LAYER_TRANSITION_TYPE_ATTACH_OVERLAY");
        add_layer(be, transition.spec);
        break;
    case BE_LAYER_TRANSITION_TYPE_ATTACH_LAYER:
        INFO_LOG("BE_LAYER_TRANSITION_TYPE_ATTACH_LAYER");
        add_overlay(be, transition.spec);
        break;
    case BE_LAYER_TRANSITION_TYPE_DETACH:
        INFO_LOG("BE_LAYER_TRANSITION_TYPE_DETACH");
        layer = be_get_layer_by_name(be, transition.name);
        layer->on_detach(be, layer->context);
        break;
    case BE_LAYER_TRANSITION_TYPE_SUSPEND:
        INFO_LOG("BE_LAYER_TRANSITION_TYPE_SUSPEND");
        layer = be_get_layer_by_name(be, transition.name);
        layer->active = FALSE;
        if (layer->on_suspend)
            layer->on_suspend(be, layer->context);
        break;
    case BE_LAYER_TRANSITION_TYPE_ACTIVATE:
        INFO_LOG("BE_LAYER_TRANSITION_TYPE_ACTIVATE");
        layer = be_get_layer_by_name(be, transition.name);
        layer->active = TRUE;
        if (layer->on_activate)
            layer->on_activate(be, layer->context);
        break;
    case BE_LAYER_TRANSITION_TYPE_NULL:
        INFO_LOG("BE_LAYER_TRANSITION_TYPE_NULL");
        UNREACHABLE();
    }
}

static void add_overlay(BeEngine *be, BeLayerSpec spec)
{
    CHECK_NULL(spec.id_str);
    CHECK_NULL(spec.on_attach);
    CHECK_NULL(spec.on_detach);
//    CHECK_NULL(spec.on_update);
//    CHECK_NULL(spec.on_event);

    BeLayer layer = {0};
    layer.active        = spec.active;
    layer.id            = be_layer_id(spec.id_str);
    layer.context       = BE_CONTEXT(spec.on_attach(be));
    layer.on_detach     = spec.on_detach;
    layer.on_update     = spec.on_update;
    layer.on_event      = spec.on_event;
    mm_array_append(&be->overlays, layer);

    INFO_LOG("{ context: "PTR_FMT" } "BE_LAYER_SPEC_FMT, layer.context, BE_LAYER_SPEC_FMT_ARG(spec));
}

static void be_transitions_init(BeLayerTransitionQueue *transitions)
{
    mm_queue_init(transitions, 16);
}

static void cleanup_layers(BeEngine *be)
{
    BeLayers layers = be->layers;
    for (EACH_BE_LAYER(layer, layers)) {
        layer->on_detach(be, layer->context);
    }
}

