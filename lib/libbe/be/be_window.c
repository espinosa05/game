#include <be/be_engine.h>
#include <be/be_layer.h>
#include <be/be_window.h>
#include <be/be_render.h>
#include <core/memory_macros.h>
#include <core/wm_utils.h>

/* static function declaration start */
static void handle_window_event(BeWindow *win, struct wm_window_event win_event);
static void handle_keyboard_event(BeWindow *win, struct wm_keyboard_event key_event);
static void handle_mouse_event(BeWindow *win, struct wm_mouse_event mouse_event);
/* static funciton declaration end */

BeWindow *be_window_on_attach(BeEngine *be)
{
    BeWindow *window = be_alloc_perm(be, sizeof(*window), 1);

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
        switch (event.event_type) {
        case WM_EVENT_TYPE_WINDOW:
            handle_window_event(win,  event.window_event);
            break;
        case WM_EVENT_TYPE_KEYBOARD:
            handle_keyboard_event(win, event.key_event);
            break;
        case WM_EVENT_TYPE_MOUSE:
            handle_mouse_event(win, event.mouse_event);
            break;
        default: UNREACHABLE();
        }
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

static void handle_window_event(BeWindow *win, struct wm_window_event win_event)
{
    UNUSED(win);
    switch (win_event.type) {
    case WM_WINDOW_EVENT_TYPE_EXPOSE:
        break;
    }
}

static void handle_keyboard_event(BeWindow *win, struct wm_keyboard_event key_event)
{
    b32 pressed = key_event.type == WM_KEYBOARD_EVENT_TYPE_KEY_PRESS;
    switch (key_event.value) {
    case WM_KEYSYM_SPACE: win->input.space = pressed; break;
    case WM_KEYSYM_A: win->input.a = pressed; break;
    case WM_KEYSYM_B: win->input.b = pressed; break;
    case WM_KEYSYM_C: win->input.c = pressed; break;
    case WM_KEYSYM_D: win->input.d = pressed; break;
    case WM_KEYSYM_E: win->input.e = pressed; break;
    case WM_KEYSYM_F: win->input.f = pressed; break;
    case WM_KEYSYM_G: win->input.g = pressed; break;
    case WM_KEYSYM_H: win->input.h = pressed; break;
    case WM_KEYSYM_I: win->input.i = pressed; break;
    case WM_KEYSYM_J: win->input.j = pressed; break;
    case WM_KEYSYM_K: win->input.k = pressed; break;
    case WM_KEYSYM_L: win->input.l = pressed; break;
    case WM_KEYSYM_M: win->input.m = pressed; break;
    case WM_KEYSYM_N: win->input.n = pressed; break;
    case WM_KEYSYM_O: win->input.o = pressed; break;
    case WM_KEYSYM_P: win->input.p = pressed; break;
    case WM_KEYSYM_Q: win->input.q = pressed; break;
    case WM_KEYSYM_R: win->input.r = pressed; break;
    case WM_KEYSYM_S: win->input.s = pressed; break;
    case WM_KEYSYM_T: win->input.t = pressed; break;
    case WM_KEYSYM_U: win->input.u = pressed; break;
    case WM_KEYSYM_V: win->input.v = pressed; break;
    case WM_KEYSYM_W: win->input.w = pressed; break;
    case WM_KEYSYM_X: win->input.x = pressed; break;
    case WM_KEYSYM_Y: win->input.y = pressed; break;
    case WM_KEYSYM_Z: win->input.z = pressed; break;
    case WM_KEYSYM_BACKSPACE:   win->input.backspace = pressed; break;
    case WM_KEYSYM_TAB:         win->input.tab = pressed; break;
    case WM_KEYSYM_ENTER:       win->input.enter = pressed; break;
    case WM_KEYSYM_ESC:         win->input.esc = pressed; break;
    case WM_KEYSYM_SHIFT_L:     win->input.shift_l = pressed; break;
    case WM_KEYSYM_SHIFT_R:     win->input.shift_r = pressed; break;
    case WM_KEYSYM_CTRL_L:      win->input.ctrl_l = pressed; break;
    case WM_KEYSYM_CTRL_R:      win->input.ctrl_r = pressed; break;
    case WM_KEYSYM_CAPS_LOCK:   win->input.caps = pressed; break;
    case WM_KEYSYM_ALT_L:       win->input.alt_l = pressed; break;
    case WM_KEYSYM_ALT_R:       win->input.alt_r = pressed; break;
    case WM_KEYSYM_SUPER_L:     win->input.super_l = pressed; break;
    case WM_KEYSYM_SUPER_R:     win->input.super_r = pressed; break;
    }
}

static void handle_mouse_event(BeWindow *win, struct wm_mouse_event mouse_event)
{
    UNUSED(win);
    UNUSED(mouse_event);
}

