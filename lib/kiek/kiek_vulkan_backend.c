#include <kiek/kiek_vulkan_backend.h>
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
    MACRO_END

/* static function declaration start */
static void SetApplicationVersionHeader(Kiek_ApplicationVersionHeader *versionHeader, Kiek_ApplicationVersionHeader *userArg);
static void GetRequiredInstanceExtensionNames(M_Array *requiredExtensionNames);
static void GetPresentInstanceExtensionProperties(M_Array *presentExtensionArray);
static b32  RequiredInstanceExtensionsPresent(const M_Array requiredExtensionNameArray);
/* static function declaration end */

void Kiek_VulkanStartup(Kiek_VulkanContext *kvk, const Kiek_VulkanContextCreateInfo kvkInfo)
{
    KIEK_TRACE("yarrak");
    ABORT();
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
    GetRequiredInstanceExtensionNames(&instanceExtensions);
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
    if (!userArg) {
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


static void GetRequiredInstanceExtensionNames(M_Array *requiredExtensionNames)
{
    OS_WindowEnvironmentExtensions weExtensions = {0};
    OS_WeGetRequiredExtensions(&weExtensions);

    M_ArrayInit(requiredExtensionNames, sizeof(*weExtensions.names), weExtensions.count);
    for (u32 i = 0; i < weExtensions.count; ++i) {
        /* don't panic, 'OS_WeGetRequiredExtensions' returns a static string array */
        KIEK_TRACE("required extension %d:\t%s", i, weExtensions.names[i]);
        M_ArrayAppend(requiredExtensionNames, (void *)weExtensions.names[i]);
    }
}

#define GET_MAJOR_VERSION(ver)  (((u32)(ver)>>22U)&0xFF)
#define GET_MINOR_VERSION(ver)  (((u32)(ver)>>12U)&0xFF)
#define GET_PATCH(ver)          (ver&0xFF)

static void GetPresentInstanceExtensionProperties(M_Array *presentExtensionArray)
{
    u32 count = 0;
    VULKAN_SETUP_CHECK(vkEnumerateInstanceExtensionProperties(NULL, &count, NULL));
    M_ArrayInit(presentExtensionArray, sizeof(VkExtensionProperties), count);
    VULKAN_SETUP_CHECK(vkEnumerateInstanceExtensionProperties(NULL, &count, presentExtensionArray->data));
}

static b32 RequiredInstanceExtensionsPresent(const M_Array requiredExtensionNameArray)
{
    ASSERT(requiredExtensionNameArray.count,    "array of size 0 passed!!");
    ASSERT(requiredExtensionNameArray.data,     "array pointing to NULL passed!!");

    M_Array presentExtensionArray = {0};
    GetPresentInstanceExtensionProperties(&presentExtensionArray);

    char **requiredExtensionNames = requiredExtensionNameArray.data;
    VkExtensionProperties *presentExtensions = presentExtensionArray.data;

    b32 requiredExtensionsPresent = FALSE;
    for (u32 i = 0; i < requiredExtensionNameArray.count; ++i) {
        requiredExtensionsPresent = FALSE;
        for (u32 j = 0; j < presentExtensionArray.count; ++j) {
            if (CStr_Compare(presentExtensions[j].extensionName, requiredExtensionNames[i])) {
                requiredExtensionsPresent = TRUE;
                break;
            }
        }
        if (!requiredExtensionsPresent) {
            break;
        }
    }

    M_Free(presentExtensions);
    return requiredExtensionsPresent;
}


