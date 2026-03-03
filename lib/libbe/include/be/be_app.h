#ifndef __BE_APP_BE_APP_H__
#define __BE_APP_BE_APP_H__

#include <core/types.h>
#include <core/memory.h>
#include <core/wm.h>

struct be_app_memory {
    struct m_arena permanent;
    struct m_arena transient;
};

struct be_app {
    struct be_app_memory        memory;
    struct be_app_layers        layers;
    struct wm                   wm;
    struct wm_window            main_window;
    DECL_ARR(struct wm_event,   events);
    struct os_time      frame_start;
    struct os_time      frame_end;
    usz dt;
};

struct be_app_layer_desc {
    DECL_FUNC_PTR(void, init,         struct be_app_memory *);
    DECL_FUNC_PTR(void, delete,       struct be_app_memory *);

    DECL_FUNC_PTR(void, on_event,     struct be_app_memory *, DECL_ARR_TYPE(struct wm_event));
    DECL_FUNC_PTR(void, on_update,    struct be_app_memory *);

    DECL_FUNC_PTR(void, suspend,      struct be_app_memory *);
    DECL_FUNC_PTR(void, transition,   struct be_app_memory *);
};

struct be_app_layers {
    DECL_FUNC_PTR_ARR(void, init,         struct be_app_memory *);
    DECL_FUNC_PTR_ARR(void, delete,       struct be_app_memory *);

    DECL_FUNC_PTR_ARR(void, on_event,     struct be_app_memory *, DECL_ARR_TYPE(struct wm_event));
    DECL_FUNC_PTR_ARR(void, on_update,    struct be_app_memory *);

    DECL_FUNC_PTR_ARR(void, suspend,      struct be_app_memory *);
    DECL_FUNC_PTR_ARR(void, transition,   struct be_app_memory *);
};

void be_app_memory_init(struct be_app_memory *be_app_memory);

void be_app_init(struct be_app *be_app, struct be_app_info be_app_info);
void be_app_run(struct be_app *be_app);
void be_app_delete(struct be_app *be_app);

void be_app_layers_init(struct be_app_layers *layers);
void be_app_layers_push(struct be_app_layers *layers, struct be_app_layer_desc layer);

#endif /* __BE_APP_BE_APP_H__ */
