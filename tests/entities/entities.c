#include <core/types.h>
#include <core/os.h>
#include <core/memory.h>
#include <core/memory_arena.h>

int main(int argc, char **argv)
{
    struct m_arena arena = {0};
    struct m_arena_info arena_info = {0};
    arena_info.mem_size = KB_SIZE * 3;
    m_arena_init(&arena, arena_info);

    return OS_EXIT_SUCCESS;
}
