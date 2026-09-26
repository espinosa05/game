#include <be/be_vk_render.h>
#include <be/be_window.h>
#include <core/utils.h>
#include <core/log.h>
#include <core/cstr.h>
#include <core/wm_vulkan.h>

#pragma GCC diagnostic ignored "-Wunused"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wreturn-type"

#define BE_VULKAN_RESULT_INIT -1

#define BE_VULKAN_SWAPCHAIN_IMAGE_COUNT 2
#define BE_VULKAN_API_VERSION           VK_API_VERSION_1_4
#define BE_VULKAN_SWAPCHAIN_FORMAT      VK_FORMAT_B8G8R8A8_SRGB
#define BE_VULKAN_DEPTH_FORMAT          VK_FORMAT_D32_SFLOAT


#define BE_RENDER_CONTEXT_FMT "{ instance: 0x"USZ_X_FMT", surface: 0x"USZ_X_FMT", physical_device: 0x"USZ_X_FMT", logical_device: 0x"USZ_X_FMT", graphics_queue: { index: "USZ_FMT", handle: 0x"USZ_X_FMT" } }"
#define BE_RENDER_CONTEXT_FMT_ARG(rc) (rc).instance.handle, (rc).surface.handle, (rc).physical_device.handle, (rc).logical_device.handle, (rc).graphics_queue.index, (rc).graphics_queue.handle

