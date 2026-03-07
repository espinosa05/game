#ifndef __BE_BE_APP_ENTRY_H__
#define __BE_BE_APP_ENTRY_H__

#include <core/types.h>
#include <core/memory_arena.h>

struct be_engine_memory {
    struct m_arena permanent;
    struct m_arena transient;
};

struct be_app_window_specs {
    usz width;
    usz height;
    const char *title;
};

struct be_app_settings {
    const char *app_name;
    struct be_app_window_specs window_specs;
};

/* to be implemented by the user */
void be_app_settings_load_data(struct be_app_settings *settings);
void be_app_load_layers(struct be_engine *engine);

#endif /* __BE_BE_APP_ENTRY_H__ */
