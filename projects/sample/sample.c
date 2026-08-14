#include <be/be_app_entry.h>
#include <be/be_window.h>
#include <be/be_layer.h>
#include <be/be_engine.h>

#include <core/cstr.h>
#include <core/utils.h>
#include <core/wm.h>
#include <core/wm_utils.h>
#include <core/log.h>

static struct wm_window_info *window_settings_on_attach(BeEngine *be);
static void window_settings_on_detach(BeEngine *be, struct wm_window_info *info);
static void *be_sample_on_attach(BeEngine *be);
static void be_sample_on_update(BeEngine *be, void *ctx);
static void be_sample_on_suspend(BeEngine *be, void *ctx);
static void be_sample_on_activate(BeEngine *be, void *ctx);
static void be_sample_on_event(BeEngine *be, void *ctx);
static void be_sample_on_detach(BeEngine *be, void *ctx);


void be_app_entry(BeEngine *be, struct cli_args args)
{
    UNUSED(args);
    be_push_layer(be, BE_LAYER_CONTEXT_ONLY(window_settings));
    be_push_layer(be, BE_LAYER_SPEC(be_sample));
}

static struct wm_window_info *window_settings_on_attach(BeEngine *be)
{
    struct wm_window_info *info = be_alloc_perm(be, sizeof(*info), 1);
    info->title     = "title";
    info->width     = 1200;
    info->height    = 720;
    info->x_pos     = X_POS_CENTERED;
    info->y_pos     = Y_POS_CENTERED;
    info->force_size = TRUE;

    return info;
}

static void window_settings_on_detach(BeEngine *be, struct wm_window_info *info)
{
    UNUSED(be);
    UNUSED(info);
}

static void *be_sample_on_attach(BeEngine *be)
{
    UNUSED(be);
    return NULL;
}

static void be_sample_on_update(BeEngine *be, void *ctx)
{
    UNUSED(ctx);
    BeLayer *win_layer = be_get_layer_by_name(be, "be_window");
    BeWindow *win = win_layer->context;

    if (win->input.esc || win->input.q)
        be->run = FALSE;

}

static void be_sample_on_suspend(BeEngine *be, void *ctx)
{
    UNUSED(be);
    UNUSED(ctx);
}

static void be_sample_on_activate(BeEngine *be, void *ctx)
{
    UNUSED(be);
    UNUSED(ctx);
}

static void be_sample_on_event(BeEngine *be, void *ctx)
{
    UNUSED(be);
    UNUSED(ctx);
}

static void be_sample_on_detach(BeEngine *be, void *ctx)
{
    UNUSED(be);
    UNUSED(ctx);
}

