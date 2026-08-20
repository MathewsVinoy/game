#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <optional>
#include <stdexcept>
#include <cstdlib>
#include <algorithm>
#include <limits>
#include <cstring>

using namespace std;

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif


//validation
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    cerr << "========== Vulkan Validation ==========\n";
    cerr << pCallbackData->pMessage << "\n";
    cerr << "=======================================\n";

    return VK_FALSE;
}

VkDebugUtilsMessengerCreateInfoEXT createDebugMessengerInfo()
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};

    createInfo.sType =
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    createInfo.pfnUserCallback =
        debugCallback;

    return createInfo;
}
VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func =
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(
                instance,
                "vkCreateDebugUtilsMessengerEXT"
            )
        );

    if (func != nullptr)
    {
        return func(
            instance,
            pCreateInfo,
            pAllocator,
            pDebugMessenger
        );
    }

    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator)
{
    auto func =
        reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(
                instance,
                "vkDestroyDebugUtilsMessengerEXT"
            )
        );

    if (func != nullptr)
    {
        func(
            instance,
            debugMessenger,
            pAllocator
        );
    }
}

// --------------------------------------------------
// Queue Family Indices
// --------------------------------------------------

struct QueueFamilyIndices
{
    optional<uint32_t> graphicsFamily;
    optional<uint32_t> presentFamily;

    bool isComplete() const
    {
        return graphicsFamily.has_value() &&
               presentFamily.has_value();
    }
};

// --------------------------------------------------
// Swapchain Support Details
// --------------------------------------------------

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;

    vector<VkSurfaceFormatKHR> formats;

    vector<VkPresentModeKHR> presentModes;
};

// --------------------------------------------------
// Find Queue Families
// --------------------------------------------------

QueueFamilyIndices findQueueFamilies(
    VkPhysicalDevice device,
    VkSurfaceKHR surface)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(
        device,
        &queueFamilyCount,
        nullptr
    );

    vector<VkQueueFamilyProperties> queueFamilies(
        queueFamilyCount
    );

    vkGetPhysicalDeviceQueueFamilyProperties(
        device,
        &queueFamilyCount,
        queueFamilies.data()
    );

    for (uint32_t i = 0; i < queueFamilies.size(); i++)
    {
        // Graphics support
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }

        // Presentation support
        VkBool32 presentSupport = VK_FALSE;

        vkGetPhysicalDeviceSurfaceSupportKHR(
            device,
            i,
            surface,
            &presentSupport
        );

        if (presentSupport)
        {
            indices.presentFamily = i;
        }

        if (indices.isComplete())
        {
            break;
        }
    }

    return indices;
}

// --------------------------------------------------
// Check Device Extension Support
// --------------------------------------------------

bool checkDeviceExtensionSupport(
    VkPhysicalDevice device)
{
    uint32_t extensionCount = 0;

    vkEnumerateDeviceExtensionProperties(
        device,
        nullptr,
        &extensionCount,
        nullptr
    );

    vector<VkExtensionProperties> availableExtensions(
        extensionCount
    );

    vkEnumerateDeviceExtensionProperties(
        device,
        nullptr,
        &extensionCount,
        availableExtensions.data()
    );

    bool swapchainFound = false;

    for (const auto& extension : availableExtensions)
    {
        if (strcmp(
                extension.extensionName,
                VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
        {
            swapchainFound = true;
            break;
        }
    }

    return swapchainFound;
}

// --------------------------------------------------
// Query Swapchain Support
// --------------------------------------------------

SwapChainSupportDetails querySwapChainSupport(
    VkPhysicalDevice device,
    VkSurfaceKHR surface)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        device,
        surface,
        &details.capabilities
    );

    uint32_t formatCount = 0;

    vkGetPhysicalDeviceSurfaceFormatsKHR(
        device,
        surface,
        &formatCount,
        nullptr
    );

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);

        vkGetPhysicalDeviceSurfaceFormatsKHR(
            device,
            surface,
            &formatCount,
            details.formats.data()
        );
    }

    uint32_t presentModeCount = 0;

    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device,
        surface,
        &presentModeCount,
        nullptr
    );

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);

        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device,
            surface,
            &presentModeCount,
            details.presentModes.data()
        );
    }

    return details;
}

