#ifndef QUEUE_FAMILY_INDICES_H
#define QUEUE_FAMILY_INDICES_H

#include <vulkan/vulkan.h>

struct value {
    bool exists;
    uint32_t value;
};

struct QueueFamilyIndices {
    struct value graphicsFamily;
    struct value presentFamily;
    struct value transferFamily;
};

bool familyEqual(struct value family1, struct value family2);
struct QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

#endif
