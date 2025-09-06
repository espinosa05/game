#include <kiek/kiek_vulkan.h>
#include <core/types.h>
#include <core/utils.h>

#define KIEK_ENGINE_ID_STRING "KIEK!"

#define ERR_MESSAGE_INCOMPATIBLE_DRIVER             "Vulkan Driver not compatible! Make sure your drivers are up to date!"
#define ERR_MESSAGE_NO_SUITABLE_VK_DEVICE_PRESENT   "No suitable vulkan device found! Make sure your GPU supports Vulkan and that your drivers are up to date!"
#define ERR_MESSAGE_VK_CREATE_DEVICE_FAILED         "Failed to setup Vulkan device!"
#define ERR_MESSAGE_VK_CREATE_INSTANCE_FAILED       "Could not start Vulkan!"

#define VULKAN_ASSERT(call, ...)                                                                \
    MACRO_START                                                                                 \
        VkStatus vkSt = call;                                                                   \
        if (UNLIKELY(vkStatus != VK_SUCCESS)) {                                                 \
            F_LOG_T(OS_STDERR, "VULKAN", __VA_ARGS__);                                          \
            F_LOG(OS_STDERR, "\t> while calling ("#call") -> %s\n", string_VkStatus(vkSt));     \
            ABORT();                                                                            \
        }                                                                                       \
    MACRO_END

typedef struct {
    u32                     count;
    char                    **names;
    VkExtensionProperties   *properties;
} VulkanInstanceExtensions;

typedef struct {
    u32                     count;
    VkPhysicalDevices       *devices;
} VulkanDevices;

typedef struct {
    u32                     count;
    VkExtensionProperties   *properties;
} VulkanDeviceExtensions;

typedef struct {
    VkPhysicalDevice            handle;
    u32                         queueCreateInfoCount;
    VkDeviceQueueCreateInfo     *queueCreateInfos;
    u32                         enabledExtensionCount;
    const char                  **enabledExtensionNames;
    VkPhysicalDeviceFeatures    enabledFeatures;
} VulkanDeviceData;

/* static function declaration start */
static void SetApplicationVersionHeader(Kiek_ApplicationVersionHeader *versionHeader, Kiek_ApplicationVersionHeader *userArg);

static b32  RequiredInstanceExtensionsPresent(const OS_WindowManagerExtensions extensions);
static void EnumerateInstanceExtensions(VulkanInstanceExtensions *presentExtensions);
static void CleanupInstanceExtensions(const VulkanInstanceExtensions presentExtensions);

static void EnumerateDevices(const Kiek_VulkanContext *kvk, VulkanDevices *presentDevices);
static void EnumerateDeviceExtensions(const VkPhysicalDevice device, VulkanDeviceExtensions *deviceExtensions);
static b32  SuitableDevicePresent(Kiek_VulkanContext *kvk, VulkanGraphicsAdapter *device);
/* static function declaration end */

void Kiek_VulkanStartup(Kiek_VulkanContext *kvk, const Kiek_VulkanContextCreateInfo kvkInfo)
{
    /* Set Application Info */
    Kiek_ApplicationVersionHeader versionHeader = {0};
    SetApplicationVersionHeader(&versionHeader, kvkInfo->appVersionHeader);
    VkApplicationInfo appInfo = {
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName   = kvkInfo.appName,
        .applicationVersion = VK_MAKE_VERSION(versionHeader.major, versionHeader.minor, versionHeader.patch),
        .pEngineName        = KIEK_ENGINE_ID_STRING,
        .engineVersion      = VK_MAKE_VERISON(KIEK_ENGINE_VERSION_MAJOR, KIEK_ENGINE_VERSION_MINOR, KIEK_ENGINE_VERSION_PATCH),
    };

    OS_WindowManagerExtensions wmExtensions = {0};
    OS_WmGetRequiredExtensions(&wmExtensions);

    /* initialize the Vulkan API */
    ASSERT_RT(RequiredInstanceExtensionsPresent(wmExtensions), ERR_MESSAGE_INCOMPATIBLE_DRIVER);
    VkInstanceCreateInfo instanceInfo = {
        .sType                      = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .enabledExtensionCount      = wmExtensions.count,
        .ppEnabledExtensionNames    = wmExtensions.names,
    };
    VULKAN_ASSERT(vkCreateInstance(&instanceInfo, NULL, &kvk->instance), ERR_MESSAGE_VK_CREATE_INSTANCE_FAILED);
    OS_WmCleanupRequiredExtensions(wmExtensions);

    /* Find a suitable GPU and create a logical instance */
    VulkanDeviceData suitableGPU = {0};
    ASSSERT_RT(SuitableDevicePresent(&suitableGPU), ERR_MESSAGE_NO_SUITABLE_VK_DEVICE_PRESENT);
    VkDeviceCreateInfo deviceInfo = {
        .sType                      = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount       = suitableGPU.queueCreateInfoCount,
        .pQueueCreateInfos          = suitableGPU.queueCreateInfos,
        .enabledExtensionCount      = suitableGPU.enabledExtensionCount,
        .ppEnabledExtensionNames    = suitableGPU.enabledExtensionNames,
        .pEnabledFeatures           = &suitableGPU.enabledFeatures,
    };
    VULKAN_ASSERT(vkCreateDevice(suitableGPU.handle, &deviceInfo, NULL, &kvk->logicalDevice), ERR_MESSAGE_VK_CREATE_DEVICE_FAILED);
}

void Kiek_VulkanShutdown(Kiek_VulkanContext *kvk)
{

}

static void InitKiekVulkanArena(M_Arena *kiekArena, M_Arena *userArg)
{
    TODO("get arena");
    if (userArg) {
        *kiekArena = userArg;
    }
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
        *versionHeader = userArg;
    }
    KIEK_TRACE("\nKIEK!-Vulkan-App Information:\n"
               "\t");
}

