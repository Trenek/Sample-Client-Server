#ifndef BUFFER_H
#define BUFFER_H

#include <vulkan/vulkan_core.h>
#include "definitions.h"

struct buffer {
    VkBuffer buffers[MAX_FRAMES_IN_FLIGHT];
    VkDeviceMemory buffersMemory[MAX_FRAMES_IN_FLIGHT];
    void *buffersMapped[MAX_FRAMES_IN_FLIGHT];
};

VkBuffer createVertexBuffer(VkDeviceMemory *vertexBufferMemory, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkCommandPool transferCommandPool, VkQueue transferQueue, uint32_t vertexQuantity, void *vertices, size_t sizeOfBuffer);
VkBuffer createIndexBuffer(VkDeviceMemory *indexBufferMemory, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkCommandPool transferCommandPool, VkQueue transferQueue, uint32_t vertexQuantity, uint32_t indicesQuantity, uint16_t indices[static indicesQuantity], size_t sizeOfBuffer);
void createBuffers(VkBufferUsageFlagBits bufferUsageBit, VkDeviceSize bufferSize, VkBuffer uniformBuffers[], VkDeviceMemory uniformBuffersMemory[], void *uniformBuffersMapped[], VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
void destroyBuffers(VkDevice device, VkBuffer uniformBuffers[], VkDeviceMemory uniformBuffersMemory[]);

#endif