#define BE_VULKAN_CHECK(c)                                                                       \
    MACRO_START                                                                             \
        b32 res = (c);                                                                      \
        if (!res) {                                                                         \
            F_LOG_T(OS_STDERR, ANSI_COLOR_RED, "BE-VK", "call to "STR_QUOT(#c)" failed!");  \
            ABORT();                                                                        \
        }                                                                                   \
    MACRO_END

/* static function declaration start */
static BeVkRenderContext *new_rc(BeEngine *be);
static void be_vulkan_get_required_instance_extensions(BeVulkanExtensions *extensions, BeArena *arena);
static void be_vulkan_get_required_device_extensions(BeVulkanExtensions *extensions, BeArena *arena);

static b32 be_vulkan_create_instance(BeEngine *be, BeVkRenderContext *rc, BeVulkanExtensions instance_extensions, BeArena *arena);
static b32 be_vulkan_create_surface(BeEngine *be, BeVkRenderContext *rc, BeArena *arena);
static b32 be_vulkan_find_suitable_device(BeEngine *be, BeVkRenderContext *rc, BeVulkanExtensions device_extensions, BeArena *arena);
static b32 be_vulkan_find_graphics_queue(BeEngine *be, BeVkRenderContext *rc,  BeArena *arena);
static b32 be_vulkan_create_device(BeEngine *be, BeVkRenderContext *rc, BeVulkanExtensions device_extensions, BeArena *arena);
static b32 be_vulkan_create_swapchain(BeEngine *be, BeVkRenderContext *rc, BeArena *arena);

static b32 device_supports_format(VkPhysicalDevice device, BeVulkanSurface surface, VkFormat *format, BeArena *arena);
static b32 required_device_extensions_present(VkPhysicalDevice device, BeVulkanExtensions required, BeArena *arena);
/* static function declaration end */

BeVkRenderContext *be_vk_render_on_attach(BeEngine *be)
{
    BeVkRenderContext *rc = new_rc(be);

    BeArena temp_vulkan_arena = {0};
    be_arena_init(&temp_vulkan_arena, KB(128));

    BeVulkanExtensions instance_extensions = {0};
    be_vulkan_get_required_instance_extensions(&instance_extensions, &temp_vulkan_arena);

    BeVulkanExtensions device_extensions = {0};
    be_vulkan_get_required_device_extensions(&device_extensions, &temp_vulkan_arena);

    BE_VULKAN_CHECK(be_vulkan_create_instance(be, rc, instance_extensions, &temp_vulkan_arena));
    BE_VULKAN_CHECK(be_vulkan_create_surface(be, rc, &temp_vulkan_arena));
    BE_VULKAN_CHECK(be_vulkan_find_suitable_device(be, rc, device_extensions, &temp_vulkan_arena));
    BE_VULKAN_CHECK(be_vulkan_find_graphics_queue(be, rc, &temp_vulkan_arena));
    BE_VULKAN_CHECK(be_vulkan_create_device(be, rc, device_extensions, &temp_vulkan_arena));
    BE_VULKAN_CHECK(be_vulkan_create_swapchain(be, rc, &temp_vulkan_arena));

    be_arena_delete(&temp_vulkan_arena);

    return rc;
}

void be_vk_render_on_update(BeEngine *be, BeVkRenderContext *render)
{
    UNUSED(be);
    UNUSED(render);
}

void be_vk_render_on_suspend(BeEngine *be, BeVkRenderContext *render)
{
    UNUSED(be);
    UNUSED(render);
}

void be_vk_render_on_activate(BeEngine *be, BeVkRenderContext *render)
{
    UNUSED(be);
    UNUSED(render);
}

void be_vk_render_on_event(BeEngine *be, BeVkRenderContext *render)
{
    UNUSED(be);
    UNUSED(render);
}

void be_vk_render_on_detach(BeEngine *be, BeVkRenderContext *render)
{
    INFO_LOG("shutting down Vulkan backend");
    vkDestroySurfaceKHR(render->instance.handle, render->surface.handle, NULL);
    vkDestroyDevice(render->logical_device.handle, NULL);
    vkDestroyInstance(render->instance.handle, NULL);
}

static BeVkRenderContext *new_rc(BeEngine *be)
{
    return be_alloc_perm(be, sizeof(BeVkRenderContext), 1);
}

static b32 be_vulkan_create_instance(BeEngine *be, BeVkRenderContext *rc, BeVulkanExtensions instance_extensions, BeArena *arena)
{
    VkApplicationInfo app_info = {0};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "be_vk_render";
    app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    app_info.pEngineName = "BLEEDING-EDGE";
    app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    app_info.apiVersion = BE_VULKAN_API_VERSION;

    VkInstanceCreateInfo instance_info = {0};
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;
    instance_info.enabledExtensionCount = instance_extensions.count;
    instance_info.ppEnabledExtensionNames = instance_extensions.data;
    VULKAN_CHECK(vkCreateInstance(&instance_info, NULL, &rc->instance.handle));

    return TRUE;
}

static b32 be_vulkan_create_surface(BeEngine *be, BeVkRenderContext *rc, BeArena *arena)
{
    BeLayer *window_layer = be_get_layer_by_name(be, "be_window");
    BeWindow *win_context = window_layer->context;

    usz st = 0;
    struct wm_vulkan_surface window_surface = {0};
    struct wm_vulkan_surface_info window_surface_info = {0};
    window_surface_info.instance = rc->instance.handle;
    window_surface_info.wm = &win_context->wm;
    window_surface_info.win = &win_context->main_window;
    st = wm_vulkan_surface_create(&window_surface, window_surface_info);
    if (st != WM_VULKAN_SURFACE_STATUS_SUCCESS) {
        ERROR_LOG("failed to create vulkan surface: "STR_FMT, wm_get_vulkan_surface_status_str(st));
        return FALSE;
    }

    rc->surface.handle = window_surface.handle;
    return TRUE;
}

enum device_extension_idx {
    DEV_EXT_SWAPCHAIN = 0,
    DEV_EXT_COUNT,
};

static b32 be_vulkan_find_suitable_device(BeEngine *be, BeVkRenderContext *rc, BeVulkanExtensions device_extensions, BeArena *arena)
{
    u32 count = 0;
    BeVulkanPhysicalDevices devices = {0};
    VULKAN_CHECK(vkEnumeratePhysicalDevices(rc->instance.handle, &count, NULL));
    void *buff = be_arena_alloc(arena, sizeof(*devices.data),count);
    mm_array_init_ext_static(&devices, buff, count);
    VULKAN_CHECK(vkEnumeratePhysicalDevices(rc->instance.handle, &count, devices.data));

    b32 device_found = FALSE;
    b32 format_supported = FALSE;
    for (usz i = 0; i < devices.count; ++i) {
        VkPhysicalDevice device = devices.data[i];
        VkPhysicalDeviceProperties properties = {0};
        vkGetPhysicalDeviceProperties(device, &properties);
        if (properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
                && properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
            continue;
        }

        if (!required_device_extensions_present(device, device_extensions, arena)) {
            continue;
        }

        device_found = TRUE;

        VkFormat format = 0;
        if (device_supports_format(device, rc->surface, &format, arena)) {
            format_supported = TRUE;
            rc->physical_device.handle = device;
            rc->physical_device.swapchain_format = format;
            break;
        }
    }

    return format_supported && device_found;
}

static b32 be_vulkan_find_graphics_queue(BeEngine *be, BeVkRenderContext *rc, BeArena *arena)
{
    u32 queue_family_count = 0;
    BeVulkanQueueFamilyProperties queue_properties = {0};
    vkGetPhysicalDeviceQueueFamilyProperties2(rc->physical_device.handle, &queue_family_count, NULL);
    void *buff = be_arena_alloc(arena, sizeof(*queue_properties.data), queue_family_count);
    mm_array_init_ext_static(&queue_properties, buff, queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties2(rc->physical_device.handle, &queue_family_count, queue_properties.data);

    b32 found = FALSE;
    for (usz queue_idx = 0; queue_idx < queue_properties.count; ++queue_idx) {
        VkBool32 present_support = VK_FALSE;
        VkQueueFamilyProperties2 property = {0};
        property = queue_properties.data[queue_idx];
        vkGetPhysicalDeviceSurfaceSupportKHR(rc->physical_device.handle, queue_idx, rc->surface.handle, &present_support);
        if (property.queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT
                && present_support) {
            found = TRUE;
            rc->graphics_queue.index = queue_idx;
        }
    }

    return found;
}

enum gfx_queue_indeces {
    GFX_QUEUE_MAIN_INDEX = 0,
    GFX_QUEUE_COUNT
};

static b32 be_vulkan_create_device(BeEngine *be, BeVkRenderContext *rc, BeVulkanExtensions device_extensions, BeArena *arena)
{
    UNUSED(arena);

    VkPhysicalDeviceDescriptorIndexingFeatures supported_indexing_features = {0};
    supported_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    supported_indexing_features.pNext = NULL;

    VkPhysicalDeviceVulkan14Features supported_features_1_4 = {0};
    supported_features_1_4.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;
    supported_features_1_4.pNext = &supported_indexing_features;

    VkPhysicalDeviceVulkan13Features supported_features_1_3 = {0};
    supported_features_1_3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    supported_features_1_3.pNext = &supported_features_1_4;

    VkPhysicalDeviceVulkan12Features supported_features_1_2 = {0};
    supported_features_1_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    supported_features_1_2.pNext = &supported_features_1_3;

    VkPhysicalDeviceFeatures2 supported_features_2 = {0};
    supported_features_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    supported_features_2.pNext = &supported_features_1_2;
    vkGetPhysicalDeviceFeatures2(rc->physical_device.handle, &supported_features_2);

    if (!supported_indexing_features.descriptorBindingPartiallyBound
            || !supported_indexing_features.runtimeDescriptorArray
            || !supported_features_1_3.dynamicRendering
            || !supported_features_1_3.synchronization2
            || !supported_features_1_2.timelineSemaphore) {
        ERROR_LOG("Selected Device doesn't meet feature requirements");
        return FALSE;
    }

    VkPhysicalDeviceDescriptorIndexingFeatures indexing_features = {0};
    indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    indexing_features.pNext = NULL;
    indexing_features.descriptorBindingPartiallyBound = VK_TRUE;
    indexing_features.runtimeDescriptorArray = VK_TRUE;

    VkPhysicalDeviceVulkan14Features features_1_4 = {0};
    features_1_4.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;
    features_1_4.pNext = &indexing_features;

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

    VkDeviceQueueCreateInfo graphics_queue_infos[GFX_QUEUE_COUNT] = {0};
    graphics_queue_infos[GFX_QUEUE_MAIN_INDEX].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    graphics_queue_infos[GFX_QUEUE_MAIN_INDEX].queueFamilyIndex = ASSERT_U32(rc->graphics_queue.index);
    graphics_queue_infos[GFX_QUEUE_MAIN_INDEX].queueCount = ARRAY_SIZE(queue_priorities);
    graphics_queue_infos[GFX_QUEUE_MAIN_INDEX].pQueuePriorities = queue_priorities;

    VkDeviceCreateInfo device_info = {0};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.pNext = &features_2;
    device_info.queueCreateInfoCount = ARRAY_SIZE(graphics_queue_infos);
    device_info.pQueueCreateInfos = &graphics_queue_infos[GFX_QUEUE_MAIN_INDEX];
    device_info.enabledExtensionCount = ASSERT_U32(device_extensions.count);
    device_info.ppEnabledExtensionNames = device_extensions.data;
    VULKAN_CHECK(vkCreateDevice(rc->physical_device.handle, &device_info, NULL, &rc->logical_device.handle));

    vkGetDeviceQueue(rc->logical_device.handle, rc->graphics_queue.index, GFX_QUEUE_MAIN_INDEX, &rc->graphics_queue.handle);

    return TRUE;
}

static b32 be_vulkan_create_swapchain(BeEngine *be, BeVkRenderContext *rc, BeArena *arena)
{
    BeLayer *window_settings = be_get_layer_by_name(be, "window_settings");
    struct wm_window_info *main_window_info = window_settings->context;

    VkSurfaceCapabilitiesKHR surface_capabilities = {0};
    VULKAN_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(rc->physical_device.handle, rc->surface.handle, &surface_capabilities));

    /* determine swapchain length */
    u32 min_image_count = CORE_MAX(BE_VULKAN_SWAPCHAIN_IMAGE_COUNT, surface_capabilities.minImageCount);
    if (surface_capabilities.maxImageCount) {
        min_image_count = CORE_MIN(min_image_count, surface_capabilities.maxImageCount);
    }

    VkExtent2D image_extent = {0};
    image_extent.width = ASSERT_U32(main_window_info->width);
    image_extent.height = ASSERT_U32(main_window_info->height);

    VkSwapchainCreateInfoKHR swapchain_info = {0};
    swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_info.surface = rc->surface.handle;
    swapchain_info.minImageCount = min_image_count;
    swapchain_info.imageFormat = BE_VULKAN_SWAPCHAIN_FORMAT;
    swapchain_info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    swapchain_info.imageExtent = image_extent;
    swapchain_info.imageArrayLayers = 1;
    swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_info.preTransform = surface_capabilities.currentTransform;
    swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    VULKAN_CHECK(vkCreateSwapchainKHR(rc->logical_device.handle, &swapchain_info, NULL, &rc->swapchain.handle));

    /* create swapchain image buffer */
    u32 image_count = 0;
    vkGetSwapchainImagesKHR(rc->logical_device.handle, rc->swapchain.handle, &image_count, NULL);
    void *image_buff = be_arena_alloc(arena, sizeof(*rc->swapchain.images.data), image_count);
    mm_array_init_ext_static(&rc->swapchain.images, image_buff, image_count);
    vkGetSwapchainImagesKHR(rc->logical_device.handle, rc->swapchain.handle, &image_count, rc->swapchain.images.data);

    /* create image view buffer */
    void *image_view_buff = be_arena_alloc(arena, sizeof(*rc->swapchain.image_views.data), image_count);
    mm_array_init_ext_static(&rc->swapchain.image_views, image_view_buff, image_count);

    /* initialize image views */
    VkImageSubresourceRange range = {0};
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.levelCount = 1;
    range.layerCount = 1;
    for (usz i = 0; i < rc->swapchain.image_views.count; ++i) {
        VkImageViewCreateInfo image_view_info = {0};
        image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_info.image = rc->swapchain.images.data[i];
        image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_info.format = BE_VULKAN_SWAPCHAIN_FORMAT;
        image_view_info.subresourceRange = range;
        VULKAN_CHECK(vkCreateImageView(rc->logical_device.handle, &image_view_info, NULL, &rc->swapchain.image_views.data[i]));
    }

    /* create semaphore buffer */
    void *semaphore_buff = be_arena_alloc(arena, sizeof(*rc->swapchain.render_complete_semaphores.data), image_count);
    mm_array_init_ext(&rc->swapchain.render_complete_semaphores, semaphore_buff, image_count);
    mm_array_reserve(&rc->swapchain.render_complete_semaphores, image_count);

    /* initialize semaphores */
    for (usz i = 0; i < rc->swapchain.render_complete_semaphores.count; ++i) {
        VkSemaphoreCreateInfo semaphore_create_info = {0};
        semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VULKAN_CHECK(vkCreateSemaphore(rc->logical_device.handle, &semaphore_create_info, NULL, &rc->swapchain.render_complete_semaphores.data[i]));
    }

    VkExtent3D depth_extent = {0};
    depth_extent.width = ASSERT_U32(main_window_info->width);
    depth_extent.height = ASSERT_U32(main_window_info->height);
    depth_extent.depth = 1;

    /* create depth image */
    VkImageCreateInfo depth_info = {0};
    depth_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    depth_info.imageType = VK_IMAGE_TYPE_2D;
    depth_info.format = BE_VULKAN_DEPTH_FORMAT;
    depth_info.extent = depth_extent;
    depth_info.mipLevels = 1;
    depth_info.arrayLayers = 1;
    depth_info.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    depth_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    depth_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    UNUSED(depth_info);

    return TRUE;
}

static b32 device_supports_format(VkPhysicalDevice device, BeVulkanSurface surface, VkFormat *format, BeArena *arena)
{
    u32 count = 0;
    BeVulkanSurfaceFormats surface_formats = {0};
    VULKAN_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.handle, &count, NULL));
    void *surface_format_buff = be_arena_alloc(arena, sizeof(*surface_formats.data), count);
    mm_array_init_ext_static(&surface_formats, surface_format_buff, count);
    VULKAN_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.handle, &count, surface_formats.data));

    for (usz i = 0; i < surface_formats.count; ++i) {
        VkSurfaceFormatKHR surface_format = surface_formats.data[i];
        INFO_LOG("format: "USZ_FMT, surface_format.format);
        if (surface_format.format == BE_VULKAN_SWAPCHAIN_FORMAT) {
            *format = surface_format.format;
            return TRUE;
        }
    }

    return FALSE;
}

