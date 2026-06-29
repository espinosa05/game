#include <core/memory_arena.h>
#include <core/memory.h>
#include <core/audio.h>
#include <core/os.h>

int main(int argc, char **argv)
{
    struct m_arena arena = {0};
    struct m_arena_info arena_info = {0};
    arena_info.mem_size = KB_SIZE * 2;
    m_arena_init(&arena, arena_info);

    struct audio_context audio = {0};
    struct audio_context_info audio_info = {0};
    audio_info.arena = &arena;
    AUDIO_CALL(audio_init(&audio, audio_info));

    audio_shutdown(&audio);
    m_arena_destroy(&arena);

    return OS_EXIT_SUCCESS;
}