// --------------------------------------------------
// Check If GPU Is Suitable
// --------------------------------------------------

bool isDeviceSuitable(
    VkPhysicalDevice device,
    VkSurfaceKHR surface)
{
    QueueFamilyIndices indices =
        findQueueFamilies(device, surface);

    bool extensionsSupported =
        checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;

    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport =
            querySwapChainSupport(device, surface);

        swapChainAdequate =
            !swapChainSupport.formats.empty() &&
            !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() &&
           extensionsSupported &&
           swapChainAdequate;
}

// --------------------------------------------------
// Choose Surface Format
// --------------------------------------------------

VkSurfaceFormatKHR chooseSwapSurfaceFormat(
    const vector<VkSurfaceFormatKHR>& formats)
{
    for (const auto& format : formats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
            format.colorSpace ==
                VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return format;
        }
    }

    return formats[0];
}

// --------------------------------------------------
// Choose Present Mode
// --------------------------------------------------

VkPresentModeKHR chooseSwapPresentMode(
    const vector<VkPresentModeKHR>& presentModes)
{
    // Prefer mailbox if available
    for (const auto& mode : presentModes)
    {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return mode;
        }
    }

    // Guaranteed to exist
    return VK_PRESENT_MODE_FIFO_KHR;
}

// --------------------------------------------------
// Choose Swapchain Extent
// --------------------------------------------------

VkExtent2D chooseSwapExtent(
    const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width !=
            numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }

    VkExtent2D actualExtent =
    {
        WIDTH,
        HEIGHT
    };

    actualExtent.width =
        max(
            capabilities.minImageExtent.width,
            min(
                capabilities.maxImageExtent.width,
                actualExtent.width
            )
        );

    actualExtent.height =
        max(
            capabilities.minImageExtent.height,
            min(
                capabilities.maxImageExtent.height,
                actualExtent.height
            )
        );

    return actualExtent;
}

// --------------------------------------------------
// 5. Create Vulkan Instance
// --------------------------------------------------

VkInstance createInstance(GLFWwindow* window,uint32_t glfwExtensionCount,const char** glfwExtensions){
    // --------------------------------------------------
    // 3. Create Vulkan Application Info
    // --------------------------------------------------
    VkApplicationInfo appInfo{};
    appInfo.sType =
        VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName =
        "Game Engine";
    appInfo.applicationVersion =
        VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName =
        "Engine";
    appInfo.engineVersion =
        VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion =
        VK_API_VERSION_1_0;
    
        
    //need to make above to differnt function
    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType =
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo =
        &appInfo;

    const char* validationLayers[] = {
        "VK_LAYER_KHRONOS_validation"
    };

    instanceCreateInfo.enabledLayerCount = 1;
    instanceCreateInfo.ppEnabledLayerNames = validationLayers;

    vector<const char*> extensions(
        glfwExtensions,
        glfwExtensions + glfwExtensionCount
    );
    if (enableValidationLayers)
    {
        extensions.push_back(
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME
        );
    }
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
    VkInstance instance = VK_NULL_HANDLE;
    if (vkCreateInstance(
            &instanceCreateInfo,
            nullptr,
            &instance) != VK_SUCCESS)
    {
        cerr << "Failed to create Vulkan instance\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return VK_NULL_HANDLE;
    }
    cout << "Vulkan instance created!\n";
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    if (enableValidationLayers)
    {
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = createDebugMessengerInfo();
        if (CreateDebugUtilsMessengerEXT(
                instance,
                &debugCreateInfo,
                nullptr,
                &debugMessenger
            ) != VK_SUCCESS)
        {
            cerr << "Failed to create debug messenger!\n";
            vkDestroyInstance(
                instance,
                nullptr
            );
            glfwDestroyWindow(window);
            glfwTerminate();
            return VK_NULL_HANDLE;
        }
    }
    cout << "Debug messenger created!\n";

    return instance;
}



