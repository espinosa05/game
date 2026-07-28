#include <be/be_app_entry.h>
#include <be/be_engine.h>
#include <be/be_physics.h>
#include <be/be_particles.h>
#include <be/be_diagnostics.h>

#include <core/utils.h>
#include <core/wm.h>
#include <core/wm_utils.h>
#include <core/log.h>

static void sample_init(struct be_engine *be);
static void sample_delete(struct be_engine *be);
static void sample_on_update(struct be_engine *be);
static void sample_on_render(struct be_engine *be);
static void sample_on_event(struct be_engine *be, struct wm_event event);
static void sample_suspend(struct be_engine *be);

enum layers {
    SAMPLE_LAYER = 0,
    SAMPLE_LAYER_COUNT,
};

#define SAMPLE_APP_RESOURCE_PATH "resources/"
#define SAMPLE_APP_SHADER_PATH SAMPLE_APP_RESOURCE_PATH "shaders/"
#define SAMPLE_APP_SIMULATION_THREAD_COUNT 4

void be_app_entry(struct be_engine *be_engine, struct be_app_settings *be_app_settings, struct cli_args args)
{
	UNUSED(args);
    be_push_layer(be_engine, BE_LAYER_SPEC(sample, NULL));
    be_push_layer(be_engine, BE_LAYER_SPEC(be_physics, NULL));
    be_push_layer(be_engine, BE_LAYER_SPEC(be_particles, NULL));
    be_push_layer(be_engine, BE_LAYER_SPEC(be_diagnostics, NULL));
}

static void sample_init(struct be_engine *be)
{
    UNUSED(be);
}

static void sample_delete(struct be_engine *be)
{
    UNUSED(be);
}

static void sample_on_event(struct be_engine *be, struct wm_event event)
{
    UNUSED(be);
    UNUSED(event);
}

static void sample_suspend(struct be_engine *be)
{
    UNUSED(be);
}


