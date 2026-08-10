#include <be/be_app_entry.h>
#include <be/be_layer.h>
#include <be/be_engine.h>

#include <core/cstr.h>
#include <core/utils.h>
#include <core/wm.h>
#include <core/wm_utils.h>
#include <core/log.h>

static struct wm_window_info *window_settings_on_attach(BeEngine *be);
static void window_settings_on_detach(BeEngine *be, struct wm_window_info *info);

void be_app_entry(BeEngine *be, struct cli_args args)
{
    UNUSED(args);
    be_push_layer(be, BE_LAYER_CONTEXT_ONLY(window_settings));
}

static struct wm_window_info *window_settings_on_attach(BeEngine *be)
{
    struct wm_window_info *info = be_alloc_perm(be, sizeof(*info), 1);
    info->title     = "title";
    info->width     = 1200;
    info->height    = 720;
    info->x_pos     = X_POS_CENTERED;
    info->y_pos     = Y_POS_CENTERED;

    return info;
}

static void window_settings_on_detach(BeEngine *be, struct wm_window_info *info)
{
    UNUSED(be);
    UNUSED(info);
}

