#include <cr/vulkan.h>

#define ERR_MESSAGE_NO_SUITABLE_VK_DEVICE_PRESENT   "No suitable vulkan device found! Make sure your GPU supports Vulkan and update your drivers!"
#define ERR_MESSAGE_VK_CREATE_DEVICE_FAILED         "Failed to setup Vulkan device!"
#define ERR_MESSAGE_VK_CREATE_INSTANCE_FAILED       "Could not start Vulkan!"

#define VK_ASSERT(call, ...)                                                                    \
    MACRO_START                                                                                 \
        VkStatus vkSt = call;                                                                   \
        ASSERT_RT(VK_SUCCESS == vkSt, "Error while calling "#call": ("U64_X_FMT") "STR_FMT":",  \
                                      vkSt, GetVulkanErrorString(vkSt))                         \
    MACRO_END


