#include "cdb.h"

void cdb_init(struct cdb_context *cdb, const struct cdb_context_info info)
{

    SPARSE_SET_CALL(sparse_index_set_init());
}

void cdb_destroy(struct cdb_context *cdb)
{

}


