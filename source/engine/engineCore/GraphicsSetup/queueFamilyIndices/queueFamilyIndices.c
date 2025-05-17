#include "queueFamilyIndices.h"

bool familyEqual(struct value family1, struct value family2) {
    return family1.exists == family2.exists && family1.value == family2.value;
}

struct QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
    struct QueueFamilyIndices indices = { 0 };

    uint32_t queueFamilyCount = 0; {
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);
    }
    VkQueueFamilyProperties queueFamilies[queueFamilyCount];

    VkBool32 presentSupport = false;
    bool found = false;

    uint32_t i = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    while (i < queueFamilyCount && found == false) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily.value = i;
            indices.graphicsFamily.exists = true;
        }

        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport) {
            indices.presentFamily.value = i;
            indices.presentFamily.exists = true;
        }

        if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            indices.transferFamily.value = i;
            indices.transferFamily.exists = true;
        }

        found = indices.graphicsFamily.exists &&
                indices.presentFamily.exists &&
                indices.transferFamily.exists &&
                indices.graphicsFamily.value == i &&
                indices.presentFamily.value == i &&
                indices.transferFamily.value == i;

        i += 1;
    }

    return indices;
}