static void be_vulkan_get_required_instance_extensions(BeVulkanExtensions *extensions, BeArena *arena)
{
    static const char *instance_extension_names[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
    };
    struct wm_vulkan_extensions wm_extensions = {0};
    wm_vulkan_extensions_get_required(&wm_extensions);

    usz count = ARRAY_SIZE(instance_extension_names) + wm_extensions.count;
    void *buff = be_arena_alloc(arena, sizeof(const char *), count);

    mm_array_init_ext(extensions, buff, count);
    mm_array_append_buff(extensions, instance_extension_names, ARRAY_SIZE(instance_extension_names));
    mm_array_append_buff(extensions, wm_extensions.names, wm_extensions.count);;
}

static void be_vulkan_get_required_device_extensions(BeVulkanExtensions *extensions, BeArena *arena)
{
    static const char *device_extension_names[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
    usz count = ARRAY_SIZE(device_extension_names);
    mm_array_init_ext_static(extensions, device_extension_names, count);
}

static b32 required_device_extensions_present(VkPhysicalDevice device, BeVulkanExtensions required, BeArena *arena)
{
    u32 count = 0;
    BeVulkanExtensionProperties present = {0};
    VULKAN_CHECK(vkEnumerateDeviceExtensionProperties(device, NULL, &count, NULL));
    void *buff = be_arena_alloc(arena, sizeof(*present.data), count);
    mm_array_init_ext_static(&present, buff, count);
    VULKAN_CHECK(vkEnumerateDeviceExtensionProperties(device, NULL, &count, present.data));

    for (usz i = 0; i < required.count; ++i) {
        b32 current_present = FALSE;
        for (usz j = 0; j < present.count; ++j) {
            if (cstr_compare(required.data[i], present.data[j].extensionName)) {
                current_present = TRUE;
            }
        }

        if (!current_present)
            return FALSE;
    }

    return TRUE;
}
