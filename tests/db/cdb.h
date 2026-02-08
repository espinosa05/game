#ifndef __CDB_H__
#define __CDB_H__

#include <core/types.h>
#include <core/sparse_set.h>

struct cs_context_info {
    struct m_arena *arena;
};

struct cs_cmp_region_info {
    struct cdb_cmp_type_desc *cmp_types;
    usz cmp_type_count;
};

struct cs_cmp_region {
    struct cdb_cmp_type_desc *descs;
    usz count;
};

struct cdb_context {
    struct m_arena *arena;
};

struct cdb_cmp_type_desc {
    usz id;
    void (*behaviour) (struct cdb_context *);
    usz cmp_width;
    usz cmp_ammount;
};

void cdb_init(struct cdb_context *cdb, const struct cdb_context_info info);
void cdb_destroy(struct cdb_context *cdb);
void cdb_get_cmp_desc(struct cdb_context *cdb, struct cmp_desc *dst, usz region, usz cmp_id)
void cdb_register_cmp_region(struct cdb_context *cdb, const struct cdb_cmp_region_info info);

#endif /* __CDB_H__ */
