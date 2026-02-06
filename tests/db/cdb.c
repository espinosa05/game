#include "cdb.h"

void cdb_init(struct cdb_context *cdb, const struct cdb_context_info info)
{
    cdb->arena = info.arena;
    cdb->regions = m_arena_alloc(cdb->arena, sizeof(struct cdb_cmp_region), info.region_count);
}

void cdb_get_cmp_desc(struct cdb_context *cdb, struct cmp_desc *dst, usz region, usz cmp_id)
{

}

void cdb_register_cmp_region(struct cdb_context *cdb, struct cdb_cmp_region_info info)
{
}

