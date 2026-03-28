#ifndef __BE_BE_ENGINE_H__
#define __BE_BE_ENGINE_H__

#include <core/wm.h>
#include <core/os.h>
#include <core/memory_macros.h>
#include <be/be_app_entry.h>

struct be_app_layers {
    MM_ARRAY_MEMBERS(struct be_app_layer);
};

struct be_app_layer_transition {
    struct be_app_layer *layer;
    usz layer_index;
};

struct be_app_layer_transition_queue {
    MM_QUEUE_MEMBERS(struct be_app_layer_transition);
};



struct be_engine {
    /* shared engine and application memory */
    struct be_engine_memory memory;

    /* windowing and inputs */
    struct wm           wm;
    struct wm_window    main_window;
    struct wm_events    events;

    /* app layers */
    struct be_app_layers                    layers;
    struct be_app_layer_transition_queue    layer_transitions;

    /* delta time context */
    struct os_time  frame_start;
    struct os_time  frame_end;
    usz             dt;
};

void be_init_layers(struct be_engine *be_engine, usz layer_count);
void be_push_layer(struct be_engine *be_engine, struct be_app_layer layer);
void be_engine_memory_init(struct be_engine_memory *be_engine_memory);
void be_engine_init(struct be_engine *be_engine);
void be_engine_add_layer(struct be_engine *be_engine, struct be_app_layer *layer);
void be_engine_run(struct be_engine *be_engine);
void be_engine_delete(struct be_engine *be_engine);

#endif /* __BE_BE_ENGINE_H__ */
