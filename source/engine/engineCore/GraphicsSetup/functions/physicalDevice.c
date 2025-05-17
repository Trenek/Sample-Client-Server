#include <string.h>
#include <myMalloc.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_beta.h>

#include "MY_ASSERT.h"
#include "swapChainSupportDetails.h"
#include "queueFamilyIndices.h"

#include <stdio.h>

const char *const deviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
#ifdef __APPLE__
    VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
#endif
};
const uint32_t deviceExtensionsCount = sizeof(deviceExtensions) / sizeof(const char *const);

static bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount = 0; {
        vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL);
    }
    VkExtensionProperties avaibleExtensions[extensionCount];

    uint32_t i = 0;
    uint32_t j = 0;
    bool isFound = true;

    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, avaibleExtensions);

    while (i < deviceExtensionsCount && isFound == true) {
        bool isElemFound = false;
        j = 0;

        while (isElemFound == false && j < extensionCount) {
            if (strcmp(deviceExtensions[i], avaibleExtensions[j].extensionName) == 0) {
                isElemFound = true;
            }

            j += 1;
        }

        isFound = isFound && isElemFound;
        i += 1;
    }

    return isFound;
}

static int isDevicePreferable(VkPhysicalDevice device, VkSurfaceKHR surface) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    struct SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);

    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    freeSwapChainSupportDetails(&swapChainSupport);

    return
        (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 1000 : 0) +
        (deviceFeatures.geometryShader ? 1 : 0);
}

void printLogical(bool a) {
    printf("%s\n", a ? "Tak" : "Nie");
}

static bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    struct QueueFamilyIndices queueFamilies = findQueueFamilies(device, surface);
    struct SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);

    bool swapChainAdequate = swapChainSupport.formatCount != 0 &&
                             swapChainSupport.presentModeCount != 0;

    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    freeSwapChainSupportDetails(&swapChainSupport);
    
    return
//      deviceFeatures.depthBounds &&
        deviceFeatures.samplerAnisotropy &&
        checkDeviceExtensionSupport(device) &&
        swapChainAdequate &&
        queueFamilies.graphicsFamily.exists &&
        queueFamilies.presentFamily.exists;
}

static VkSampleCountFlagBits getMaxUsableSampleCount(VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceProperties physicalDeviceProperties; {
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    }
    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
                                physicalDeviceProperties.limits.framebufferDepthSampleCounts;

    return (counts & VK_SAMPLE_COUNT_64_BIT) ? VK_SAMPLE_COUNT_64_BIT :
           (counts & VK_SAMPLE_COUNT_32_BIT) ? VK_SAMPLE_COUNT_32_BIT :
           (counts & VK_SAMPLE_COUNT_16_BIT) ? VK_SAMPLE_COUNT_16_BIT :
           (counts & VK_SAMPLE_COUNT_8_BIT)  ? VK_SAMPLE_COUNT_8_BIT :
           (counts & VK_SAMPLE_COUNT_4_BIT)  ? VK_SAMPLE_COUNT_4_BIT :
           (counts & VK_SAMPLE_COUNT_2_BIT)  ? VK_SAMPLE_COUNT_2_BIT :
                                               VK_SAMPLE_COUNT_1_BIT;
}

VkPhysicalDevice pickPhysicalDevice(VkSampleCountFlagBits *msaaSamples, VkInstance instance, VkSurfaceKHR surface) {
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    uint32_t deviceCount = 0; {
        vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
        MY_ASSERT(deviceCount != 0);
    }
    VkPhysicalDevice devices[deviceCount];
    uint32_t i = 0;
    uint32_t preferableScore = 0;
    uint32_t currentScore = 0;

    vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

    while (i < deviceCount) {
        if (isDeviceSuitable(devices[i], surface)) {
            currentScore = isDevicePreferable(devices[i], surface);
            if (currentScore >= preferableScore) {
                preferableScore = currentScore;
                physicalDevice = devices[i];
            }
        }
        i += 1;
    }

    MY_ASSERT(physicalDevice != NULL);
    *msaaSamples = getMaxUsableSampleCount(physicalDevice);

    return physicalDevice;
}
