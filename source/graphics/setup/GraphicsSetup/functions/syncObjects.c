#include <vulkan/vulkan.h>

#include "MY_ASSERT.h"

VkSemaphore createSemaphore(VkDevice device) {
    VkSemaphore semaphore = NULL;

    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .flags = 0,
        .pNext = NULL
    };

    MY_ASSERT(VK_SUCCESS == vkCreateSemaphore(device, &semaphoreInfo, NULL, &semaphore));

    return semaphore;
}

VkFence createFence(VkDevice device) {
    VkFence fence = NULL;

    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        .pNext = NULL
    };

    MY_ASSERT(VK_SUCCESS == vkCreateFence(device, &fenceInfo, NULL, &fence));

    return fence;
}
