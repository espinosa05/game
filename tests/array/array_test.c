#include <core/memory_macros.h>

struct arr {
    MM_ARRAY_MEMBERS(int);
};

int main(int argc, char **argv)
{
    struct arr array = {0};
    mm_array_init(&array, 32);
    return 0;
}
