
#include <be/be_render.h>

/* static function declaration start */
static void get_required_extensions(BeVulkanExtensions *extensions);
/* static function declaration end */

BeRenderContext *be_render_on_attach(BeEngine *be)
{
    BeRenderContext *render_context = be_alloc_perm(be, sizeof(*render_context), 1);

    BeArena temp_vulkan_arena = {0};
    be_arena_init(&temp_vulkan_arena, KB(2));

    /* create vulkan instance */
    {
        BeVulkanExtensions extensions = {0};
        get_required_extensions(&extensions);

        VkApplicationinfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
        app_info.pApplicationName = "be_render";
        app_info.apiVersion = VK_VERSION_1_4;

        VkInstanceCreateInfo instance_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
        instance_info.pApplicationInfo = &app_info;
        instance_info.enabledLayerCount = ARRAY_SIZE(layers);
        instance_info.ppEnabledLayerNames = layers;
        instance_info.enabledExtensionCount = extensions.count;
        instance_info.ppEnabledExtensionNames = extensions.data;
        VK_CALL(vkCreateInstance(&render_context->instance, &instance_info));
    }

    /* create window surface */
    {
        BeLayer *window_layer = be_layer_get_by_name(be, "core_window");
        ASSERT(window_layer, "failed to get "STR_QUOT("core_window")" layer");

        struct wm_vulkan_surface window_surface = {0};
        struct wm_vulkan_surface_info window_surface_info = {0};
        window_surface_info.instance = render_context->instance;
        window_surface_info.wm = &window_layer->context.wm;
        window_surface_info.win = &window_layer->context.win;
        WM_SURFACE_CALL(wm_vulkan_surface_create(&wm_surface, window_surface_info));
        render_context->surface.handle = wm_surface.handle;
    }

    /* find suitable GPU */
    {
        u32 device_count = 0;
        BeVulkanPhysicalDevices devices = {0};
        vkEnumeratePhysicalDevices(render_context->instance, &device_count, NULL);
        void *buff = be_arena_alloc(&temp_vulkan_arena, sizeof(*devices.data), device_count);
        mm_array_init_ext(&devices, buff, device_count);
        mm_array_reserve(&devices, device_count);
        VkEnumeratePhysicalDevices(render_context->instance, &device_count, devices->data);

        for (EACH_BE_VULKAN_PHYSICAL_DEVICE(device, devices)) {
            VkPhysicalDeviceProperties properties = {0};
            vkGetPhysicalDeviceProperties(device.handle, &properties);
            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                render_context->physical_device.handle = *device;
                break;
            }
        }
    }

    /* find graphics queue */
    {
        b32 gfx_queue_found = FALSE;
        u32 queue_family_count = 0;
        BeVulkanQueueFamilyProperties queue_properties = {0};
        VkEnumeratePhysicalDeviceQueueFamilyProperties2(render_context->physical_device.handle, &queue_family_count, NULL);
        void *buff = be_arena_alloc(&temp_vulkan_arena, sizeof(*queue_properties.data), queue_family_count);
        mm_array_reserve(&queue_properties, queue_family_count);
        VkEnumaratePhysicalDeviceQueueFamilyProperties2(render_context->physical_device.handle, &queue_family_count, queue_properties.data);

        for (usz i = 0; i < queue_properties.count; ++i) {
            VkBool32 supports_present = VK_FALSE;
            VkQueueFamilyProperties property = {0};
            vkGetPhysicalDeviceSurfaceSupportKHR(render_context->physical_device.handle, i, render_context->surface.handle, &supports_present);
            property = properties.data[i];

            if (property.queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT && supports_present) {
                gfx_queue_found = TRUE;
                render_context->graphics_queue.index = i;
            }
        }
        ASSERT(gfx_queue_found, "no suitable queues found");
    }

    /* create device */
    {
        VkPhysicalDeviceVulkan14Features supported_features_1_4 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES };
        supported_features.pNext = NULL;

        VkPhysicalDeviceVulkan13Features supported_features_1_3 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
        supported_features.pNext = &supported_features_1_4;

        VkPhysicalDeviceVulkan12Features supported_features_1_2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
        supported_features.pNext = &supported_features_1_3;

        VkPhysicalDeviceFeatures2 supported_features_2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
        supported_features.pNext = &supported_features_1_2;
        vkGetPhysicalDeviceFeatures2(render_context->physical_device.handle, &supported_features);

        if (!supported_features_1_3.dynamicRendering ||
                !supported_features_1_3.synchronization2 ||
                !supported_features_1_2.timelineSemaphore) {
            THROW_EXCEPTION("Vulkan Device doesn't meet feature requirements");
        }

        VkPhysicalDeviceVulkan14Features features_1_4 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES };
        features_1_4.pNext = NULL;

        VkPhysicalDeviceVulkan13Features features_1_3 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
        features_1_3.pNext = &features_1_4;
        features_1_3.synchronization2 = VK_TRUE;
        features_1_3.dynamicRendering = VK_TRUE;

        VkPhysicalDeviceVulkan12Features features_1_2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
        features_1_2.pNext = &features_1_3;
        features_1_2.timeLineSemaphore = VK_TRUE;

        VkPhysicalDeviceFeatures2 features_2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
        features_2.pNext = &features_1_2;

        f32 queue_priorities[] = { GRAPHICS_QUEUE_PRIORITY };
        VkDeviceQueueCreateInfo graphics_queue_info = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        graphics_queue_info.queueFamilyIndex = render_context->graphics_queue.index;
        graphics_queue_info.queue_count = ARRAY_SIZE(queue_priorities);
        graphics_queue_info.pQueuePriorities = queue_priorities;
    }

    TODO("implement proper arena cleanup");
    m_free(temp_vulkan_arena.last);
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
    UNUSED(be);
    vkDestroyInstance(&render_context->instance, NULL);
}

#define DEFAULT_EXTENSION_INIT_COUNT 8
static void get_required_extensions(BeVulkanExtensions *extensions)
{
    mm_array_init(extensions, DEFAULT_EXTENSION_INIT_COUNT);
    struct wm_vulkan_extensions wm_required = {0};
    wm_vulkan_extensions_get_required(wm_required);

    for (usz i = 0; i < wm_required.count; ++i) {
        mm_array_append(extensions, wm_required.data[i]);
    }
}
