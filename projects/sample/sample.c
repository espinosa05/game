#include <be/be_app_entry.h>

#include <be/be_engine.h>
#include <core/utils.h>
#include <core/wm.h>

struct game_context {

};

static void handle_window_event(struct game_context *game_context, struct wm_window_event event);
static void handle_keyboard_event(struct game_context *game_context, struct wm_keyboard_event event);
static void handle_mouse_event(struct game_context *game_context, struct wm_mouse_event event);

static void delete(void *context, struct be_engine_memory *memory)
{
    UNUSED(context);
    UNUSED(memory);
    return;
}

static void on_render(void *context, struct be_engine_memory *memory)
{
    UNUSED(context);
    UNUSED(memory);
    return;
}

static void on_update(void *context, struct be_engine_memory *memory)
{
    UNUSED(context);
    UNUSED(memory);
    return;
}

static void on_event(void *context, struct be_engine_memory *memory, struct wm_event *event)
{
    UNUSED(memory);
    struct game_context *game_context = context;

    switch (event->event_type) {
    case WM_EVENT_TYPE_WINDOW:
        handle_window_event(game_context, event->window_event);
        break;
    case WM_EVENT_TYPE_KEYBOARD:
        handle_keyboard_event(game_context, event->key_event);
        break;
    case WM_EVENT_TYPE_MOUSE:
        handle_mouse_event(game_context, event->mouse_event);
        break;
    }
}

static void suspend(void *context)
{
    UNUSED(context);
}

static void handle_window_event(struct game_context *game_context, struct wm_window_event event)
{
    UNUSED(game_context);
    UNUSED(event);
}

static void handle_keyboard_event(struct game_context *game_context, struct wm_keyboard_event event)
{
    UNUSED(game_context);
    UNUSED(event);
}

static void handle_mouse_event(struct game_context *game_context, struct wm_mouse_event event)
{
    UNUSED(game_context);
    UNUSED(event);
}

static struct be_app_layer get_game_layer(void)
{
    return (struct be_app_layer) {
        .delete     = delete,
        .on_render  = on_render,
        .on_update  = on_update,
        .on_event   = on_event,
        .suspend    = suspend,
    };
}

#define MAX_LAYERS 4
void be_app_entry(struct be_engine *be_engine, struct be_app_settings *be_app_settings)
{
    be_app_settings->app_window_title   = "test app";
    be_app_settings->app_window_width   = 1200;
    be_app_settings->app_window_height  = 720;

    be_init_layers(be_engine, MAX_LAYERS);
    be_push_layer(be_engine, get_game_layer());
}
