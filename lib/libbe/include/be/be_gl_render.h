#ifndef __BE_BE_GL_RENDER_H__
#define __BE_BE_GL_RENDER_H__

#include <be/be_engine.h>

typedef struct {
    int a;
} BeGlRenderContext;

#define BE_GL_RENDER_LAYER_SPEC BE_LAYER_SPEC(be_gl_render)

BeGlRenderContext *be_gl_render_on_attach(BeEngine *be);
void be_gl_render_on_update(BeEngine *be, BeGlRenderContext *rc);
void be_gl_render_on_suspend(BeEngine *be, BeGlRenderContext *rc);
void be_gl_render_on_activate(BeEngine *be, BeGlRenderContext *rc);
void be_gl_render_on_event(BeEngine *be, BeGlRenderContext *rc);
void be_gl_render_on_detach(BeEngine *be, BeGlRenderContext *rc);


#endif /* __BE_BE_GL_RENDER_H__ */
