#include <core/memory.h>
#include <core/memory_arena.h>
#include <core/strings.h>
#include <core/log.h>

int main()
{
    struct m_arena arena = {0};
    struct m_arena_info arena_info = {0};
    arena_info.buffer   = (u8[KB_SIZE]) {0};
    arena_info.mem_size = KB_SIZE;
    arena_info.external = TRUE;
    m_arena_init(&arena, arena_info);

    struct str_builder sb = {0};
    str_builder_init(&sb, 20);
    str_builder_append(&sb, "this ");
    str_builder_append(&sb, "is ");
    str_builder_append(&sb, "a ");
    str_builder_append(&sb, "test ");

    char *out = NULL;
    str_builder_to_cstr_alloc(&sb, &out);
    LOG("m_alloc: "STR_FMT, out);

    str_builder_to_cstr_ar(&sb, &out, &arena);
    LOG("m_arena: "STR_FMT, out);


    return 0;
}
