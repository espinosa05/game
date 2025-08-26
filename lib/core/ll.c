#include <core/ll.h>
#include <core/memory.h>

/* static function declaration start */
static void _SinglyLinkedList_SetElement(_SinglyLinkedList *element, void *data, usz size);
/* static function declaration end */

void LL_SingleInit(LL_Single *ll, const LL_SingleInfo *info)
{
    ll->elementSize = info->typeSize;
    ll->cap = info->count;
}

void LL_SingleAppend(LL_Single *ll, void *element)
{
    _SinglyLinkedList prev = ll->last;
    prev->next = M_ArenaAlloc(ll->arena, BYTE_SIZE, sizeof(*prev->next));
    prev->next->element = M_ArenaAlloc(ll->arena, BYTE_SIZE, ll->typeSize);

    _SinglyLinkedList_SetElement(prev->next, element, ll->typeSize);

    ll->last = prev->next;
}

void LL_SingleInsert(LL_Single *ll, usz at, void *element, usz size)
{
    _SinglyLinkedList e = ll->start;
    for (usz i = 0; i < at; ++i) {
        e = e->next;
    }
    _SinglyLinkedList_SetElement(e, element, ll->elementSize);
}

void LL_SingleReverse(LL_Single *ll)
{
    _SinglyLinkedList rawList = ll->start;
}

void LL_SingleRemove(LL_Single *ll, usz at)
{

}

void LL_DoubleAppend(LL_Double *ll, )
{

}

void LL_DoubleInsert(LL_Double *ll, )
{

}

static void _SinglyLinkedList_SetElement(_SinglyLinkedList *element, void *data, usz size)
{
    M_Copy(element->element, data, size);
}

