#ifndef __BE_BE_ENGINE_H__
#define __BE_BE_ENGINE_H__

#include <core/memory.h>

struct be_engine_memory {
    struct m_arena permanent;
    struct m_arena transient;
};

struct be_engine {
    struct be_engine_memory                 memory;
    struct be_app_layers                    app_layers;
    struct wm                               wm;
    struct wm_window                        main_window;

    struct m_static_array(struct wm_event)  events;
    struct os_time                          frame_start;
    struct os_time                          frame_end;
    usz dt;
};

struct be_app_layer_desc {
    decl_func_ptr(void, init,         struct be_engine_memory *);
    decl_func_ptr(void, delete,       struct be_engine_memory *);

    decl_func_ptr(void, on_event,     struct m_static_array(struct wm_event));
    decl_func_ptr(void, on_update,    struct be_engine_memory *);

    decl_func_ptr(void, suspend,      struct be_engine_memory *);
    decl_func_ptr(void, transition,   struct be_engine_memory *);
};

struct be_app_layers {
    struct m_static_array(func_ptr_type(void, struct be_engine_memory *)) init;
    struct m_static_array(func_ptr_type(void, struct be_engine_memory *)) delete;

    struct m_static_array(func_ptr_type(void, struct m_static_array(struct wm_event))) on_event;
    struct m_static_array(func_ptr_type(void, struct be_engine_memory *)) on_update;
    struct m_static_array(func_ptr_type(void, struct be_engine_memory *)) on_render;

    struct m_static_array(func_ptr_type(void, struct be_engine_memory *)) suspend;
    struct m_static_array(func_ptr_type(void, struct be_engine_memory *)) transition;
};

void be_engine_memory_init(struct be_engine_memory *be_engine_memory);

void be_engine_init(struct be_engine *be_engine, struct be_engine_info be_engine_info);
void be_engine_run(struct be_engine *be_engine);
void be_engine_delete(struct be_engine *be_engine);

#endif /* __BE_BE_ENGINE_H__ */
