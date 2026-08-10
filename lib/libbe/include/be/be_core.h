#ifndef __BE_BE_CORE_H__
#define __BE_BE_CORE_H__

#include <be/be_engine.h>
#include <be/be_layer.h>

#include <ntt/ntt.h>

typedef struct {
    NttIDs      entity_storage;
    NttSystems  entity_systems;

    NttComponents       cmp_storage;
    NttComponentActions cmp_actions;
} BeCore;

#define BE_CORE_LAYER_SPEC BE_LAYER_SPEC(be_core)

BeCore *be_core_on_attach(BeEngine *be);
void be_core_on_update(BeEngine *be, BeCore *core);
void be_core_on_event(BeEngine *be, BeCore *core);
void be_core_on_suspend(BeEngine *be, BeCore *core);
void be_core_on_activate(BeEngine *be, BeCore *core);
void be_core_on_detach(BeEngine *be, BeCore *core);

#endif /* __BE_BE_CORE_H__ */
