#include <core/utils.h>
#include <core/os_dynamic_library.h>
#include <core/log.h>

void (*start) (void);
void (*run) (void);
void (*shutdown) (void);

struct os_librar test_lib;
struct os_library_info test_lib_info;

void init_functions(void)
{
    start       = NULL;
    run         = NULL;
    shutdown    = NULL;
    LOG_INFO("init test...");

    test_lib_info.path = "./";
    test_lib_info.name = "test";
    os_open_library(&test_lib, &test_lib_info);
    os_load_library_symbol(&test_lib, &start,      "test_start_func");
    os_load_library_symbol(&test_lib, &run,        "test_run_func");
    os_load_library_symbol(&test_lib, &shutdown,   "test_shutdown_func");

    ASSERT(start,       "start function NULL!!");
    ASSERT(run,         "run function NULL!!");
    ASSERT(shutdown,    "shutdown function NULL!!");
}

void run_functions(void)
{
    LOG_INFO("running test...");
    start();
    run();
    shutdown();
    LOG_INFO("test complete!");
}

void cleanup(void)
{
    LOG_INFO("finishing test...");
    os_close_library(&test_lib);
}

int main(void)
{
    init_functions();
    run_functions();
    cleanup();

    return 0;
}