// --------------------------------------------------
// Main
// --------------------------------------------------



int main()
{
    // --------------------------------------------------
    // 1. Initialize GLFW
    // --------------------------------------------------

    if (!glfwInit())
    {
        cerr << "Failed to initialize GLFW\n";
        return EXIT_FAILURE;
    }

    // Tell GLFW that we will use Vulkan
    glfwWindowHint(
        GLFW_CLIENT_API,
        GLFW_NO_API
    );

    // --------------------------------------------------
    // 2. Create Window
    // --------------------------------------------------

    GLFWwindow* window =
        glfwCreateWindow(
            WIDTH,
            HEIGHT,
            "Vulkan Window",
            nullptr,
            nullptr
        );

    if (!window)
    {
        cerr << "Failed to create GLFW window\n";

        glfwTerminate();

        return EXIT_FAILURE;
    }

    cout << "GLFW window created!\n";


    // --------------------------------------------------
    // 4. Get GLFW Vulkan Extensions
    // --------------------------------------------------

    uint32_t glfwExtensionCount = 0;

    const char** glfwExtensions =
        glfwGetRequiredInstanceExtensions(
            &glfwExtensionCount
        );

    if (glfwExtensions == nullptr)
    {
        cerr << "GLFW could not find required Vulkan extensions!\n";

        glfwDestroyWindow(window);
        glfwTerminate();

        return EXIT_FAILURE;
    }

    // --------------------------------------------------
    // 5. Create Vulkan Instance
    // --------------------------------------------------
    VkInstance instance = createInstance(window, glfwExtensionCount, glfwExtensions);
    if (instance == VK_NULL_HANDLE)
    {
        cerr << "Failed to create Vulkan instance!\n";

        glfwDestroyWindow(window);
        glfwTerminate();

        return EXIT_FAILURE;
    }

    // --------------------------------------------------
    // 6. Create Window Surface
    // --------------------------------------------------

    VkSurfaceKHR surface = VK_NULL_HANDLE;

    if (glfwCreateWindowSurface(
            instance,
            window,
            nullptr,
            &surface) != VK_SUCCESS)
    {
        cerr << "Failed to create window surface!\n";

        vkDestroyInstance(
            instance,
            nullptr
        );

        glfwDestroyWindow(window);
        glfwTerminate();

        return EXIT_FAILURE;
    }

    cout << "Vulkan surface created!\n";

    // --------------------------------------------------
    // 7. Find Physical Devices
    // --------------------------------------------------

    uint32_t deviceCount = 0;

    vkEnumeratePhysicalDevices(
        instance,
        &deviceCount,
        nullptr
    );

    cout << "Number of GPUs: "
         << deviceCount
         << "\n";

    if (deviceCount == 0)
    {
        cerr << "No Vulkan GPU found!\n";

        vkDestroySurfaceKHR(
            instance,
            surface,
            nullptr
        );

        vkDestroyInstance(
            instance,
            nullptr
        );

        glfwDestroyWindow(window);
        glfwTerminate();

        return EXIT_FAILURE;
    }

    vector<VkPhysicalDevice> devices(
        deviceCount
    );

    vkEnumeratePhysicalDevices(
        instance,
        &deviceCount,
        devices.data()
    );

    // --------------------------------------------------
    // 8. Select Physical Device
    // --------------------------------------------------

    VkPhysicalDevice physicalDevice =
        VK_NULL_HANDLE;

    // First prefer a suitable discrete GPU
    for (auto gpu : devices)
    {
        VkPhysicalDeviceProperties properties{};

        vkGetPhysicalDeviceProperties(
            gpu,
            &properties
        );

        cout << "Found GPU: "
             << properties.deviceName
             << "\n";

        if (properties.deviceType ==
                VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
            isDeviceSuitable(gpu, surface))
        {
            physicalDevice = gpu;
            break;
        }
    }

    // If no discrete GPU, find any suitable GPU
    if (physicalDevice == VK_NULL_HANDLE)
    {
        for (auto gpu : devices)
        {
            if (isDeviceSuitable(gpu, surface))
            {
                physicalDevice = gpu;
                break;
            }
        }
    }

    if (physicalDevice == VK_NULL_HANDLE)
    {
        cerr << "Failed to find a suitable GPU!\n";

        vkDestroySurfaceKHR(
            instance,
            surface,
            nullptr
        );

        vkDestroyInstance(
            instance,
            nullptr
        );

        glfwDestroyWindow(window);
        glfwTerminate();

        return EXIT_FAILURE;
    }

    VkPhysicalDeviceProperties gpuProperties{};

    vkGetPhysicalDeviceProperties(
        physicalDevice,
        &gpuProperties
    );

    cout << "Selected GPU: "
         << gpuProperties.deviceName
         << "\n";

    // --------------------------------------------------
    // 9. Find Queue Families
    // --------------------------------------------------

    QueueFamilyIndices indices =
        findQueueFamilies(
            physicalDevice,
            surface
        );

    cout << "Graphics queue family: "
         << indices.graphicsFamily.value()
         << "\n";

    cout << "Present queue family: "
         << indices.presentFamily.value()
         << "\n";

    // --------------------------------------------------
    // 10. Create Queue Information
    // --------------------------------------------------

    vector<VkDeviceQueueCreateInfo>
        queueCreateInfos;

    vector<uint32_t> uniqueQueueFamilies;

    uniqueQueueFamilies.push_back(
        indices.graphicsFamily.value()
    );

    if (indices.presentFamily.value() !=
        indices.graphicsFamily.value())
    {
        uniqueQueueFamilies.push_back(
            indices.presentFamily.value()
        );
    }

    float queuePriority = 1.0f;

    for (uint32_t queueFamily :
         uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueInfo{};

        queueInfo.sType =
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

        queueInfo.queueFamilyIndex =
            queueFamily;

        queueInfo.queueCount = 1;

        queueInfo.pQueuePriorities =
            &queuePriority;

        queueCreateInfos.push_back(
            queueInfo
        );
    }

    // --------------------------------------------------
    // 11. Device Extensions
    // --------------------------------------------------

    const char* deviceExtensions[] =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    // --------------------------------------------------
    // 12. Create Logical Device
    // --------------------------------------------------

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo deviceCreateInfo{};

    deviceCreateInfo.sType =
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    deviceCreateInfo.queueCreateInfoCount =
        static_cast<uint32_t>(
            queueCreateInfos.size()
        );

    deviceCreateInfo.pQueueCreateInfos =
        queueCreateInfos.data();

    deviceCreateInfo.enabledExtensionCount = 1;

    deviceCreateInfo.ppEnabledExtensionNames =
        deviceExtensions;

    deviceCreateInfo.pEnabledFeatures =
        &deviceFeatures;

    VkDevice device = VK_NULL_HANDLE;

    if (vkCreateDevice(
            physicalDevice,
            &deviceCreateInfo,
            nullptr,
            &device) != VK_SUCCESS)
    {
        cerr << "Failed to create logical device!\n";

        vkDestroySurfaceKHR(
            instance,
            surface,
            nullptr
        );

        vkDestroyInstance(
            instance,
            nullptr
        );

        glfwDestroyWindow(window);
        glfwTerminate();

        return EXIT_FAILURE;
    }

    cout << "Logical device created!\n";

    // --------------------------------------------------
    // 13. Get Graphics Queue
    // --------------------------------------------------

    VkQueue graphicsQueue =
        VK_NULL_HANDLE;

    vkGetDeviceQueue(
        device,
        indices.graphicsFamily.value(),
        0,
        &graphicsQueue
    );

    cout << "Graphics queue obtained!\n";

    // --------------------------------------------------
    // 14. Get Present Queue
    // --------------------------------------------------

    VkQueue presentQueue =
        VK_NULL_HANDLE;

    vkGetDeviceQueue(
        device,
        indices.presentFamily.value(),
        0,
        &presentQueue
    );

    cout << "Present queue obtained!\n";

    // --------------------------------------------------
    // 15. Query Swapchain Support
    // --------------------------------------------------

    SwapChainSupportDetails swapChainSupport =
        querySwapChainSupport(
            physicalDevice,
            surface
        );

    // --------------------------------------------------
    // 16. Choose Swapchain Settings
    // --------------------------------------------------

    VkSurfaceFormatKHR surfaceFormat =
        chooseSwapSurfaceFormat(
            swapChainSupport.formats
        );

    VkPresentModeKHR presentMode =
        chooseSwapPresentMode(
            swapChainSupport.presentModes
        );

    VkExtent2D extent =
        chooseSwapExtent(
            swapChainSupport.capabilities
        );

    cout << "Swapchain width: "
         << extent.width
         << "\n";

    cout << "Swapchain height: "
         << extent.height
         << "\n";

    // --------------------------------------------------
    // 17. Choose Number of Swapchain Images
    // --------------------------------------------------

    uint32_t imageCount =
        swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount >
            swapChainSupport.capabilities.maxImageCount)
    {
        imageCount =
            swapChainSupport.capabilities.maxImageCount;
    }

    // --------------------------------------------------
    // 18. Create Swapchain
    // --------------------------------------------------

    VkSwapchainCreateInfoKHR swapChainCreateInfo{};

    swapChainCreateInfo.sType =
        VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

    swapChainCreateInfo.surface =
        surface;

    swapChainCreateInfo.minImageCount =
        imageCount;

    swapChainCreateInfo.imageFormat =
        surfaceFormat.format;

    swapChainCreateInfo.imageColorSpace =
        surfaceFormat.colorSpace;

    swapChainCreateInfo.imageExtent =
        extent;

    swapChainCreateInfo.imageArrayLayers = 1;

    swapChainCreateInfo.imageUsage =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] =
    {
        indices.graphicsFamily.value(),
        indices.presentFamily.value()
    };

    if (indices.graphicsFamily !=
        indices.presentFamily)
    {
        swapChainCreateInfo.imageSharingMode =
            VK_SHARING_MODE_CONCURRENT;

        swapChainCreateInfo.queueFamilyIndexCount =
            2;

        swapChainCreateInfo.pQueueFamilyIndices =
            queueFamilyIndices;
    }
    else
    {
        swapChainCreateInfo.imageSharingMode =
            VK_SHARING_MODE_EXCLUSIVE;

        swapChainCreateInfo.queueFamilyIndexCount = 0;

        swapChainCreateInfo.pQueueFamilyIndices =
            nullptr;
    }

    swapChainCreateInfo.preTransform =
        swapChainSupport.capabilities.currentTransform;

    swapChainCreateInfo.compositeAlpha =
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    swapChainCreateInfo.presentMode =
        presentMode;

    swapChainCreateInfo.clipped =
        VK_TRUE;

    swapChainCreateInfo.oldSwapchain =
        VK_NULL_HANDLE;

    VkSwapchainKHR swapChain =
        VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(
            device,
            &swapChainCreateInfo,
            nullptr,
            &swapChain) != VK_SUCCESS)
    {
        cerr << "Failed to create swapchain!\n";

        vkDestroyDevice(
            device,
            nullptr
        );

        vkDestroySurfaceKHR(
            instance,
            surface,
            nullptr
        );

        vkDestroyInstance(
            instance,
            nullptr
        );

        glfwDestroyWindow(window);
        glfwTerminate();

        return EXIT_FAILURE;
    }

    cout << "Swapchain created!\n";

    // --------------------------------------------------
    // 19. Get Swapchain Images
    // --------------------------------------------------

    vector<VkImage> swapChainImages;

    uint32_t swapChainImageCount = 0;

    vkGetSwapchainImagesKHR(
        device,
        swapChain,
        &swapChainImageCount,
        nullptr
    );

    swapChainImages.resize(
        swapChainImageCount
    );

    vkGetSwapchainImagesKHR(
        device,
        swapChain,
        &swapChainImageCount,
        swapChainImages.data()
    );

    cout << "Swapchain images: "
         << swapChainImages.size()
         << "\n";

    // --------------------------------------------------
    // 20. Create Image Views
    // --------------------------------------------------

    vector<VkImageView> swapChainImageViews;

    swapChainImageViews.resize(
        swapChainImages.size()
    );

    for (size_t i = 0;
         i < swapChainImages.size();
         i++)
    {
        VkImageViewCreateInfo viewInfo{};

        viewInfo.sType =
            VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

        viewInfo.image =
            swapChainImages[i];

        viewInfo.viewType =
            VK_IMAGE_VIEW_TYPE_2D;

        viewInfo.format =
            surfaceFormat.format;

        viewInfo.components.r =
            VK_COMPONENT_SWIZZLE_IDENTITY;

        viewInfo.components.g =
            VK_COMPONENT_SWIZZLE_IDENTITY;

        viewInfo.components.b =
            VK_COMPONENT_SWIZZLE_IDENTITY;

        viewInfo.components.a =
            VK_COMPONENT_SWIZZLE_IDENTITY;

        viewInfo.subresourceRange.aspectMask =
            VK_IMAGE_ASPECT_COLOR_BIT;

        viewInfo.subresourceRange.baseMipLevel = 0;

        viewInfo.subresourceRange.levelCount = 1;

        viewInfo.subresourceRange.baseArrayLayer = 0;

        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(
                device,
                &viewInfo,
                nullptr,
                &swapChainImageViews[i])
            != VK_SUCCESS)
        {
            cerr << "Failed to create image view!\n";

            for (auto imageView :
                 swapChainImageViews)
            {
                if (imageView != VK_NULL_HANDLE)
                {
                    vkDestroyImageView(
                        device,
                        imageView,
                        nullptr
                    );
                }
            }

            vkDestroySwapchainKHR(
                device,
                swapChain,
                nullptr
            );

            vkDestroyDevice(
                device,
                nullptr
            );

            vkDestroySurfaceKHR(
                instance,
                surface,
                nullptr
            );

            vkDestroyInstance(
                instance,
                nullptr
            );

            glfwDestroyWindow(window);
            glfwTerminate();

            return EXIT_FAILURE;
        }
    }

    cout << "Swapchain image views created!\n";

    // --------------------------------------------------
    // 21. Main Loop
    // --------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        
    }

    // --------------------------------------------------
    // 22. Wait for GPU
    // --------------------------------------------------

    vkDeviceWaitIdle(device);

    // --------------------------------------------------
    // 23. Cleanup Image Views
    // --------------------------------------------------

    for (auto imageView :
         swapChainImageViews)
    {
        vkDestroyImageView(
            device,
            imageView,
            nullptr
        );
    }

    // --------------------------------------------------
    // 24. Cleanup Swapchain
    // --------------------------------------------------

    vkDestroySwapchainKHR(
        device,
        swapChain,
        nullptr
    );

    // --------------------------------------------------
    // 25. Cleanup Logical Device
    // --------------------------------------------------

    vkDestroyDevice(
        device,
        nullptr
    );

    // --------------------------------------------------
    // 26. Cleanup Surface
    // --------------------------------------------------

    vkDestroySurfaceKHR(
        instance,
        surface,
        nullptr
    );

    // --------------------------------------------------
    // 27. Cleanup Vulkan Instance
    // --------------------------------------------------

    vkDestroyInstance(
        instance,
        nullptr
    );

    // --------------------------------------------------
    // 28. Cleanup GLFW
    // --------------------------------------------------

    glfwDestroyWindow(window);

    glfwTerminate();

    cout << "Vulkan shutdown complete!\n";

    return EXIT_SUCCESS;
}