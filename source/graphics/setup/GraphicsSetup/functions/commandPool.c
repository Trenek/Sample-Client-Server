#include <vulkan/vulkan.h>

#include "MY_ASSERT.h"
#include "queueFamilyIndices.h"

VkCommandPool createCommandPool(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    VkCommandPool commandPool = NULL;

    struct QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice, surface);

    VkCommandPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queueFamilyIndices.graphicsFamily.value
    };

    MY_ASSERT(VK_SUCCESS == vkCreateCommandPool(device, &poolInfo, NULL, &commandPool));

    return commandPool;
}

VkCommandPool createTransferCommandPool(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    VkCommandPool commandPool = NULL;

    struct QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice, surface);

    VkCommandPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        .queueFamilyIndex = queueFamilyIndices.graphicsFamily.value
    };

    MY_ASSERT(VK_SUCCESS == vkCreateCommandPool(device, &poolInfo, NULL, &commandPool));

    return commandPool;
}
