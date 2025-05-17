#include <vulkan/vulkan.h>

#include "definitions.h"
#include "MY_ASSERT.h"

void createCommandBuffer(VkCommandBuffer commandBuffer[MAX_FRAMES_IN_FLIGHT], VkDevice device, VkCommandPool commandPool) {
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = MAX_FRAMES_IN_FLIGHT
    };

    MY_ASSERT(VK_SUCCESS == vkAllocateCommandBuffers(device, &allocInfo, commandBuffer));
}
