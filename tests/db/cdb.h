#ifndef __CDB_H__
#define __CDB_H__

#include <core/types.h>
#include <core/sparse_set.h>

struct cdb_context_info {
    struct m_arena *arena;
};

struct cdb {
    struct m_arena *arena;
    struct sparse_index_set entities;
};

#define CDB_QUERY_DST(dst) ((dst)(void *))

void cdb_init(struct cdb *cdb, const struct cdb_context_info info);
void cdb_destroy(struct cdb *cdb);
void cdb_query(struct cdb *cdb, void *dst, usz query[]);

#endif /* __CDB_H__ */
