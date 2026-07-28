#ifndef __BE_BE_ENTRY_H__
#define __BE_BE_ENTRY_H__

#include <core/types.h>
#include <core/memory_arena.h>
#include <core/cli.h>
#include <core/wm.h>

struct be_engine;

#define BE_LAYER_SPEC(name)                 \
    (struct be_layer_spec) {                \
        .on_attach  = name##_on_attach      \
        .on_update  = name##_on_update,     \
        .on_event   = name##_on_event,      \
        .on_suspend = name##_on_suspend,    \
        .on_detach  = name##_on_detach,     \
    }

struct be_layer_spec {
    void (*on_attach) (struct be_engine *);
    void (*on_update) (struct be_engine *);
    void (*on_event) (struct be_engine *, struct wm_event);
    void (*on_suspend) (struct be_engine *);
    void (*on_detach) (struct be_engine *);
};

/* the init function is omitted here */
struct be_layer {
    void (*on_update) (struct be_engine *);
    void (*on_event) (struct be_engine *, struct wm_event);
    void (*on_suspend) (struct be_engine *);
    void (*on_detach) (struct be_engine *);
};

#define BE_WINDOW_TARGET_FRAME_MS_UNCAP 0

/* to be implemented by the user */
extern void be_app_entry(struct be_engine *be_engine, struct cli_args args);

#endif /* __BE_BE_ENTRY_H__ */
