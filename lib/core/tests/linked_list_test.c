#include <core/ll.h>

void PrintList(OS_Stream *stream, LL_Single list *ll)
{
    OS_StreamPrintf(stream, "{ ");
    for (LL_Single *e = ll; e != NULL; e = e->next) {
        OS_StreamPrintf(stream, "%d, ", *(u32 *)e->element);
    }
    OS_StreamPrintf(stream, "}\n");
}

#define MEM_SIZE (1 << 10)
int main(int argc, char **argv)
{
    M_Arena arena = {0};
    M_ArenaInfo arenaInfo = {0};
    arenaInfo.memSize = MEM_SIZE;
    arenaInfo.external = TRUE;
    arenaInfo.buffer = (u8[MEM_SIZE]) {0};
    M_ArenaInit(&arena, &arenaInfo);

    LL_Single list = {0};
    LL_SingleInfo listInfo = {0};
    listInfo.typeSize = sizeof(u32);
    listInfo.count = MEM_SIZE/sizeof(u32);
    listInfo.arena = &arena;
    LL_SingleInit(&list, &listInfo);
    LL_SingleAppend(&list, &(u32) {500});
    LL_SingleAppend(&list, &(u32) {43});
    LL_SingleAppend(&list, &(u32) {595});

    PrintList(stderr, &list);

    u32 array[3] = {0};
    LL_SingleToArray(&list, &array);

    return 0;
}
