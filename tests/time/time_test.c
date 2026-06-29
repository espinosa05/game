#include <core/os.h>
#include <core/os_thread.h>
#include <core/log.h>
// #include <core/os_random.h>

void os_random_seed(u64 seed)
{
    srand(seed);
}

u64 os_random(void)
{
    return rand();
}

int main(int argc, char **argv)
{
    struct os_time curr = {0};
    os_time_get_current(&curr);
    os_random_seed(os_time_get_usec(curr));

    usz lower = 20;
    usz upper = 190;

    usz sleep_val = (os_random() + lower) % upper;

    INFO_LOG(USZ_FMT"ms", sleep_val);
    os_thread_sleep_msec(sleep_val);

    return OS_EXIT_SUCCESS;
}

