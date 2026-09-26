#include <be/be_gl_render.h>
#include <core/wm_gl.h>

/* static function declaration start */
static BeGlRenderContext *new_rc(BeEngine *be);
/* static function declaration end */

BeGlRenderContext *be_gl_render_on_attach(BeEngine *be)
{
    BeGlRenderContext *rc = new_rc(be);

    b32 loaded = wm_gl_load();
    ASSERT(loaded, "failed to load OpenGL");

    return rc;
}

void be_gl_render_on_update(BeEngine *be, BeGlRenderContext *rc)
{
    UNUSED(be);
    UNUSED(rc);
}

void be_gl_render_on_suspend(BeEngine *be, BeGlRenderContext *rc)
{
    UNUSED(be);
    UNUSED(rc);
}

void be_gl_render_on_activate(BeEngine *be, BeGlRenderContext *rc)
{
    UNUSED(be);
    UNUSED(rc);
}

void be_gl_render_on_event(BeEngine *be, BeGlRenderContext *rc)
{
    UNUSED(be);
    UNUSED(rc);
}

void be_gl_render_on_detach(BeEngine *be, BeGlRenderContext *rc)
{
    UNUSED(be);
    UNUSED(rc);
}

static BeGlRenderContext *new_rc(BeEngine *be)
{
    return be_alloc_perm(be, sizeof(BeGlRenderContext), 1);
}
