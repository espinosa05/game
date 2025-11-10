#include <core/log.h>

void test_start_func(void)
{
    F_LOG_T(OS_STDERR, "LIBTEST", ANSI_COLOR_GREEN, __func__);
}

void test_run_func(void)
{
    F_LOG_T(OS_STDERR, "LIBTEST", ANSI_COLOR_YELLOW, __func__);
}

void test_shutdown_func(void)
{
    F_LOG_T(OS_STDERR, "LIBTEST", ANSI_COLOR_CYAN, __func__);
}
