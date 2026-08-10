#ifndef __BE_BE_ARENA_H__
#define __BE_BE_ARENA_H__

#include <core/types.h>

struct s_be_arena_chunk;
struct s_be_arena_chunk {
    struct s_be_arena_chunk *next;
    usz count;
    usz used;
    void *base;
};
typedef struct s_be_arena_chunk BeArenaChunk;

void be_arena_chunk_init_ext(BeArenaChunk *chunk, void *base, usz size);
BeArenaChunk *be_arena_chunk_extend(BeArenaChunk *chunk, usz size);
void *be_arena_chunk_get(BeArenaChunk *chunk);

typedef struct {
    usz prev_total_size;
    BeArenaChunk *first;
    BeArenaChunk *last;
} BeArena;

void be_arena_init(BeArena *arena, usz init_size);
void *be_arena_alloc(BeArena *arena, usz chunk, usz count);
void be_arena_clear(BeArena *arena);

#endif /* __BE_BE_ARENA_H__ */
