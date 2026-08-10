
#include <be/be_core.h>

BeCore *be_core_on_attach(BeEngine *be)
{
    BeCore *core = be_alloc_perm(be, sizeof(*core), 1);
    return core;
}

void be_core_on_update(BeEngine *be, BeCore *core)
{
    UNUSED(be);
    UNUSED(core);
}

void be_core_on_event(BeEngine *be, BeCore *core)
{
    UNUSED(be);
    UNUSED(core);
}
void be_core_on_suspend(BeEngine *be, BeCore *core)
{
    UNUSED(be);
    UNUSED(core);
}
void be_core_on_activate(BeEngine *be, BeCore *core)
{
    UNUSED(be);
    UNUSED(core);
}
void be_core_on_detach(BeEngine *be, BeCore *core)
{
    UNUSED(be);
    UNUSED(core);
}
