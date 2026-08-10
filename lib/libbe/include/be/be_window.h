#ifndef __BE_BE_WINDOW_H__
#define __BE_BE_WINDOW_H__

#include <be/be_layer.h>
#include <core/wm.h>
#include <core/memory_macros.h>

typedef struct {
    MM_STACK_MEMBERS(struct wm_event);
} BeWindowEvents;

typedef struct {
    struct wm wm;
    struct wm_window main_window;
    BeWindowEvents events;
} BeWindow;

#define BE_WINDOW_LAYER_SPEC BE_LAYER_SPEC(be_window)

BeWindow *be_window_on_attach(BeEngine *be);
void be_window_on_update(BeEngine *be, BeWindow *win);
void be_window_on_suspend(BeEngine *be, BeWindow *win);
void be_window_on_activate(BeEngine *be, BeWindow *win);
void be_window_on_event(BeEngine *be, BeWindow *win);
void be_window_on_detach(BeEngine *be, BeWindow *win);

#endif /* __BE_BE_WINDOW_H__ */
