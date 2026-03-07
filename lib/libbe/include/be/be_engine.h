#ifndef __BE_BE_ENGINE_H__
#define __BE_BE_ENGINE_H__

#include <core/wm.h>
#include <core/os.h>

struct be_engine_memory {
    struct m_arena permanent;
    struct m_arena transient;
};

struct be_engine {
    /* shared engine and application memory */
    struct be_engine_memory memory;

    /* windowing and inputs */
    struct wm           wm;
    struct wm_window    main_window;
    struct wm_events    events;

    /* app layers */
    struct be_app_layer_stack               layers;
    struct be_app_layer_transition_queue    layer_transitions;

    /* delta time context */
    struct os_time  frame_start;
    struct os_time  frame_end;
    usz             dt;
};

void be_engine_memory_init(struct be_engine_memory *be_engine_memory);

void be_engine_init(struct be_engine *be_engine);
void be_engine_run(struct be_engine *be_engine);
void be_engine_delete(struct be_engine *be_engine);

#endif /* __BE_BE_ENGINE_H__ */
