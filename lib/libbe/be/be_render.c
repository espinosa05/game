
#include <be/be_render.h>
#include <be/be_window.h>
#include <core/utils.h>
#include <core/log.h>
#include <core/wm_vulkan.h>

#pragma GCC diagnostic ignored "-Wunused"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wreturn-type"


#define BE_VULKAN_SWAPCHAIN_IMAGE_COUNT (2)
#define BE_VULKAN_STATUS_INIT (-1)

#define BE_VULKAN_API_VERSION VK_API_VERSION_1_4
#define BE_RENDER_CONTEXT_FMT "{ instance: 0x"USZ_X_FMT", surface: 0x"USZ_X_FMT", physical_device: 0x"USZ_X_FMT", logical_device: 0x"USZ_X_FMT", graphics_queue: { index: "USZ_FMT", handle: 0x"USZ_X_FMT" } }"
#define BE_RENDER_CONTEXT_FMT_ARG(rc) (rc).instance.handle, (rc).surface.handle, (rc).physical_device.handle, (rc).logical_device.handle, (rc).graphics_queue.index, (rc).graphics_queue.handle

#define VK_CALL(c)                                                                                                  \
    MACRO_START                                                                                                     \
        VkResult rc = (c);                                                                                          \
        if (VK_SUCCESS != rc) {                                                                                     \
            F_LOG_T(OS_STDERR, ANSI_COLOR_RED, "VULKAN", "call to "STR_QUOT(#c)" failed! "STR_QUOT(USZ_FMT), rc);   \
            ABORT();                                                                                                \
        }                                                                                                           \
    MACRO_END

#define BE_VK_CALL(c)                                                                       \
    MACRO_START                                                                             \
        b32 rc = (c);                                                                       \
        if (!rc) {                                                                          \
            F_LOG_T(OS_STDERR, ANSI_COLOR_RED, "BE-VK", "call to "STR_QUOT(#c)" failed!");  \
            ABORT();                                                                        \
        }                                                                                   \
    MACRO_END

/* idfk, man. I'm tired */
typedef BeRenderContext BeRenderContextSpec;

/* static function declaration start */
static b32 be_vulkan_create_instance(BeVulkanInstance *instance);
static b32 be_vulkan_create_surface(BeEngine *be, BeVulkanSurface *surface, BeVulkanInstance instance, BeArena *arena);
static b32 be_vulkan_find_suitable_device(BeVulkanPhysicalDevice *physical_device, BeVulkanInstance instance, BeArena *arena);
static b32 be_vulkan_find_graphics_queue(BeVulkanQueue *gfx_queue, BeVulkanPhysicalDevice physical_device, BeVulkanSurface surface, BeArena *arena);
static b32 be_vulkan_create_device(BeVulkanDevice *logical_device, BeVulkanPhysicalDevice physical_device, BeVulkanQueue graphics_queue, BeArena *arena);
static b32 be_vulkan_create_swapchain(BeVulkanSwapchain *swapchain, BeVulkanPhysicalDevice physical_device, BeVulkanSurface surface);
/* static function declaration end */

BeRenderContext *be_render_on_attach(BeEngine *be)
{
    BeRenderContext *render_context = be_alloc_perm(be, sizeof(*render_context), 1);

    BeArena temp_vulkan_arena = {0};
    be_arena_init(&temp_vulkan_arena, KB(2));

    BeVulkanInstance instance = {0};
    BE_VK_CALL(be_vulkan_create_instance(&instance));

    BeVulkanSurface surface = {0};
    BE_VK_CALL(be_vulkan_create_surface(be, &surface, instance, &temp_vulkan_arena));

    BeVulkanPhysicalDevice physical_device = {0};
    BE_VK_CALL(be_vulkan_find_suitable_device(&physical_device, instance, &temp_vulkan_arena));

    BeVulkanQueue graphics_queue = {0};
    BE_VK_CALL(be_vulkan_find_graphics_queue(&graphics_queue, physical_device, surface, &temp_vulkan_arena));

    BeVulkanDevice logical_device = {0};
    BE_VK_CALL(be_vulkan_create_device(&logical_device, physical_device, graphics_queue, &temp_vulkan_arena));

    BeVulkanSwapchain swapchain = {0};
    BE_VK_CALL(be_vulkan_create_swapchain(&swapchain, physical_device, surface, surface_format));

    be_arena_delete(&temp_vulkan_arena);

    *render_context = (BeRenderContext) {
        instance,
        surface,
        physical_device,
        logical_device,
        graphics_queue,
        swapchain,
    };

    INFO_LOG("BE-RENDER-CONTEXT: "BE_RENDER_CONTEXT_FMT, BE_RENDER_CONTEXT_FMT_ARG(*render_context));

    return render_context;
}

void be_render_on_update(BeEngine *be, BeRenderContext *render)
{
    UNUSED(be);
    UNUSED(render);
}

void be_render_on_suspend(BeEngine *be, BeRenderContext *render)
{
    UNUSED(be);
    UNUSED(render);
}

void be_render_on_activate(BeEngine *be, BeRenderContext *render)
{
    UNUSED(be);
    UNUSED(render);
}

void be_render_on_event(BeEngine *be, BeRenderContext *render)
{
    UNUSED(be);
    UNUSED(render);
}

void be_render_on_detach(BeEngine *be, BeRenderContext *render)
{
    INFO_LOG("shutting down Vulkan backend");
    vkDestroySurfaceKHR(render->instance, handle, render->surface.handle, NULL);
    vkDestroyDevice(render->logical_device.handle, NULL);
    vkDestroyInstance(render->instance.handle, NULL);
}

static b32 be_vulkan_create_instance(BeVulkanInstance *instance)
{
    VkApplicationInfo app_info = {0};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "be_render";
    app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    app_info.pEngineName = "BLEEDING-EDGE";
    app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    app_info.apiVersion = BE_VULKAN_API_VERSION;

    struct wm_vulkan_extensions wm_extensions = {0};
    wm_vulkan_extensions_get_required(&wm_extensions);

    VkInstanceCreateInfo instance_info = {0};
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;
    instance_info.enabledExtensionCount = wm_extensions.count;
    instance_info.ppEnabledExtensionNames = wm_extensions.names;
    VK_CALL(vkCreateInstance(&instance_info, NULL, &instance->handle));

    return TRUE;
}

static b32 be_vulkan_create_surface(BeEngine *be, BeVulkanSurface *surface, BeVulkanInstance instance, BeArena *arena)
{
    BeLayer *window_layer = be_get_layer_by_name(be, "be_window");
    ASSERT(window_layer, "failed to get "STR_QUOT("be_window")" layer");

    BeWindow *win_context = window_layer->context;

    usz st = 0;
    struct wm_vulkan_surface window_surface = {0};
    struct wm_vulkan_surface_info window_surface_info = {0};
    window_surface_info.instance = instance.handle;
    window_surface_info.wm = &win_context->wm;
    window_surface_info.win = &win_context->main_window;
    st = wm_vulkan_surface_create(&window_surface, window_surface_info);
    if (st != WM_VULKAN_SURFACE_STATUS_SUCCESS) {
        ERROR_LOG("failed to create vulkan surface: "STR_FMT, wm_get_vulkan_surface_status_str(st));
        return FALSE;
    }

    surface->handle = window_surface.handle;
    return TRUE;
}

static b32 be_vulkan_find_suitable_device(BeVulkanPhysicalDevice *physical_device, BeVulkanInstance instance, BeArena *arena)
{
    u32 device_count = 0;
    BeVulkanPhysicalDevices devices = {0};
    vkEnumeratePhysicalDevices(instance.handle, &device_count, NULL);
    void *buff = be_arena_alloc(arena, sizeof(*devices.data), device_count);
    mm_array_init_ext(&devices, buff, device_count);
    mm_array_reserve(&devices, device_count);
    vkEnumeratePhysicalDevices(instance.handle, &device_count, devices.data);

    b32 discrete_found = FALSE;
    b32 integrated_found = FALSE;
    for (usz i = 0; i < devices.count; ++i) {
        VkPhysicalDevice device = devices.data[i];
        VkPhysicalDeviceProperties properties = {0};
        vkGetPhysicalDeviceProperties(device, &properties);
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            discrete_found = TRUE;
            physical_device->handle = device;
            break;
        } else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU
                    && !discrete_found) {
            INFO_LOG("found integrated GPU!");
            integrated_found = TRUE;
            physical_device->handle = device;
            break;
        }
    }
    return format_supported && (discrete_found || integrated_found);
}

static b32 be_vulkan_find_graphics_queue(BeVulkanQueue *gfx_queue, BeVulkanPhysicalDevice physical_device, BeVulkanSurface surface, BeArena *arena)
{
    u32 queue_family_count = 0;
    BeVulkanQueueFamilyProperties queue_properties = {0};
    vkGetPhysicalDeviceQueueFamilyProperties2(physical_device.handle, &queue_family_count, NULL);
    void *buff = be_arena_alloc(arena, sizeof(*queue_properties.data), queue_family_count);
    mm_array_init_ext(&queue_properties, buff, queue_family_count);
    mm_array_reserve(&queue_properties, queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties2(physical_device.handle, &queue_family_count, queue_properties.data);

    for (usz i = 0; i < queue_properties.count; ++i) {
        VkBool32 present_support = VK_FALSE;
        VkQueueFamilyProperties2 property = {0};
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device.handle, i, surface.handle, &present_support);
        property = queue_properties.data[i];

        if (property.queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT && present_support) {
            gfx_queue->index = i;
        }
    }
    return TRUE;
}

#define GFX_QUEUE_COUNT 1
#define CMP_QUEUE_COUNT 1
static b32 be_vulkan_create_device(BeVulkanDevice *logical_device, BeVulkanPhysicalDevice physical_device, BeVulkanQueue graphics_queue, BeArena *arena)
{
    UNUSED(arena);

    VkPhysicalDeviceVulkan14Features supported_features_1_4 = {0};
    supported_features_1_4.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;
    supported_features_1_4.pNext = NULL;

    VkPhysicalDeviceVulkan13Features supported_features_1_3 = {0};
    supported_features_1_3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    supported_features_1_3.pNext = &supported_features_1_4;

    VkPhysicalDeviceVulkan12Features supported_features_1_2 = {0};
    supported_features_1_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    supported_features_1_2.pNext = &supported_features_1_3;

    VkPhysicalDeviceFeatures2 supported_features_2 = {0};
    supported_features_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    supported_features_2.pNext = &supported_features_1_2;
    vkGetPhysicalDeviceFeatures2(physical_device.handle, &supported_features_2);

    if (!supported_features_1_3.dynamicRendering ||
            !supported_features_1_3.synchronization2 ||
            !supported_features_1_2.timelineSemaphore) {
        ERROR_LOG("Selected Device doesn't meet feature requirements");
        return FALSE;
    }

    VkPhysicalDeviceVulkan14Features features_1_4 = {0};
    features_1_4.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;
    features_1_4.pNext = NULL;

    VkPhysicalDeviceVulkan13Features features_1_3 = {0};
    features_1_3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features_1_3.pNext = &features_1_4;
    features_1_3.synchronization2 = VK_TRUE;
    features_1_3.dynamicRendering = VK_TRUE;

    VkPhysicalDeviceVulkan12Features features_1_2 = {0};
    features_1_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features_1_2.pNext = &features_1_3;
    features_1_2.timelineSemaphore = VK_TRUE;

    VkPhysicalDeviceFeatures2 features_2 = {0};
    features_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features_2.pNext = &features_1_2;

    f32 queue_priorities[GFX_QUEUE_COUNT] = {F32(1.0)};

    VkDeviceQueueCreateInfo graphics_queue_info = {0};
    graphics_queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    graphics_queue_info.queueFamilyIndex = graphics_queue.index;
    graphics_queue_info.queueCount = ARRAY_SIZE(queue_priorities);
    graphics_queue_info.pQueuePriorities = queue_priorities;

    VkDeviceCreateInfo device_info = {0};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.pNext = &features_2;
    device_info.queueCreateInfoCount = GFX_QUEUE_COUNT;
    device_info.pQueueCreateInfos = &graphics_queue_info;
    VK_CALL(vkCreateDevice(physical_device.handle, &device_info, NULL, &logical_device->handle));

    return TRUE;
}

static b32 be_vulkan_create_swapchain(BeVulkanSwapchain *swapchain, BeVulkanPhysicalDevice physical_device, BeVulkanSurface surface)
{
    BeLayer *window_settings = be_get_layer_by_name(be, "be_window");
    struct wm_window_info *main_window_info = window_settings->context;

    VkStatus st = BE_VULKAN_STATUS_INIT;
    VkSurfaceCapabilitiesKHR surface_capabilities = {0};
    VK_CALL(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice.handle, surface.handle, &surface_capabilities));

    u32 image_count = CORE_MAX(BE_VULKAN_SWAPCHAIN_IMAGE_COUNT, surface_capabilities.minImageCount);
    if (surface_capabilities.maxImageCount) {
        image_count = CORE_MIN(image_count, surface_capabilities.maxImageCount);
    }

    VkExtend2D image_extend = {0};
    image_extend.width = U32(main_window_info->width);
    image_extend.height = U32(main_window_info->height);

    VkSwapchainCreateInfoKHR swapchain_info = {0};
    swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_info.minImageCount = image_count;
    swapchain_info.swapchainFormat = swapchain_format.handle;
    swapchain_info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    swapchain_info.imageExtent = image_extend;
    swapchain_info.imageArrayLayers = 1;
    swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_info.preTransform = surface_capabilities.currentTransform;
    swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
}
