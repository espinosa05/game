#define NOB_IMPLEMENTATION
#include "nob.h"

#define IS_ARG(a, v) (0 == strcmp((a), (v)))

#define OUT_DIR "out/"
#define LIB_DIR "lib/"
#define INC_DIR "include/"

#define GAME_DIR "game/"

#define LINKS   "-lcore", "-lxcb", "-lvulkan"

#define CC      "gcc"
#define PROGNAME "game"


/* IGNORE LIST */
static const char *ignore_list[] = {
    __FILE__, /* obviously, we disregard this very file */
};

/* base names for the required libraries */
static const char *lib_names[] = {
    "core",
};

static bool run_project_executable;

bool nob_set_current_dir_log(const char *path)
{
    nob_log(NOB_INFO, "setting CWD to -> %s", path);
    return nob_set_current_dir(path);
}

#define MIN_FILENAME_LEN 1
/* checks for file extensions */
bool is_type(char *file, char *type)
{
    int type_len = strlen(type);
    int file_len = strlen(file);
    char *ext = NULL;

    if (type_len + 1 + MIN_FILENAME_LEN > file_len)
        return false;

    ext = &file[file_len-type_len];

    if (ext[-1] != '.')
        return false;

    return 0 == strcmp(ext, type);
}

/* check if file path is in ignore list */
bool is_ignored(char *file)
{
    for (int i = 0; i < NOB_ARRAY_LEN(ignore_list); ++i) {
        if (0 == strcmp(file, ignore_list[i]))
            return true;
    }

    return false;
}

char *src_path_to_obj_path(char *c_file)
{
    char *o_file = NULL;

    asprintf(&o_file, OUT_DIR"%.*s.o", strlen(c_file)-2, c_file);

    return o_file;
}

void copy_shared_libraries(void)
{
    for (int i = 0; i < NOB_ARRAY_LEN(lib_names); ++i) {
        Nob_Cmd copy_cmd = {0};
        Nob_String_Builder src_path = {0};

        nob_sb_appendf(&src_path, "%s%s/out/lib%s.a",
                       LIB_DIR, lib_names[i], lib_names[i]);
        nob_sb_append_null(&src_path);

        nob_cmd_append(&copy_cmd, "cp", src_path.items, OUT_DIR);
        nob_cmd_run_sync(copy_cmd);

        nob_sb_free(src_path);
        nob_cmd_free(copy_cmd);
    }

}

void build_libraries(void)
{
    Nob_Procs build_threads = {0};
    Nob_Cmd build_cmd = {0};
    Nob_File_Paths lib_deps = {0};
    const char *base_dir = NULL;

    base_dir = nob_get_current_dir_temp();
    NOB_ASSERT(nob_read_entire_dir(LIB_DIR, &lib_deps));
    for (int i = 0; i < lib_deps.count; ++i) {
        Nob_String_Builder lib_dir_path = {0};
        /* we skip hidden files, "." and ".." */
        if (lib_deps.items[i][0] == '.' && nob_get_file_type(lib_deps.items[i]) == NOB_FILE_DIRECTORY) {
            nob_log(NOB_INFO, "skipping -> %s", lib_deps.items[i]);
            continue;
        }
        /* for every path in LIB_DIR, change path and "./nob" */
        nob_sb_appendf(&lib_dir_path, "%s%s", LIB_DIR, lib_deps.items[i]);
        nob_sb_append_null(&lib_dir_path);
        NOB_ASSERT(nob_set_current_dir_log(lib_dir_path.items));
        nob_cmd_append(&build_cmd, "./nob");
        nob_da_append(&build_threads, nob_cmd_run_async_and_reset(&build_cmd));
        NOB_ASSERT(nob_set_current_dir_log(base_dir));
    }
    NOB_ASSERT(nob_procs_wait_and_reset(&build_threads));
    NOB_ASSERT(nob_set_current_dir_log(base_dir));
    nob_temp_reset();

    copy_shared_libraries();
}

