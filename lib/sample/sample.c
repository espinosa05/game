#include <be/be_app_entry.h>
#include <be/be_engine.h>

#include <core/utils.h>
#include <core/wm.h>
#include <core/wm_utils.h>
#include <core/log.h>

enum layers {
    SAMPLE_LAYER = 0,
    SAMPLE_LAYER_COUNT,
};

BE_APP_LAYER_DELETE_FUNCTION(sample)
{
    UNUSED(be_engine);
}

BE_APP_LAYER_INIT_FUNCTION(sample)
{
    UNUSED(be_engine);
}

BE_APP_LAYER_ON_RENDER_FUNCTION(sample)
{
    UNUSED(be_engine);
}

BE_APP_LAYER_ON_UPDATE_FUNCTION(sample)
{
    UNUSED(be_engine);
}

BE_APP_LAYER_ON_EVENT_FUNCTION(sample)
{
    UNUSED(be_engine);
}

BE_APP_LAYER_SUSPEND_FUNCTION(sample)
{
    UNUSED(be_engine);
}

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

    be_init_layers(be_engine, SAMPLE_LAYER_COUNT);
    be_push_layer(be_engine, BE_APP_LAYER_SPEC(sample));
}

