#include <vulkan/vulkan.h>

#include "MY_ASSERT.h"

#include "queueFamilyIndices.h"

extern const char *const deviceExtensions[];
extern const uint32_t deviceExtensionsCount;

static int32_t removeDuplications(int32_t queueNumber, VkDeviceQueueCreateInfo a[]) {
    int32_t i = 0;
    int32_t j = 0;

    bool found = false;

    while (i < queueNumber) {
        found = false;

        j = i + 1;
        while (found == false && j < queueNumber) {
            found = a[i].queueFamilyIndex == a[j].queueFamilyIndex;

            j += 1;
        }

        if (found == true) {
            j = i + 1;
            while (j < queueNumber) {
                a[j - 1] = a[j];

                j += 1;
            }

            i -= 1;
            queueNumber -= 1;
        }

        i += 1;
    }

    return queueNumber;
}

VkDevice createLogicalDevice(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice, VkQueue *graphicsQueue, VkQueue *presentQueue, VkQueue *transferQueue) {
    VkDevice device = NULL;

    struct QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);

    VkDeviceQueueCreateInfo queueCreateInfo[] = {
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = indices.graphicsFamily.value,
            .queueCount = 1,
            .pQueuePriorities = (float[]) { 1.0f },
            .flags = 0,
            .pNext = NULL
        },
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = indices.presentFamily.value,
            .queueCount = 1,
            .pQueuePriorities = (float[]) { 1.0f },
            .flags = 0,
            .pNext = NULL
        },
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = indices.transferFamily.value,
            .queueCount = 1,
            .pQueuePriorities = (float[]) { 1.0f },
            .flags = 0,
            .pNext = NULL
        }
    };
    int32_t queueNumber = removeDuplications(sizeof(queueCreateInfo) / sizeof(VkDeviceQueueCreateInfo), queueCreateInfo);

    VkPhysicalDeviceDescriptorIndexingFeatures indexingFeature = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
        .pNext = NULL
    };

    VkPhysicalDeviceFeatures2 deviceFeatures = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &indexingFeature,
        .features = {
            .samplerAnisotropy = VK_TRUE,
            .depthBounds = VK_TRUE
        },
    };

    vkGetPhysicalDeviceFeatures2(physicalDevice, &deviceFeatures);

    VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pQueueCreateInfos = queueCreateInfo,
        .queueCreateInfoCount = queueNumber,
        .pEnabledFeatures = NULL,
        .enabledExtensionCount = deviceExtensionsCount,
        .ppEnabledExtensionNames = deviceExtensions,
        .pNext = &deviceFeatures,
        .flags = 0
    };

    MY_ASSERT(indexingFeature.shaderSampledImageArrayNonUniformIndexing);
    MY_ASSERT(indexingFeature.descriptorBindingSampledImageUpdateAfterBind);
    MY_ASSERT(indexingFeature.shaderUniformBufferArrayNonUniformIndexing);
    // MY_ASSERT(indexingFeature.descriptorBindingUniformBufferUpdateAfterBind); // it is probably unnecessary
    MY_ASSERT(indexingFeature.shaderStorageBufferArrayNonUniformIndexing);
    MY_ASSERT(indexingFeature.descriptorBindingStorageBufferUpdateAfterBind);
    MY_ASSERT(VK_SUCCESS == vkCreateDevice(physicalDevice, &createInfo, NULL, &device));

    vkGetDeviceQueue(device, indices.graphicsFamily.value, 0, graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value, 0, presentQueue);
    vkGetDeviceQueue(device, indices.transferFamily.value, 0, transferQueue);

    return device;
}
