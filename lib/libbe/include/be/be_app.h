#ifndef __BE_APP_BE_APP_H__
#define __BE_APP_BE_APP_H__

#include <core/types.h>
#include <core/memory.h>
#include <core/wm.h>

void be_app_layers_init(struct be_app_layers *layers);
void be_app_layers_push(struct be_app_layers *layers, struct be_app_layer_desc layer);

#endif /* __BE_APP_BE_APP_H__ */
