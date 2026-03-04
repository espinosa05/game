#ifndef __BE_BE_ENGINE_H__
#define __BE_BE_ENGINE_H__

#include <core/memory.h>

struct be_engine_memory {
    struct m_arena permanent;
    struct m_arena transient;
};

struct be_engine {
    struct be_engine_memory memory;
    struct m_array          layers;
    struct m_array          events;
    struct wm               wm;
    struct wm_window        main_window;
    struct os_time          frame_start;
    struct os_time          frame_end;
    usz                     dt;
};

struct be_app_layer_desc {
    void (*init) (struct be_engine_memory *);
    void (*delete) (struct be_engine_memory *);
    void (*on_event) (struct m_array);
    void (*on_update) (struct be_engine_memory *);
    void (*suspend) (struct be_engine_memory *);
    void (*transition) (struct be_engine_memory *);
};

void be_engine_memory_init(struct be_engine_memory *be_engine_memory);

void be_engine_init(struct be_engine *be_engine);
void be_engine_run(struct be_engine *be_engine);
void be_engine_delete(struct be_engine *be_engine);

#endif /* __BE_BE_ENGINE_H__ */
