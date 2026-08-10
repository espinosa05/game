#include <be/be_engine.h>
#include <be/be_window.h>
#include <core/memory_macros.h>
#include <core/wm_utils.h>

BeWindow *be_window_on_attach(BeEngine *be)
{
    BeWindow *window = be_alloc_perm(be, sizeof(*window), 1);
    void *buff = be_alloc_perm(be, sizeof(*window->events.data), 32);
    mm_stack_init_ext(&window->events, buff, 32);

    BeLayer *wi_layer = be_get_layer_by_name(be, "window_settings");
    ASSERT(wi_layer, "client must provide a "STR_QUOT("window_settings")"layer with struct wm_window_info!");

    WM_CALL(wm_init(&window->wm));

    struct wm_window_info window_info = *(struct wm_window_info *)wi_layer->context;
    WM_CALL(wm_window_create(&window->wm, &window->main_window, window_info));
    wm_window_show(&window->wm, &window->main_window);

    be_detach_layer_by_name(be, "window_settings");

    return window;
}

void be_window_on_update(BeEngine *be, BeWindow *win)
{
    UNUSED(be);
    while (TRUE) {
        struct wm_event event = {0};
        wm_window_poll_events(&win->wm, &win->main_window, &event);
        if (event.event_type == WM_EVENT_EMPTY_QUEUE)
            break;
        mm_stack_push(&win->events, event);
    }
}

void be_window_on_suspend(BeEngine *be, BeWindow *win)
{
    UNUSED(be);
    UNUSED(win);
}

void be_window_on_activate(BeEngine *be, BeWindow *win)
{
    UNUSED(be);
    UNUSED(win);
}

void be_window_on_event(BeEngine *be, BeWindow *win)
{
    UNUSED(be);
    UNUSED(win);
}

void be_window_on_detach(BeEngine *be, BeWindow *win)
{
    UNUSED(be);
    UNUSED(win);

    wm_window_close(&win->wm, &win->main_window);
    wm_shutdown(&win->wm);
}
