#include <be/be_engine.h>
#include <be/be_app_entry.h>
#include <core/log.h>
#include <core/memory_macros.h>

/* static function declaration start */
static b32 should_close(struct be_engine *be_engine);
static void tick_start(struct be_engine *be_engine);
static void tick_end(struct be_engine *be_engine);
static void poll_events(struct be_engine *be_engine);
static void handle_events(struct be_engine *be_engine);
static void update(struct be_engine *be_engine);
static void render(struct be_engine *be_engine);
static void present_image(struct be_engine *be_engine);
static void transition_layers(struct be_engine *be_engine);
static void init_events(struct be_engine *engine, const usz count);
/* static function declaration end */


void be_init_layers(struct be_engine *be_engine, usz layer_count)
{
    struct be_app_layer *buff = m_arena_alloc(&be_engine->memory.permanent, sizeof(struct be_app_layer), layer_count);
    mm_array_init_ext(&be_engine->layers, buff, layer_count);
}

void be_push_layer(struct be_engine *be_engine, struct be_app_layer layer)
{
    mm_array_append(&be_engine->layers, layer);
}

#define TRANSIENT_BUFF_SIZE (32*MB_SIZE)
#define PERMANENT_BUFF_SIZE (16*MB_SIZE)

void be_engine_memory_init(struct be_engine_memory *be_engine_memory)
{
    TODO("add a arena list for resizable memory buffers");
    static u8 transient_buff[TRANSIENT_BUFF_SIZE] = {0};
    static u8 permanent_buff[PERMANENT_BUFF_SIZE] = {0};

    struct m_arena_info transient_info = {0};
    transient_info.external = TRUE;
    transient_info.buffer   = transient_buff;
    transient_info.mem_size = ARRAY_SIZE(transient_buff);
    m_arena_init(&be_engine_memory->transient, transient_info);

    struct m_arena_info permanent_info = {0};
    permanent_info.external = TRUE;
    permanent_info.buffer   = permanent_buff;
    permanent_info.mem_size = ARRAY_SIZE(permanent_buff);
    m_arena_init(&be_engine_memory->permanent, permanent_info);
}

void be_engine_memory_cleanup(struct be_engine_memory *be_engine_memory)
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

    init_events(be_engine, 5);

    /* load data from the client application code */
    struct be_app_settings app_settings = {0};
    be_app_entry(be_engine, &app_settings);

    wm_init(&be_engine->wm);
    struct wm_window_info main_window_info = {0};
    main_window_info.initial_title  = app_settings.app_window_title,
    main_window_info.force_size     = TRUE,
    main_window_info.width          = app_settings.app_window_width,
    main_window_info.height         = app_settings.app_window_height,
    main_window_info.x_pos          = X_POS_CENTERED,
    main_window_info.y_pos          = Y_POS_CENTERED,
    wm_window_create(&be_engine->wm, &be_engine->main_window, main_window_info);
    wm_window_show(&be_engine->wm, &be_engine->main_window);
}

void be_engine_run(struct be_engine *be_engine)
{
    while (!should_close(be_engine)) {
        tick_start(be_engine);

        poll_events(be_engine);
        handle_events(be_engine);

        update(be_engine);
        render(be_engine);

        present_image(be_engine);

        transition_layers(be_engine);
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

#define WM_EVENT_QUEUE_LENGTH 16

static void tick_start(struct be_engine *be_engine)
{
    /* get frame start time */
    {
        struct os_time frame_time = {0};
        os_time_get_monotonic(&be_engine->frame_start);
        os_time_get_diff(&frame_time, be_engine->frame_start, be_engine->frame_end);
        be_engine->dt = os_time_get_msec(frame_time);
    }

    /* setup transient data */
    {
        m_zero(&be_engine->events, sizeof(be_engine->events));
        void *event_buff = m_arena_alloc(&be_engine->memory.transient, sizeof(*be_engine->events.data), WM_EVENT_QUEUE_LENGTH);
        mm_array_init_ext(&be_engine->events, event_buff, WM_EVENT_QUEUE_LENGTH);
    }
}

static void tick_end(struct be_engine *be_engine)
{
    os_time_get_monotonic(&be_engine->frame_end);

    /* reset transient memory */
    m_arena_clear(&be_engine->memory.transient);
}

static void poll_events(struct be_engine *be_engine)
{
    struct wm_event event = {0};

    do {
        wm_window_poll_events(&be_engine->wm, &be_engine->main_window, &event);
        if (event.event_type == WM_EVENT_EMPTY_QUEUE)
            break;
        mm_array_append(&be_engine->events, event);
    } while (mm_array_free_space(&be_engine->events));
}

static void handle_events(struct be_engine *be_engine)
{
    struct wm_event event = {0};
    void (*on_event) (void *, struct be_engine_memory *, struct wm_event) = NULL;
    void *context = NULL;
    struct be_app_layers layers = be_engine->layers;

    for (usz i = 0; i < layers.count; ++i) {
        on_event = layers.data[i].on_event;
        context = layers.data[i].context;
        mm_array_retreive(&be_engine->events, &event);
        on_event(context, &be_engine->memory, event);
    }
}

static void update(struct be_engine *be_engine)
{
    void (*on_update) (void *, struct be_engine_memory *) = NULL;
    void *context = NULL;
    struct be_app_layers layers = be_engine->layers;

    for (usz i = 0; i < layers.count; ++i) {
        on_update = layers.data[i].on_update;
        context = layers.data[i].context;
        on_update(context, &be_engine->memory);
    }
}

static void render(struct be_engine *be_engine)
{
    void (*on_render) (void *, struct be_engine_memory *) = NULL;
    void *context = NULL;
    struct be_app_layers layers = be_engine->layers;

    for (usz i = 0; i < layers.count; ++i) {
        on_render = layers.data[i].on_render;
        context = layers.data[i].context;
        on_render(context, &be_engine->memory);
    }
}

static void present_image(struct be_engine *be_engine)
{
    UNUSED(be_engine);
}

static void transition_layers(struct be_engine *be_engine)
{
    UNUSED(be_engine);
    IMPL();
}

static void init_events(struct be_engine *be_engine, const usz count)
{
    mm_array_init_ar(&be_engine->events, count, &be_engine->memory.permanent);
}