void clean_libs(void)
{
    Nob_Cmd clean_cmd = {0};
    Nob_File_Paths lib_deps = {0};
    const char *base_dir = NULL;

    base_dir = nob_get_current_dir_temp();
    NOB_ASSERT(nob_read_entire_dir(LIB_DIR, &lib_deps));

    for (int i = 0; i < lib_deps.count; ++i) {
        Nob_String_Builder lib_dir_path = {0};
        /* we skip hidden files, "." and ".." */
        if (lib_deps.items[i][0] == '.' && nob_get_file_type(lib_deps.items[i]) == NOB_FILE_DIRECTORY) {
            nob_log(NOB_INFO, "skipping -> %s", lib_deps.items[i]);
            continue;
        }
        /* for every path in LIB_DIR, change path and "./nob" */
        nob_sb_appendf(&lib_dir_path, "%s%s", LIB_DIR, lib_deps.items[i]);
        nob_sb_append_null(&lib_dir_path);

        /* change path */
        NOB_ASSERT(nob_set_current_dir_log(lib_dir_path.items));
        nob_cmd_append(&clean_cmd, "./nob", "clean");
        nob_cmd_run_sync_and_reset(&clean_cmd);
        NOB_ASSERT(nob_set_current_dir_log(base_dir));
    }
}

void clean_game()
{
    Nob_Cmd clean_cmd = {0};
    const char *base_dir = NULL;

    base_dir = nob_get_current_dir_temp();
    NOB_ASSERT(nob_set_current_dir_log(GAME_DIR));
    nob_cmd_append(&clean_cmd, "./nob", "clean");
    nob_cmd_run_sync_and_reset(&clean_cmd);

    NOB_ASSERT(nob_set_current_dir_log(base_dir));

    nob_temp_reset();
}

void clean_project(void)
{
    clean_libs();
    clean_game();
}

void parse_options(int argc, char **argv)
{
    if (argc < 2)
        return;

    Nob_Cmd cmd = {0};
    nob_shift(argv, argc);
    if (IS_ARG("clean", *argv)) {
        clean_project();
        exit(EXIT_SUCCESS);
    } else if (IS_ARG("run", *argv)) {
        run_project_executable = true;
    }  else {
        nob_log(NOB_ERROR, "%s: no such target", *argv);
        exit(EXIT_FAILURE);
    }
}

void build_application()
{
    const char *base_dir = nob_get_current_dir_temp();
    NOB_ASSERT(nob_set_current_dir(GAME_DIR));

    /* build game sources */
    {
        Nob_Cmd game_cmp_cmd = {0};
        nob_cmd_append(&game_cmp_cmd, "./nob");
        nob_cmd_run_sync(game_cmp_cmd);
    }

    NOB_ASSERT(nob_set_current_dir(base_dir));

    nob_temp_reset();
}

void link_executable()
{
    /* copy the game object files */
    {
        Nob_Cmd game_copy_cmd = {0};

        nob_cmd_append(&game_copy_cmd, "cp", GAME_DIR OUT_DIR "start.o", OUT_DIR);
        nob_cmd_run_sync_and_reset(&game_copy_cmd);

        nob_cmd_append(&game_copy_cmd, "cp", GAME_DIR OUT_DIR "main.o", OUT_DIR);
        nob_cmd_run_sync_and_reset(&game_copy_cmd);


        nob_cmd_append(&game_copy_cmd, "cp", GAME_DIR OUT_DIR "app.o", OUT_DIR);
        nob_cmd_run_sync_and_reset(&game_copy_cmd);
    }

    Nob_Cmd game_link_cmd = {0};
    nob_cmd_append(&game_link_cmd, CC);
    nob_cmd_append(&game_link_cmd, OUT_DIR "start.o");
    nob_cmd_append(&game_link_cmd, OUT_DIR "app.o");
    nob_cmd_append(&game_link_cmd, OUT_DIR "main.o");
    nob_cmd_append(&game_link_cmd, "-L./"OUT_DIR, LINKS);
    nob_cmd_append(&game_link_cmd, "-o", OUT_DIR PROGNAME ".elf");
    nob_cmd_run_sync(game_link_cmd);
}

void build_project(void)
{
    NOB_ASSERT(nob_mkdir_if_not_exists(OUT_DIR));
    build_libraries();
    build_application();
    link_executable();
}

void run_project(void)
{
    Nob_Cmd run_cmd = {0};

    nob_cmd_append(&run_cmd, OUT_DIR PROGNAME ".elf");
    nob_cmd_run_sync(run_cmd);
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    run_project_executable = false;
    parse_options(argc, argv);
    build_project();

    if (run_project_executable) {
        run_project();
    }
    return EXIT_SUCCESS;
}


