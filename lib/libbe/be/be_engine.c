#include <be/be_engine.h>
#include <core/log.h>

static void load_client_settings(struct be_engine_settings *client_settings);
static void load_client_layers(struct be_app_layers *client_layers);

static b32 should_close(struct be_engine *be_engine);
static void tick_start(struct be_engine *be_engine);
static void poll_events(struct be_engine *be_engine);
static void handle_events(struct be_engine *be_engine);
static void update(struct be_engine *be_engine);
static void render(struct be_engine *be_engine);
static void tick_end(struct be_engine *be_engine);

#define TRANSIENT_BUFF_SIZE (32*MB_SIZE)
#define PERMANENT_BUFF_SIZE (16*MB_SIZE)

void be_engine_memory_init(struct be_engine_memory *be_engine_memory)
{
    static u8 transient_buff[TRANSIENT_BUFF_SIZE] = {0};
    static u8 permanent_buff[PERMANENT_BUFF_SIZE] = {0};

    struct m_arena_info transient_info = {0};
    transient_info.external = TRUE;
    transient_info.buffer   = transient_buff;
    transient_info.size     = ARRAY_SIZE(transient_buff);
    m_arena_init(&be_engine_memory->transient, transient_info);

    struct m_arena_info permanent_info = {0};
    permanent_info.external = TRUE;
    permanent_info.buffer   = permanent_buff;
    permanent_info.size     = ARRAY_SIZE(permanent_buff);
    m_arena_init(&be_engine_memory->permanent, permanent_info);
}

void be_engine_memory_init(struct be_engine_memory *be_engine_memory)
{
    m_arena_delete(&be_engine_memory->transient);
    m_arena_delete(&be_engine_memory->permanent);
}

void be_engine_init(struct be_engine *be_engine)
{
    be_engine->dt = 0;

    os_time_init(&be_engine->frame_start);
    os_time_init(&be_engine->frame_end);

    be_engine_memory_init(&be_engine->memory);

    /* load data from the client application code */
    struct be_app_settings app_settings = {0};
    be_app_settings_load(&app_settings);

    m_zero(be_engine->events.data, sizeof(be_engine->event.data));

    wm_init(&be_engine->wm);
    struct wm_window_info main_window_info = {0};
    main_window_info.initial_title  = app_settings->app_window_title,
    main_window_info.force_size     = TRUE,
    main_window_info.width          = app_settings->app_window_width,
    main_window_info.height         = app_settings->app_window_height,
    main_window_info.x_pos          = X_POS_CENTERED,
    main_window_info.y_pos          = Y_POS_CENTERED,
    wm_window_create(&be_engine->wm, &be_engine->main_window, main_window_info);

    /* load layers from the client application code */
    be_app_load_layers(&be_engine);
}

void be_engine_run(struct be_engine *be_engine)
{
    while (!should_close(be_engine)) {
        tick_start(be_engine);

        poll_events(be_engine);
        handle_events(be_engine);

        update_layers(be_engine);
        render_layers(be_engine);
        present_image(be_engine);
        tick_end(be_engine);
    }
}

void be_engine_delete(struct be_engine *be_engine)
{
    wm_shutdown(&be_engine->wm);
    be_engine_memory_cleanup(&be_engine->memory);
}


static b32 should_close(struct be_engine *be_engine)
{
    IMPL();
    UNUSED(be_engine);
    return FALSE;
}

static void tick_start(struct be_engine *be_engine)
{
    os_timer_get_monotonic(&be_engine->frame_start);

    struct os_time frame_time = {0};
    os_time_get_diff(&frame_time, be_engine->frame_start, be_engine->frame_end);

    be_engine->dt = os_time_get_msec(frame_time);
}

static void poll_events(struct be_engine *be_engine)
{
    struct wm_event event = {0};

    do {
        wm_window_poll_events(&be_engine->wm, &be_engine->main_window, &event);
        m_static_array_push(&be_engine->events, event);
    } while (!m_static_array_free_space(&be_engine->events)
                || event.event_type != WM_EVENT_EMPTY_QUEUE);

}

static void handle_events(struct be_engine *be_engine)
{
    decl_func_ptr(void, on_event, struct be_engine_memory *) = NULL;

    for (usz i = 0; i < be_engine->app_layers.count; ++i) {
        on_event = be_engine->app_layers.on_event.data[i];
        on_event(&be_engine->memory, &be_engine->events);
    }
}

static void update(struct be_engine *be_engine)
{
    decl_func_ptr(void, on_update, struct be_engine_memory *);

    for (usz i = 0; i < be_engine->app_layers.count; ++i) {
        on_update = be_engine->app_layers.on_update[i];
        on_update(&be_engine->memory);
    }
}

static void render(struct be_engine *be_engine)
{
    decl_func_ptr(void, on_render, struct be_engine_memory *);

    for (usz i = 0; i < be_engine->app_layers.count; ++i) {
        on_render = be_engine->app_layers.on_render[i];
        on_render(&be_engine->memory);
    }
}

static void tick_end(struct be_engine *be_engine)
{
    os_timer_get_monotonic(&be_engine->frame_end);
}

