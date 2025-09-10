#include <kiek/kiek_vulkan.h>
#include <core/os_vulkan.h>
#include <core/memory.h>
#include <core/types.h>
#include <core/utils.h>
#include <core/log.h>

#define KIEK_ENGINE_ID_STRING "KIEK!"

#define KIEK_TRACE(...) F_LOG_T(OS_STDERR, "KIEK_TRACE", ANSI_COLOR_YELLOW, __VA_ARGS__)
#define VULKAN_SETUP_CHECK(call)                                                        \
    MACRO_START                                                                         \
        VkResult vkRs = call;                                                           \
        if (vkRs != VK_SUCCESS) {                                                       \
            F_LOG_T(OS_STDERR, "FATAL", ANSI_COLOR_RED, "failed to setup renderer:\n"   \
                                                        #call " failed with error %s",  \
                                                        string_VkResult(vkRs));         \
            ABORT();                                                                    \
        }                                                                               \
    MACRO_END                                                                           \

/* static function declaration start */
static void SetApplicationVersionHeader(Kiek_ApplicationVersionHeader *versionHeader, Kiek_ApplicationVersionHeader *userArg);
static void GetRequiredInstanceExtensions(M_Array *requiredExtensions);
static b32  RequiredInstanceExtensionsPresent(const M_Array requiredExtensions);
/* static function declaration end */

void Kiek_VulkanStartup(Kiek_VulkanContext *kvk, const Kiek_VulkanContextCreateInfo kvkInfo)
{
    /* set application info */
    Kiek_ApplicationVersionHeader versionHeader = {0};
    SetApplicationVersionHeader(&versionHeader, kvkInfo.appVersionHeader);
    VkApplicationInfo appInfo = {
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName   = kvkInfo.appName,
        .applicationVersion = VK_MAKE_VERSION(versionHeader.major, versionHeader.minor, versionHeader.patch),
        .pEngineName        = KIEK_ENGINE_ID_STRING,
        .engineVersion      = VK_MAKE_VERSION(KIEK_ENGINE_VERSION_MAJOR, KIEK_ENGINE_VERSION_MINOR, KIEK_ENGINE_VERSION_PATCH),
    };

    /* initialize the Vulkan API */
    M_Array instanceExtensions = {0};
    GetRequiredInstanceExtensions(&instanceExtensions);
    ASSERT_RT(RequiredInstanceExtensionsPresent(instanceExtensions), "required vulkan extensions not available"); // works!!
    VkInstanceCreateInfo instanceInfo = {
        .sType                      = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo           = &appInfo,
        .enabledExtensionCount      = instanceExtensions.count,
        .ppEnabledExtensionNames    = instanceExtensions.data,
    };
    VULKAN_SETUP_CHECK(vkCreateInstance(&instanceInfo, NULL, &kvk->instance));
    M_ArrayDelete(instanceExtensions);

    ABORT();
}

void Kiek_VulkanShutdown(Kiek_VulkanContext *kvk)
{
    KIEK_TRACE("shutting down...");
    vkDestroyInstance(kvk->instance, NULL);
}

#ifndef KIEK_APP_VERISON_MAJOR
#   define KIEK_APP_VERSION_MAJOR  0
#endif /* KIEK_APP_VERSION_MAJOR */
#ifndef KIEK_APP_VERSION_MINOR
#   define KIEK_APP_VERSION_MINOR  0
#endif /* KIEK_APP_VERSION_MINOR */
#ifndef KIEK_APP_VERSION_PATCH
#   define KIEK_APP_VERSION_PATCH  1
#endif /* KIEK_APP_VERSION_PATCH */

#define KIEK_APPLICATION_VERSION_HEADER_DEFAULT (Kiek_ApplicationVersionHeader) { KIEK_APP_VERSION_MAJOR, KIEK_APP_VERSION_MINOR, KIEK_APP_VERSION_PATCH }

static void SetApplicationVersionHeader(Kiek_ApplicationVersionHeader *versionHeader, Kiek_ApplicationVersionHeader *userArg)
{
    *versionHeader = KIEK_APPLICATION_VERSION_HEADER_DEFAULT;
    if (userArg) {
        KIEK_TRACE("No \"KIEK!\" application version passed! falling back to default Testing version...");
        *versionHeader = *userArg;
    }

    KIEK_TRACE("\nKIEK!-Vulkan-App Information\n"
               "\tKiek-Version-Major\t:%d\n"
               "\tKiek-Version-Minor\t:%d\n"
               "\tKiek-Version-Patch\t:%d\n",
               versionHeader->major,
               versionHeader->minor,
               versionHeader->patch);
}


static void GetRequiredInstanceExtensions(M_Array *requiredExtensions)
{
    OS_WindowManagerExtensions wmExtensions = {0};
    OS_WmGetRequiredExtensions(&wmExtensions);

    M_ArrayInit(requiredExtensions, sizeof(*wmExtensions.names), wmExtensions.count);
    for (u32 i = 0; i < wmExtensions.count; ++i) {
        /* don't panic, 'OS_WmGetRequiredExtensions' returns a static string array */
        KIEK_TRACE("required extension %d:\t%s", i, wmExtensions.names[i]);
        M_ArrayAppend(requiredExtensions, (void *)wmExtensions.names[i]);
    }
}

#define GET_MAJOR_VERSION(ver)  (((u32)(ver)>>22U)&0xFF)
#define GET_MINOR_VERSION(ver)  (((u32)(ver)>>12U)&0xFF)
#define GET_PATCH(ver)          (ver&0xFF)

static b32 RequiredInstanceExtensionsPresent(const M_Array requiredExtensions)
{
    b32 requiredExtensionPresent = FALSE;
    char **requiredExtensionNames = requiredExtensions.data;
    VkExtensionProperties *presentExtensions = NULL;
    u32 presentExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &presentExtensionCount, NULL);
    KIEK_TRACE("extension count %d", presentExtensionCount);
    presentExtensions = M_Alloc(sizeof(*presentExtensions), presentExtensionCount);
    vkEnumerateInstanceExtensionProperties(NULL, &presentExtensionCount, presentExtensions);

    KIEK_TRACE("present extensions:");
    for (u32 i = 0; i < requiredExtensions.count; ++i) {
        requiredExtensionPresent = FALSE;
        for (u32 j = 0; j < presentExtensionCount; ++j) {
            KIEK_TRACE("- %s", presentExtensions[j].extensionName);
            KIEK_TRACE("present %d -> %s | required %d -> %s", i, presentExtensions[j].extensionName, j, requiredExtensionNames[i]);
            if (CStr_Compare(requiredExtensionNames[i], presentExtensions[j].extensionName)) {
                KIEK_TRACE("extension \"%s\" for spec version %d.%d.%d found",
                            GET_MAJOR_VERSION(presentExtensions[j].specVersion),
                            GET_MINOR_VERSION(presentExtensions[j].specVersion),
                            GET_PATCH(presentExtensions[j].specVersion),
                            presentExtensions[j].extensionName);

                requiredExtensionPresent = TRUE;
                break;
            }
        }
        if (FALSE == requiredExtensionPresent) {
            break;
        }
    }

    M_Free(presentExtensions);
    return requiredExtensionPresent;
}


