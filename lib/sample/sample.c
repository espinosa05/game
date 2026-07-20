#include <be/be_app_entry.h>
#include <be/be_engine.h>

#include <core/utils.h>
#include <core/wm.h>
#include <core/wm_utils.h>
#include <core/log.h>

struct game_context {
    struct be_engine *engine;
};

/* static function declaration start */
static void delete(struct be_engine *engine);
static void on_render(struct be_engine *engine);
static void on_update(struct be_engine *engine);
static void on_event(struct be_engine *engine, struct wm_event event);
static void suspend(struct be_engine *engine);
/* static function declaration end */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void init(struct be_engine *engine)
{
    return;
}

static void delete(struct be_engine *engine)
{
    return;
}

static void on_render(struct be_engine *engine)
{
    return;
}

static void on_update(struct be_engine *engine)
{
    return;
}

static void on_event(struct be_engine *engine, struct wm_event event)
{
    if (event.event_type == WM_EVENT_TYPE_KEYBOARD
            && event.key_event.type == WM_KEYBOARD_EVENT_TYPE_KEY_PRESS
            && (event.key_event.value == WM_KEYSYM_Q
                || event.key_event.value == WM_KEYSYM_ESC)) {
        INFO_LOG("closing...");
        engine->close = TRUE;
    }
    return;
}

static void suspend(struct be_engine *engine)
{
}

static struct be_app_layer_spec get_game_layer(void)
{
    return (struct be_app_layer_spec) {
        .init       = init,
        .delete     = delete,
        .on_render  = on_render,
        .on_update  = on_update,
        .on_event   = on_event,
        .suspend    = suspend,
    };
}

#pragma GCC diagnostic pop

enum layers {
    SAMPLE_TEST_LAYER_MENU = 0,
    SAMPLE_TEST_LAYER_OVERLAY,
    SAMPLE_TEST_LAYER_3D_RENDERER,

    SAMPLE_TEST_LAYER_COUNT,
};

#define SAMPLE_APP_RESOURCE_PATH "resources/"
#define SAMPLE_APP_SHADER_PATH SAMPLE_APP_RESOURCE_PATH "shaders/"
#define SAMPLE_APP_SIMULATION_THREAD_COUNT 4
void be_app_entry(struct be_engine *be_engine, struct be_app_settings *be_app_settings, struct cli_args args)
{
	UNUSED(args);
    be_app_settings->app_name                   = "test_app";
    be_app_settings->app_window_title           = "test app";
    be_app_settings->app_window_width           = 1200;
    be_app_settings->app_window_height          = 720;
    be_app_settings->app_sim_thread_count       = SAMPLE_APP_SIMULATION_THREAD_COUNT;

    be_init_layers(be_engine, SAMPLE_TEST_LAYER_COUNT);
    be_push_layer(be_engine, get_game_layer());
}

