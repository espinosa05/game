#ifndef __BE_BE_ENGINE_H__
#define __BE_BE_ENGINE_H__

#include <be/be_layer.h>
#include <be/be_arena.h>
#include <core/types.h>
#include <core/os.h>
#include <core/cli.h>

typedef struct {
    MM_ARRAY_MEMBERS(BeLayer);
} BeLayers;

typedef struct {
    struct os_time start;
    struct os_time end;
} BeFrameTime;

typedef struct s_be_engine BeEngine;
struct s_be_engine {
    b32 run;
    f64 delta;

    BeFrameTime frame_time;
    BeArena     transient;
    BeArena     permanent;
    BeLayers    layers;
};

void be_engine_init(BeEngine *be, struct cli_args args);
void be_engine_run(BeEngine *be);
void be_engine_delete(BeEngine *be);

#endif /* __BE_BE_BENGINE_H__ */
