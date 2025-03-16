#include <myMalloc.h>
#include <string.h>
#include <stdio.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "MY_ASSERT.h"

#ifdef NDEBUG
static const bool enableValidationLayers = false;
#else
static const bool enableValidationLayers = true;
#endif

static const char *const validationLayers[] = {
    "VK_LAYER_KHRONOS_validation"
};
static const uint32_t validationLayersCount = sizeof(validationLayers) / sizeof(const char *);

[[maybe_unused]]
static bool checkValidationLayerSupport(void) {
    bool layerFound = true;

    uint32_t layerCount = 0; {
        vkEnumerateInstanceLayerProperties(&layerCount, NULL);
    }
    VkLayerProperties avaibleLayers[layerCount];

    uint32_t i = 0;
    uint32_t j = 0;

    vkEnumerateInstanceLayerProperties(&layerCount, avaibleLayers);

    while (i < validationLayersCount && layerFound == true) {
        layerFound = false;

        j = 0;
        while (j < layerCount && layerFound == false) {
            if (strcmp(validationLayers[i], avaibleLayers[j].layerName) == 0) {
                layerFound = true;
            }

            j += 1;
        }

        i += 1;
    }

    return layerFound;
}

[[maybe_unused]]
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, [[maybe_unused]] void *pUserData) {
    fprintf(stderr, "VL ");

    switch ((int)messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            fprintf(stderr, "VERBOSE: ");
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            fprintf(stderr, "INFO: ");
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            fprintf(stderr, "WARNING: ");
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            fprintf(stderr, "ERROR: ");
            break;
    }

    fprintf(stderr, "%s\n", pCallbackData->pMessage);

    return VK_FALSE;
}

[[maybe_unused]]
static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger) {
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    return func != NULL ? func(instance, pCreateInfo, pAllocator, pDebugMessenger) :
                          VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    if (func != NULL && enableValidationLayers) {
        func(instance, debugMessenger, pAllocator);
    }
}

static const char **getRequiredExtensions(uint32_t *extensionCount) {
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(extensionCount);

    uint32_t newSize = *extensionCount + (enableValidationLayers ? 2 : 1);
    
    const char **extensions = malloc(sizeof(const char *) * newSize);
    memcpy(extensions, glfwExtensions, sizeof(const char *) * *extensionCount);
    extensions[*extensionCount] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME; // macos Hell
    
    if (enableValidationLayers) {
        extensions[*extensionCount + 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME; // not sure how it works
    }
    
    *extensionCount = newSize;

    return extensions;
}

VkInstance createInstance([[maybe_unused]] VkDebugUtilsMessengerEXT *debugMessenger) {
    assert(true == checkValidationLayerSupport());

    VkInstance instance = NULL;

    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = "Hello Triangle",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_3
    };

    uint32_t extensionCount = 0;
    const char **extensions = getRequiredExtensions(&extensionCount);
    
    [[maybe_unused]]
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity =  VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
//                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType =      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debugCallback,
        .flags = 0,
        .pNext = NULL,
        .pUserData = NULL
    };

    VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = enableValidationLayers ? &debugCreateInfo : NULL,
        .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = enableValidationLayers ? validationLayersCount : 0,
        .ppEnabledLayerNames = enableValidationLayers ? validationLayers : NULL,
        .enabledExtensionCount = extensionCount,
        .ppEnabledExtensionNames = extensions
    };

    MY_ASSERT(VK_SUCCESS == vkCreateInstance(&createInfo, NULL, &instance));
    assert(VK_SUCCESS == CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, NULL, debugMessenger));

    if (enableValidationLayers) free(extensions);

    return instance;
}
