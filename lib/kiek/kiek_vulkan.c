#include <kiek/kiek_vulkan.h>
#include <core/types.h>
#include <core/utils.h>

#define ERR_MESSAGE_INCOMPATIBLE_DRIVER             "Vulkan Driver not compatible! Make sure your drivers are up to date!"
#define ERR_MESSAGE_NO_SUITABLE_VK_DEVICE_PRESENT   "No suitable vulkan device found! Make sure your GPU supports Vulkan and that your drivers are up to date!"
#define ERR_MESSAGE_VK_CREATE_DEVICE_FAILED         "Failed to setup Vulkan device!"
#define ERR_MESSAGE_VK_CREATE_INSTANCE_FAILED       "Could not start Vulkan!"

#define VK_ASSERT(call, ...)                                                                    \
    MACRO_START                                                                                 \
        VkStatus vkSt = call;                                                                   \
        ASSERT_RT(VK_SUCCESS == vkSt, "Error while calling "#call": ("U64_X_FMT") "STR_FMT":",  \
                                      vkSt, GetVulkanErrorString(vkSt))                         \
    MACRO_END

typedef struct {
    u32                     count;
    char                    **names;
    VkExtensionProperties   *properties;
} VulkanExtensions;

/* static function declaration start */
static b32 VulkanExtensionsPresent(const OS_WindowManagerExtensions extensions);
static void VulkanExtensionsEnumerate(VulkanExtensions *presentExtensions);
static void VulkanExtensionsCleanup(const VulkanExtensions presentExtensions);
/* static function declaration end */

void Kiek_VulkanStartup(Kiek_VulkanContext *kvk, const Kiek_VulkanContextCreateInfo kvkInfo)
{
    OS_WindowManagerExtensions wmExtensions = {0};
    OS_WmGetRequiredExtensions(&wmExtensions);
    ASSERT_RT(VulkanExtensionsPresent(wmExtensions), ERR_MESSAGE_INCOMPATIBLE_DRIVER);

    VkInstanceCreateInfo instanceInfo = {
        .sType                      = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .enabledExtensionCount      = wmExtensions.count,
        .ppEnabledExtensionNames    = wmExtensions.names,
    };
    VK_ASSERT(vkCreateInstance(&instanceInfo, NULL, &kvk->instance), ERR_MESSAGE_VK_CREATE_INSTANCE_FAILED);
    VK_ASSERT(vk);

    OS_WmCleanupRequiredExtensions(wmExtensions);
}

void Kiek_VulkanShutdown(Kiek_VulkanContext *kvk)
{

}

static b32 VulkanExtensionsPresent(const OS_WindowManagerExtensions extensions)
{
    b32 requiredExtensionPresent;
    VulkanExtensions presentExtensions = {0};

    VulkanExtensionsEnumerate(&presentExtensions);

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

    VulkanExtensionsCleanup(presentExtensions);

    return requiredExtensionPresent;
}

static void VulkanExtensionsEnumerate(VulkanExtensions *presentExtensions)
{
    u32 extensionPropertyCount;

    extensionPropertyCount = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extensionPropertyCount, NULL);
    presentExtensions.properties = M_Alloc(sizeof(*extensionProperties), extensionPropertyCount);
    presentExtensions.names = M_Alloc(sizeof(char *), extensionPropertyCount);
    vkEnumerateInstanceExtensionProperties(NULL, &extensionPropertyCount, &extensionProperties);

    for (u32 i = 0; i < extensionPropertyCount; ++i) {
        presentExtensions.names[i] = extensions.properties[i].extensionName;
    }

}

static void VulkanExtensionsCleanup(const VulkanExtensions presentExtensions)
{
    M_Free(presentExtensions.names);
    M_Free(presentExtensions.properties);
}
