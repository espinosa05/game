#include "cdb.h"

#include <core/types.h>
#include <core/memory.h>
#include <core/cli.h>
#include <core/wm.h>
#include <core/os_directory.h>

u8 buff[GB_SIZE] = {0};

struct app_context_info {
    const char      *name;
    struct cli_args args;
};

struct app_context {
    struct wm_window *main_window;
};

struct app_conf {
    const char **mod_list;
};

struct mod_data {
    struct os_library *lib;
    void (**mod_init) (void *);
    void (**mod_tick) (void *);
    void (**mod_cleanup) (void *);
};

/* static function declaration start */
static void app_conf_defaults(struct app_conf *conf);

static void app_init(struct app_context *app, const struct app_context_info info);
static void app_run(struct app_context *app);
static void app_shutdown(struct app_context *app);
static b32 app_should_close(struct app_context *app);

static void mod_load_symbols(struct os_library *mod_lib, struct mod_data *mod_data, struct m_arena *arena);
/* static function declaration end */

int main(int argc, char **argv)
{
    struct app_context app = {0};
    struct app_context_info app_info = {
        .name = __FILE__,
        .args = CLI_ARGS(argc, argv),
    };
    app_init(&app, app_info);
    app_run(&app);
    app_shutdown(&app);

    return EXIT_SUCCESS;
}

static void app_conf_defaults(struct app_conf *conf)
{
    *conf = (struct app_conf) {0};
}

#define MAX_REGIONS 3

static void app_init(struct app_context *app, const struct app_context_info info)
{
}

static void app_run(struct app_context *app)
{
}

static void app_shutdown(struct app_context *app)
{
}

static b32 app_should_close(struct app_context *app)
{
    return app->main_window->should_close;
}

static void load_mods(struct m_array *mod_array, const struct app_conf conf, struct m_arena *arena)
{
    /* open mod directory */
    INFO_LOG("entering mod directory: "STR_FMT, conf.mod_path);
    struct os_dir mod_directory = {0};
    struct os_dir_info mod_directory_info = {
        .path = conf.mod_path,
        .perm = DPERM_READ,
    };
    os_dir_open(&mod_directory, mod_directory_info);

    /* array containing mod file names */
    struct m_array mod_lib_names = {0};
    os_dir_get_file_paths(&mod_directory, &mod_lib_names, OS_RELATIVE_PATH);
    usz mod_count = mod_lib_names.count;

    /* initialize output mod_array */
    struct m_array_info mod_array_info = {
        .base   = m_arena_alloc(arena, sizeof(struct mod_data), mod_count),
        .width  = sizeof(struct mod_data),
        .count  = mod_count,
    };
    m_array_init_ext(mod_array, mod_array_info);

    for (usz i = 0; i < mod_count; ++i) {
        const char *name = NULL;
        const char *stripped_name = NULL;

        name = m_arena_get_addr(mod_lib_names, i);
        cstr_token_ar(name, ".", &stripped_name, NULL, arena);

        INFO_LOG("opening library "STR_QUOT_LIT(STR_FMT)"...", name);
        /* get mod entry */
        struct mod_data *curr_mod_data = m_arena_get_addr(mod_array, i);
        struct os_library_info mod_lib_info = {
            .path = conf.mod_path,
            .name = stripped_name,
        };
        os_library_open(&curr_mod_data->lib, mod_lib_info);
        mod_load_symbols(stripped_name, curr_mod_data, arena);
    }

    os_dir_close(&mod_directory);
}

static void mod_load_symbols(char *mod_name, struct mod_data *mod_data, struct m_arena *arena)
{
    usz arena_offset = 0;
    m_arena_save(arena, &arena_offset);

    char *mod_init_sym = NULL;
    char *mod_tick_sym = NULL;
    char *mod_stop_sym = NULL;

    cstr_format_ar(&mod_init_sym, STR_FMT"_init", mod_name);
    cstr_format_ar(&mod_tick_sym, STR_FMT"_tick", mod_name);
    cstr_format_ar(&mod_cleanup_sym, STR_FMT"_cleanup", mod_name);

    os_library_load_symbol(&mod_data->lib, &mod_data->mod_init, mod_init_sym);
    os_library_load_symbol(&mod_data->lib, &mod_data->mod_tick, mod_tick_sym);
    os_library_load_symbol(&mod_data->lib, &mod_data->mod_cleanup, mod_cleanup_sym);

    m_arena_restore(arena, arena_offset);
}
