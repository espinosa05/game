#ifndef __BE_BE_LAYER_H__
#define __BE_BE_LAYER_H__

#include <core/types.h>
#include <core/wm.h>

typedef struct s_be_engine BeEngine;

/* helper macros for initialization */
#define BE_CONTEXT(context)         ((void *)context)
#define BE_ON_ATTACH(on_attach)     ((void *(*) (BeEngine *))on_attach)
#define BE_ON_UPDATE(on_update)     ((void (*)  (BeEngine *, void *))on_update)
#define BE_ON_SUSPEND(on_suspend)   ((void (*)  (BeEngine *, void *))on_suspend)
#define BE_ON_ACTIVATE(on_activate) ((void (*)  (BeEngine *, void *))on_activate)
#define BE_ON_EVENT(on_event)       ((void (*)  (BeEngine *, void *))on_event)
#define BE_ON_DETACH(on_detach)     ((void (*)  (BeEngine *, void *))on_detach)

#define BE_LAYER_SPEC(name)                                     \
    (BeLayerSpec) {                                             \
        .active         = TRUE,                                 \
        .id_str         = #name,                                \
        .on_attach      = BE_ON_ATTACH(name##_on_attach),       \
        .on_update      = BE_ON_UPDATE(name##_on_update),       \
        .on_suspend     = BE_ON_SUSPEND(name##_on_suspend),     \
        .on_activate    = BE_ON_ACTIVATE(name##_on_activate),   \
        .on_event       = BE_ON_EVENT(name##_on_event),         \
        .on_detach      = BE_ON_DETACH(name##_on_detach),       \
    }

#define BE_LAYER_CONTEXT_ONLY(name)                     \
    (BeLayerSpec) {                                     \
        .id_str     = #name,                            \
        .on_attach  = BE_ON_ATTACH(name##_on_attach),   \
        .on_detach  = BE_ON_DETACH(name##_on_detach),   \
    }                                                   \

typedef struct {
    b32 active;
    const char *id_str;
    void *(*on_attach)  (BeEngine *);
    void (*on_update)   (BeEngine *, void *);
    void (*on_suspend)  (BeEngine *, void *);
    void (*on_activate) (BeEngine *, void *);
    void (*on_event)    (BeEngine *, void *);
    void (*on_detach)   (BeEngine *, void *);
} BeLayerSpec;

/* the on_attach function is omitted here */
typedef struct {
    b32 active;
    u64 id;
    void *context;
    void (*on_update)   (BeEngine *, void *);
    void (*on_suspend)  (BeEngine *, void *);
    void (*on_activate) (BeEngine *, void *);
    void (*on_event)    (BeEngine *, void *);
    void (*on_detach)   (BeEngine *, void *);
} BeLayer;

#define EACH_BE_LAYER(layer, layers) BeLayer *EACH_MM_ARRAY(layer, &layers)

#define BE_LAYER_SPEC_FMT        "("STR_FMT") { on_attach: "PTR_FMT", on_update: "PTR_FMT", on_suspend: "PTR_FMT", on_activate: "PTR_FMT", on_event: "PTR_FMT", on_detach: "PTR_FMT" }"
#define BE_LAYER_SPEC_FMT_ARG(l) (l).id_str, (l).on_attach, (l).on_update, (l).on_event, (l).on_detach

void be_push_layer(BeEngine *be, BeLayerSpec layer);
u64 be_layer_id(const char *string);
BeLayer *be_get_layer_by_name(BeEngine *be, char *id_str);
void be_detach_layer_by_name(BeEngine *be, char *id_str);

BeLayer *be_layer_by_id(BeEngine *be, u64 id);
void be_detach_layer_by_id(BeEngine *be, u64 id);

#endif /* __BE_BE_LAYER_H__ */
