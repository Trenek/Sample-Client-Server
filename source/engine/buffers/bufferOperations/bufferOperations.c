#include <vulkan/vulkan.h>

#include "queueFamilyIndices.h"
#include "commonOperations.h"

#include "MY_ASSERT.h"

VkBuffer createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkDeviceSize size, VkBufferUsageFlags usage) {
    VkBuffer vertexBuffer = NULL;
    struct QueueFamilyIndices families = findQueueFamilies(physicalDevice, surface);

    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = familyEqual(families.graphicsFamily, families.transferFamily) ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount = 2,
        .pQueueFamilyIndices = (uint32_t[]){ families.graphicsFamily.value, families.transferFamily.value },
        .flags = 0
    };

    MY_ASSERT(VK_SUCCESS == vkCreateBuffer(device, &bufferInfo, NULL, &vertexBuffer));

    return vertexBuffer;
}

VkDeviceMemory createBufferMemory(VkDevice device, VkPhysicalDevice physicalDevice, VkBuffer vertexBuffer, VkMemoryPropertyFlags properties) {
    VkDeviceMemory vertexBufferMemory = NULL;

    VkMemoryRequirements memRequirements; {
        vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);
    }

    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties)
    };

    // not supposet to call for every individual buffer, 4096 MAX, better to create custom allocator or use VulkanMemoryAllocator
    MY_ASSERT(VK_SUCCESS == vkAllocateMemory(device, &allocInfo, NULL, &vertexBufferMemory));
    vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);

    return vertexBufferMemory;
}

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDevice device, VkCommandPool commandPool, VkQueue queue) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);

    VkBufferCopy copyRegion = {
        .srcOffset = 0,
        .dstOffset = 0,
        .size = size
    };

    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCOmmands(commandBuffer, device, commandPool, queue);
}

void destroyBuffer(VkDevice device, VkBuffer buffer, VkDeviceMemory bufferMemory) {
    vkDestroyBuffer(device, buffer, NULL);
    vkFreeMemory(device, bufferMemory, NULL);
}
