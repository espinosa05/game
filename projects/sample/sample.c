#include <be/be_app_entry.h>
#include <be/be_layer.h>
#include <be/be_engine.h>

#include <core/utils.h>
#include <core/wm.h>
#include <core/wm_utils.h>
#include <core/log.h>

static struct wm_window_info *window_settings_on_attach(BeEngine *be);
static void window_settings_on_detach(BeEngine *be, struct wm_window_info *info);

#define SAMPLE_APP_RESOURCE_PATH "resources/"
#define SAMPLE_APP_SHADER_PATH SAMPLE_APP_RESOURCE_PATH "shaders/"
#define SAMPLE_APP_SIMULATION_THREAD_COUNT 4

void be_app_entry(BeEngine *be, struct cli_args args)
{
    be_layer_push(be_engine, BE_LAYER_SPEC(window_settings));
}

static struct wm_window_info *window_settings_on_attach(BeEngine *be)
{
    struct wm_window_info *info = be_alloc_permanent(be, sizeof(*info), 1);
    info->title     = "title";
    info->width     = 1200;
    info->height    = 720;
    info->x_pos     = X_POS_CENTERED;
    info->y_pos     = Y_POS_CENTERED;

    return info;
}

static void window_settings_on_detach(BeEngine *be, struct wm_window_info *info) {}
