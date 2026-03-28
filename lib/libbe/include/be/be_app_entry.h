#ifndef __BE_BE_APP_ENTRY_H__
#define __BE_BE_APP_ENTRY_H__

#include <core/types.h>
#include <core/memory_arena.h>
#include <core/wm.h>

struct be_engine_memory {
    struct m_arena permanent;
    struct m_arena transient;
};

struct be_app_layer {
    void *context;
    void (*delete) (void *, struct be_engine_memory *);
    void (*on_render) (void *, struct be_engine_memory *);
    void (*on_update) (void *, struct be_engine_memory *);
    void (*on_event) (void *, struct be_engine_memory *, struct wm_event);
    void (*suspend) (void *);
};

struct be_app_settings {
    const char  *app_window_title;
    usz         app_window_width;
    usz         app_window_height;
};

struct be_engine;
/* to be implemented by the user */
extern void be_app_entry(struct be_engine *be_engine, struct be_app_settings *be_app_settings);

#endif /* __BE_BE_APP_ENTRY_H__ */
