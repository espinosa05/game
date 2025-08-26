#ifndef __CORE_LL_H__
#define __CORE_LL_H__

#include <core/types.h>
#include <core/memory.h>

typedef struct {
    M_Arena *arena;
} LL_SingleInfo;

typedef struct {
    _SinglyLinkedList *next;
    void *element;
} _SinglyLinkedList;

typedef struct {
    _SinglyLinkedList *start;
    _SinglyLinkedList *end;
    M_Arena *arena;
    usz elementSize;
    usz cap;
} LL_Single;

void LL_SingleAppend(LinkedList *ll, void *element);
void LL_SingleInsert(LinkedList *ll, usz at, void *element);
void LL_SingleRemove(LinkedList *ll, usz at);

typedef struct _DoublyLinkedList {
    _DoublyLinkedList *next;
    _DoublyLinkedList *prev;
    void *element;
} _DoublyLinkedList;

typedef struct {
    _DoublyLinkedList *start;
    _DoublyLinkedList *end;
} LL_Double;

#endif /* __CORE_LL_H__ */
