#include <core/types.h>
#include <core/os.h>
#include <core/os_path.h>
#include <core/log.h>
#include <core/memory.h>
#include <core/memory_arena.h>
#include <core/bm_fonts.h>

int main(int argc, char **argv)
{
    struct m_arena arena = {0};
    struct m_arena_info arena_info = {0};
    arena_info.mem_size = 32*KB_SIZE;
    m_arena_init(&arena, arena_info);

    struct bm_font_spec font = {0};
    usz st = bm_font_read_font_file_ar("test.bmf", &font, &arena);
    if (st != BM_FONT_STATUS_SUCCESS) {
        ERROR_LOG("failed to open font file: "STR_FMT, bm_font_status_str(st));
        m_arena_destroy(&arena);
        return OS_EXIT_FAILURE;
    }

    m_arena_destroy(&arena);

    return OS_EXIT_SUCCESS;
}
