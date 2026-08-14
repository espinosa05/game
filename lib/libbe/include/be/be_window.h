#ifndef __BE_BE_WINDOW_H__
#define __BE_BE_WINDOW_H__

#include <be/be_layer.h>
#include <core/wm.h>
#include <core/memory_macros.h>

typedef struct {
    MM_STACK_MEMBERS(struct wm_event);
} BeWindowEvents;

typedef struct {
    b8 a;
    b8 b;
    b8 c;
    b8 d;
    b8 e;
    b8 f;
    b8 g;
    b8 h;
    b8 i;
    b8 j;
    b8 k;
    b8 l;
    b8 m;
    b8 n;
    b8 o;
    b8 p;
    b8 q;
    b8 r;
    b8 s;
    b8 t;
    b8 u;
    b8 v;
    b8 w;
    b8 x;
    b8 y;
    b8 z;
    b8 space;
    b8 backspace;
    b8 tab;
    b8 enter;
    b8 esc;
    b8 shift_l;
    b8 shift_r;
    b8 ctrl_l;
    b8 ctrl_r;
    b8 caps;
    b8 alt_l;
    b8 alt_r;
    b8 super_l;
    b8 super_r;
} BeInput;

typedef struct {
    struct wm wm;
    struct wm_window main_window;
    BeWindowEvents events;
    BeInput input;
} BeWindow;

typedef usz BeKey;

#define BE_WINDOW_LAYER_SPEC BE_LAYER_SPEC(be_window)

BeWindow *be_window_on_attach(BeEngine *be);
void be_window_on_update(BeEngine *be, BeWindow *win);
void be_window_on_suspend(BeEngine *be, BeWindow *win);
void be_window_on_activate(BeEngine *be, BeWindow *win);
void be_window_on_event(BeEngine *be, BeWindow *win);
void be_window_on_detach(BeEngine *be, BeWindow *win);

#endif /* __BE_BE_WINDOW_H__ */
