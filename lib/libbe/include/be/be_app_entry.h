#ifndef __BE_BE_ENTRY_H__
#define __BE_BE_ENTRY_H__

#include <core/types.h>
#include <core/memory_arena.h>
#include <core/cli.h>
#include <core/wm.h>

typedef struct s_be_engine BeEngine;

#define BE_WINDOW_TARGET_FRAME_MS_UNCAP 0
/* to be implemented by the user */
extern void be_app_entry(BeEngine *be, struct cli_args args);

#endif /* __BE_BE_ENTRY_H__ */