static b32 RequiredInstanceExtensionsPresent(const OS_WindowManagerExtensions extensions)
{
    b32 requiredExtensionPresent;
    VulkanExtensions presentExtensions = {0};
    EnumarateInstanceExtensions(&presentExtensions);

    for (u32 i = 0; i < extensions.count; ++i) {
        requiredExtensionPresent = FALSE;
        for (u32 j = 0; j < presentExtensions.count; ++j) {
            if (CStr_Compare(extensions.names[i], presentExtensions.names[j])) {
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
    u32 extensionPropertyCount;

    extensionPropertyCount = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extensionPropertyCount, NULL);
    presentExtensions.properties = M_Alloc(sizeof(*extensionProperties), extensionPropertyCount);
    presentExtensions.names = M_Alloc(sizeof(char *), extensionPropertyCount);
    vkEnumerateInstanceExtensionProperties(NULL, &extensionPropertyCount, &extensionProperties);

    TODO("Create Dynamic Array implementation for properties!");
    for (u32 i = 0; i < extensionPropertyCount; ++i) {
        presentExtensions.names[i] = extensions.properties[i].extensionName;
    }

}

static void CleanupInstanceExtensions(const VulkanInstanceExtensions presentExtensions)
{
    M_Free(presentExtensions.names);
    M_Free(presentExtensions.properties);
}

static void EnumerateDevices(const Kiek_VulkanContext *kvk, VulkanDevices *presentDevices)
{
    u32 physicalDeviceCount;

    physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(NULL, &physicalDeviceCount, NULL);
    presentDevices.
}

static void EnumerateDeviceExtensions(const VkPhysicalDevice device, VulkanDeviceExtensions *deviceExtensions)
{

}

static b32 SuitableDevicePresent(Kiek_VulkanContext *kvk, VkPhysicalDevice *device)
{
    VulkanDevices presentDevices = {0};
    EnumerateDevices(kvk, &presentDevices);

    for (usz i = 0; i < presentDevices.count; ++i) {
        VulkanDeviceExtensions extensions = {0};
        EnumerateDeviceExtensions(presentDevices.devices[i], &extensions);
    }
}
