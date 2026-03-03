#include "be_app.h"
#include <core/log.h>

static b32 should_close(struct be_app *be_app);
static void tick_start(struct be_app *be_app);
static void poll_events(struct be_app *be_app);
static void handle_events(struct be_app *be_app);
static void update(struct be_app *be_app);
static void render(struct be_app *be_app);
static void tick_end(struct be_app *be_app);

#define TRANSIENT_BUFF_SIZE (32*MB_SIZE)
#define PERMANENT_BUFF_SIZE (16*MB_SIZE)

void be_app_memory_init(struct be_app_memory *be_app_memory)
{
    static u8 transient_buff[TRANSIENT_BUFF_SIZE] = {0};
    static u8 permanent_buff[PERMANENT_BUFF_SIZE] = {0};

    struct m_arena_info transient_info = {0}
    transient_info.external = TRUE;
    transient_info.buffer   = transient_buff;
    transient_info.size     = ARRAY_SIZE(transient_buff);
    m_arena_init(&be_app_memory->transient, transient_info);

    struct m_arena_info permanent_info = {0};
    permanent_info.external = TRUE;
    permanent_info.buffer   = permanent_buff;
    permanent_info.size     = ARRAY_SIZE(permanent_buff);
    m_arena_init(&be_app_memory->permanent, permanent_info);
}

void be_app_init(struct be_app *be_app)
{
    be_app->dt = 0;

    os_time_init(&be_app->frame_start);
    os_time_init(&be_app->frame_end);

    be_app_memory_init(&be_app->memory);

    const usz queue_length = 32;
    struct m_array_info event_arr_info = {0};
    event_arr_info.size     = queue_length;
    event_arr_info.width    = sizeof(struct wm_event);
    event_arr_info.base     = m_arena_alloc(&be_app->memory.permanent, sizeof(struct wm_event), queue_length);
    event_arr_info.external = TRUE;
    m_array_init_ext(&be_app->events, event_arr_info);

    wm_init(&be_app->wm);
    struct wm_window_info main_window_info = {0};
    main_window_info.initial_title  = "be_app",
    main_window_info.force_size     = TRUE,
    main_window_info.width          = 1200,
    main_window_info.height         = 720,
    main_window_info.x_pos          = X_POS_CENTERED,
    main_window_info.y_pos          = Y_POS_CENTERED,
    wm_window_create(&be_app->wm, &be_app->main_window, main_window_info);

    /* THIS FUNCTION IS WRITTEN BY THE APPLICATION PROGRAMMER!!! */
    be_app_load_layers(&be_app);
}

void be_app_run(struct be_app *be_app)
{
    while (!should_close(be_app)) {
        tick_start(be_app);
        poll_events(be_app);
        handle_events(be_app);
        update(be_app);
        render(be_app);
        tick_end(be_app);
    }
}

void be_app_delete(struct be_app *be_app)
{
    wm_shutdown(&be_app->wm);
}

void be_app_layers_init(struct be_app_layers *layers)
{

}

void be_app_layers_push(struct be_app_layers *layers, struct be_app_layer_desc layer)
{

}

static b32 should_close(struct be_app *be_app)
{
    IMPL();
    UNUSED(be_app);
    return FALSE;
}

static void tick_start(struct be_app *be_app)
{
    os_timer_get_monotonic(&be_app->frame_start);

    struct os_time frame_time = {0};
    os_time_get_diff(&frame_time, be_app->frame_start, be_app->frame_end);

    be_app->dt = os_time_get_msec(frame_time);
}

static void poll_events(struct be_app *be_app)
{
    wm_window_poll_events(&be_app->wm, &be_app->main_window, &be_app->event);
}

static void handle_events(struct be_app *be_app)
{
    DECL_FUNC_PTR(void, on_event, struct be_app_memory *);
    struct be_app_layer_desc *desc = be_app->layers.data;

    for (usz i = 0; i < be_app->layers.count; ++i) {
        on_event = desc->on_event[i];
        on_event(&be_app->memory, &be_app->events);
    }
}

static void update(struct be_app *be_app)
{
    DECL_FUNC_PTR(void, on_update, struct be_app_memory *);
    struct be_app_layer_desc *desc = NULL;

    for (usz i = 0; i < be_app->layers.count; ++i) {
        on_update = desc->on_update[i];
        on_update(&be_app->memory);
    }
}

static void render(struct be_app *be_app)
{
    UNUSED(be_app);
}

static void tick_end(struct be_app *be_app)
{
    os_timer_get_monotonic(&be_app->frame_end);
}

