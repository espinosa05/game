#ifndef __BE_BE_EVENT_H__
#define __BE_BE_EVENT_H__

#include <core/types.h>
#include <core/memory_macros.h>

enum e_be_event_types {
    BE_EVENT_INPUT,
    BE_EVENT_LAYER_TRANSITION,
};

typedef struct {
    b32 handled;
    usz type;
} BeEvent;

typedef struct {
    MM_QUEUE_MEMBERS(BeEvent);
} BeEventQueue;

#endif /* __BE_BE_EVENT_H__ */
