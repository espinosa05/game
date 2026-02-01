#include "cdb.h"

u8 buff[KB_SIZE*2] = {0};

enum cmp_ids {
    CMP_ID_DIR,
    CMP_ID_ACCEL,
    CMP_ID_SPEED,
    CMP_ID_POS,
};

void register_components(struct cdb *cdb, struct cdb_cmp_desc *desc, usz count)
{
    for (usz i = 0; i < count; ++i) {
        cdb_cmp_register(cdb, desc[i]);
    }
}

int main(int argc, char **argv)
{
    struct m_arena arena = {0};
    struct m_arena_info arena_info = {
        .mem_size   = sizeof(buff),
        .buffer     = buff,
        .external   = TRUE,
    };
    m_arena_init(&arena, arena_info);

    struct cdb_context cdb = {0};
    struct cdb_context_info cdb_info = {
        .arena = &arena,
    };
    cdb_init(&cdb, cdb_info);

    struct cdb_cmp_desc cmp_descs[] = {
        { CMP_ID_DIR,   cmp_bhv_dir,    sizeof(vec3) },
        { CMP_ID_ACCEL, cmp_bhv_accel,  sizeof(usz) },
        { CMP_ID_SPEED, cmp_bhv_speed,  sizeof(usz) },
        { CMP_ID_POS,   cmp_bhv_pos,    sizeof(vec3) },
    };
    register_components(&cdb, cmp_descs, ARRAY_SIZE(cmp_descs));

    cdb_destroy(&cdb);

    return 0;
}
