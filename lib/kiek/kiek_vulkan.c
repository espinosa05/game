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

typedef struct {
    u32                     count;
    char                    **names;
    VkExtensionProperties   *properties;
} VulkanInstanceExtensions;

/* static function declaration start */
static void SetApplicationVersionHeader(Kiek_ApplicationVersionHeader *versionHeader, Kiek_ApplicationVersionHeader *userArg);

static b32  RequiredInstanceExtensionsPresent(const OS_WindowManagerExtensions extensions);
static void EnumerateInstanceExtensions(VulkanInstanceExtensions *presentExtensions);
static void CleanupInstanceExtensions(const VulkanInstanceExtensions presentExtensions);

static void EnumeratePresentDevices(M_Array *presentDevices);

static void SelectSuitableDevice(*M_Array presentDevices, VkPhysicalDevice *suitableDevice);

static void EnumerateQueueCreateExtensions();
/* static function declaration end */

void Kiek_VulkanStartup(Kiek_VulkanContext *kvk, const Kiek_VulkanContextCreateInfo kvkInfo)
{
    /* Set Application Info */
    Kiek_ApplicationVersionHeader versionHeader = {0};
    SetApplicationVersionHeader(&versionHeader, kvkInfo.appVersionHeader);
    VkApplicationInfo appInfo = {
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName   = kvkInfo.appName,
        .applicationVersion = VK_MAKE_VERSION(versionHeader.major, versionHeader.minor, versionHeader.patch),
        .pEngineName        = KIEK_ENGINE_ID_STRING,
        .engineVersion      = VK_MAKE_VERSION(KIEK_ENGINE_VERSION_MAJOR, KIEK_ENGINE_VERSION_MINOR, KIEK_ENGINE_VERSION_PATCH),
    };

    OS_WindowManagerExtensions wmExtensions = {0};
    OS_WmGetRequiredExtensions(&wmExtensions);

    /* initialize the Vulkan API */
    ASSERT_RT(RequiredInstanceExtensionsPresent(wmExtensions)); // works!!
    VkInstanceCreateInfo instanceInfo = {
        .sType                      = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo           = &appInfo,
        .enabledExtensionCount      = wmExtensions.count,
        .ppEnabledExtensionNames    = wmExtensions.names,
    };
    VULKAN_SETUP_CHECK(vkCreateInstance(&instanceInfo, NULL, &kvk->instance));
    OS_WmCleanupRequiredExtensions(wmExtensions);

    /* query available device specs */
    M_Array presentDevices = {0};
    VkPhysicalDevice suitableDevice = VK_NULL_HANDLE;
    EnumeratePresentDevices(&presentDevices);
    SelectSuitableDevice(presentDevices, &suitableDevice);
    M_ArrayDelete(presentDevices);

    /* get suitable device information */
    M_Array queueCreateInfos = {0};
    M_Array deviceExtensions = {0};
    EnumerateQueueCreateInfos(suitableDevice, &queueCreateInfos);
    EnumerateDeviceExtensions(suitableDevice, &deviceExtensions);

    /* create logical device handle */
    VkDeviceCreateInfo deviceInfo = {
        .sType                      = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount       = queueCreateInfos.count,
        .pQueueCreateInfos          = queueCreateInfos.data,
        .enabledExtensionCount      = deviceExtensions.count,
        .ppEnabledExtensionNames    = deviceExtensions.data,
        .pEnabledFeatures           = &deviceEnabledFeatures,
    };
    VULKAN_SETUP_CHECK(vkCreateDevice(suitableDevice, &deviceInfo, NULL, &kvk->logicalDevice.handle));

    M_ArrayDelete(queueCreateInfos);
    M_ArrayDelete(deviceExtensions);
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

    KIEK_TRACE("KIEK!-Vulkan-App Information\n"
               "\tKiek-Version-Major\t:%d\n",
               "\tKiek-Version-Minor\t:%d\n",
               "\tKiek-Version-Patch\t:%d\n",
               versionHeader->major,
               versionHeader->minor,
               versionHeader->patch);
}

static b32 RequiredInstanceExtensionsPresent(const OS_WindowManagerExtensions extensions)
{
    b32 requiredExtensionPresent;
    VulkanInstanceExtensions presentExtensions = {0};
    EnumerateInstanceExtensions(&presentExtensions);

    for (u32 i = 0; i < extensions.count; ++i) {
        requiredExtensionPresent = FALSE;
        for (u32 j = 0; j < presentExtensions.count; ++j) {
            usz extensionLength = CStr_Length(presentExtensions.names[j]);
            if (CStr_Compare(extensions.names[i], presentExtensions.names[j], extensionLength)) {
                requiredExtensionPresent = TRUE;
                break;
            }
        }

        if (FALSE == requiredExtensionPresent) {
            break;
        }
    }
    CleanupInstanceExtensions(presentExtensions);

    return requiredExtensionPresent;
}

static void EnumerateInstanceExtensions(VulkanInstanceExtensions *presentExtensions)
{
    presentExtensions->count         = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &presentExtensions->count, NULL);
    presentExtensions->properties    = M_Alloc(sizeof(*presentExtensions->properties),  presentExtensions->count);
    presentExtensions->names         = M_Alloc(sizeof(*presentExtensions->names),       presentExtensions->count);
    vkEnumerateInstanceExtensionProperties(NULL, &presentExtensions->count, presentExtensions->properties);

    TODO("Create Dynamic Array implementation for properties!");
    for (u32 i = 0; i < presentExtensions->count; ++i) {
        presentExtensions->names[i] = presentExtensions->properties[i].extensionName;
    }

}

static void CleanupInstanceExtensions(const VulkanInstanceExtensions presentExtensions)
{
    M_Free(presentExtensions.names);
    M_Free(presentExtensions.properties);
}

static void EnumerateDevices(VulkanDevices *presentDevices)
{
    presentDevices->count   = 0;
    VULKAN_SETUP_CHECK(vkEnumeratePhysicalDevices(NULL, &presentDevices->count, NULL));
    presentDevices->handles = M_Alloc(sizeof(*presentDevices->handles), presentDevices->count);
    vkEnumeratePhysicalDevices(NULL, &presentDevices->count, presentDevices->handles);
}

static void CleanupDevices(const VulkanDevices presentDevices)
{
    M_Free(presentDevices.handles);
}

static void EnumerateDeviceExtensions(const VkPhysicalDevice device, VulkanDeviceExtensions *deviceExtensions)
{
    deviceExtensions->count         = 0;
    vkEnumerateDeviceExtensionProperties(device, NULL, &deviceExtensions->count);
    deviceExtensions->properties    = M_Alloc(sizeof(*deviceExtensions->properties), deviceExtensions->count);
}

static void CleanupDeviceExtensions(const VulkanDeviceExtensions deviceExtensions)
{
    M_Free(deviceExtensions.properties);
}

static void GetSuitableDevice(VulkanDeviceData *deviceData)
{
    VulkanDevices devices = {0};
    EnumerateDevices(&devices);

    for (u32 i = 0; i < devices.count; ++i) {
        VulkanDeviceExtensions deviceExtensions = {0}
        EnumerateDeviceExtensions(&deviceExtensions);
    }
}
