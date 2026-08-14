#ifndef __BE_BE_ENGINE_H__
#define __BE_BE_ENGINE_H__

#include <be/be_layer.h>
#include <be/be_arena.h>
#include <be/be_event.h>
#include <core/types.h>
#include <core/os.h>
#include <core/cli.h>

typedef struct {
    MM_ARRAY_MEMBERS(BeLayer);
} BeLayers;

typedef struct {
    usz type;
    union {
        char        *name;
        BeLayerSpec spec;
    };
} BeLayerTransition;

enum e_be_layer_transition_types {
    BE_LAYER_TRANSITION_TYPE_NULL = 0,
    BE_LAYER_TRANSITION_TYPE_ATTACH_LAYER,
    BE_LAYER_TRANSITION_TYPE_ATTACH_OVERLAY,
    BE_LAYER_TRANSITION_TYPE_DETACH,
    BE_LAYER_TRANSITION_TYPE_SUSPEND,
    BE_LAYER_TRANSITION_TYPE_ACTIVATE,
};

typedef struct {
    MM_QUEUE_MEMBERS(BeLayerTransition);
} BeLayerTransitionQueue;

typedef struct {
    struct os_time start;
    struct os_time end;
} BeFrameTime;

typedef struct s_be_engine BeEngine;
struct s_be_engine {
    b32 run;
    f64 dt;

    BeEventQueue    events;
    BeFrameTime     frame_time;
    BeArena         transient;
    BeArena         permanent;

    BeLayers                layers;
    BeLayers                overlays;
    /* transition queue for both layers and overlays */
    BeLayerTransitionQueue  layer_transitions;
};

void be_engine_init(BeEngine *be, struct cli_args args);
void be_engine_run(BeEngine *be);
void be_engine_delete(BeEngine *be);

void *be_alloc_perm(BeEngine *be, usz chunk, usz count);
void *be_alloc_tran(BeEngine *be, usz chunk, usz count);

#endif /* __BE_BE_BENGINE_H__ */
