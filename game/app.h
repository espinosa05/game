#ifndef __APP_H__
#define __APP_H__

#include <core/types.h>
#include <core/memory.h>
#include <core/cli.h>

#include <kiek/kiek_vulkan_backend.h>

typedef struct {
    CLI_Args    args;
    const char  *name;
} App_ContextCreateInfo;

typedef struct {
    Kiek_VulkanContext  kvk;
    M_Arena             *memoryArena;
} App_Context;

void App_Init(App_Context *app, const App_ContextCreateInfo *info);
void App_Run(App_Context *app);
void App_Cleanup(App_Context *app);

#endif /* __APP_H__ */
