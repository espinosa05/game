#ifndef __BE_APP_BE_APP_H__
#define __BE_APP_BE_APP_H__

#include <core/types.h>
#include <core/memory.h>
#include <core/wm.h>

/* forward declare */
struct be_app_layer_stack;

struct be_app_layer {
    void (*init) (struct be_engine_memory *);
    void (*delete) (struct be_engine_memory *);
    void (*on_event) (struct be_app_layer_stack *);
    void (*on_update) (struct be_engine_memory *);
    void (*suspend) (struct be_engine_memory *);
    void (*transition) (struct be_engine_memory *);
};

struct be_app_layer_stack {
    MM_STACK_MEMBERS(struct be_app_layer);
};

struct be_app_layer_transition {
    struct be_app_layer layer;
    usz layer_index;
};

struct be_app_layer_transition_queue {
    MM_QUEUE_MEMBERS(struct be_app_layer_transition);
};

void be_app_settings_load(struct be_app_settings *settings);
void be_app_layers_init(struct be_app_layers *layers);
void be_app_layers_push(struct be_app_layers *layers, struct be_app_layer_desc layer);

#endif /* __BE_APP_BE_APP_H__ */
