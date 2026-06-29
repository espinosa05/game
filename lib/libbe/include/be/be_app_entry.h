#ifndef __BE_BE_APP_ENTRY_H__
#define __BE_BE_APP_ENTRY_H__

#include <core/types.h>
#include <core/memory_arena.h>
#include <core/wm.h>

struct be_engine;

struct be_app_layer {
    void *context;
    void (*delete) (struct be_engine *);
    void (*on_render) (struct be_engine *);
    void (*on_update) (struct be_engine *);
    void (*on_event) (struct be_engine *, struct wm_event);
    void (*suspend) (struct be_engine *);
};

#define BE_APP_WINDOW_TARGET_FRAME_MS_UNCAP 0

struct be_app_settings {
    const char  *app_name;
    const char  *app_window_title;
    usz         app_window_width;
    usz         app_window_height;
    f64         app_window_target_frame_ms;
    usz         app_sim_thread_count;
    const char  *app_root_path;
};

struct be_engine;
/* to be implemented by the user */
extern void be_app_entry(struct be_engine *be_engine, struct be_app_settings *be_app_settings);

#endif /* __BE_BE_APP_ENTRY_H__ */
