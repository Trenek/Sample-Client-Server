#include <string.h>

#include <vulkan/vulkan.h>

#include "MY_ASSERT.h"
#include "bufferOperations.h"

VkBuffer createVertexBuffer(VkDeviceMemory *vertexBufferMemory, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkCommandPool transferCommandPool, VkQueue transferQueue, uint32_t vertexQuantity, void *vertices, size_t sizeOfBuffer) {
    VkBuffer vertexBuffer = NULL;

    VkBuffer stagingBuffer = createBuffer(
        device,
        physicalDevice,
        surface,
        vertexQuantity * sizeOfBuffer,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT
    );
    VkDeviceMemory stagingBufferMemory = createBufferMemory(
        device,
        physicalDevice,
        stagingBuffer,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    void *data = NULL;

    vkMapMemory(device, stagingBufferMemory, 0, /*bufferInfo.size*/ vertexQuantity * sizeOfBuffer, 0, &data);
    memcpy(data, vertices, /*bufferInfo.size*/ vertexQuantity * sizeOfBuffer);
    vkUnmapMemory(device, stagingBufferMemory);

    vertexBuffer = createBuffer(
        device,
        physicalDevice,
        surface,
        vertexQuantity * sizeOfBuffer,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
    );
    *vertexBufferMemory = createBufferMemory(
        device,
        physicalDevice,
        vertexBuffer,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    copyBuffer(stagingBuffer, vertexBuffer, vertexQuantity * sizeOfBuffer, device, transferCommandPool, transferQueue);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);

    return vertexBuffer;
}

VkBuffer createIndexBuffer(VkDeviceMemory *indexBufferMemory, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkCommandPool transferCommandPool, VkQueue transferQueue, uint32_t vertexQuantity, uint32_t indicesQuantity, uint16_t indices[static indicesQuantity], size_t sizeOfVertexBuffer) {
    VkBuffer indexBuffer = NULL;

    VkBuffer stagingBuffer = createBuffer(
        device,
        physicalDevice,
        surface,
        vertexQuantity * sizeOfVertexBuffer,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT
    );
    VkDeviceMemory stagingBufferMemory = createBufferMemory(
        device,
        physicalDevice,
        stagingBuffer,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    void *data = NULL;

    vkMapMemory(device, stagingBufferMemory, 0, /*bufferInfo.size*/ indicesQuantity * sizeof(uint16_t), 0, &data);
    memcpy(data, indices, /*bufferInfo.size*/ indicesQuantity * sizeof(uint16_t));
    vkUnmapMemory(device, stagingBufferMemory);

    indexBuffer = createBuffer(
        device,
        physicalDevice,
        surface,
        vertexQuantity * sizeOfVertexBuffer,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
    );
    *indexBufferMemory = createBufferMemory(
        device,
        physicalDevice,
        indexBuffer,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    copyBuffer(stagingBuffer, indexBuffer, indicesQuantity * sizeof(uint16_t), device, transferCommandPool, transferQueue);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);

    return indexBuffer;
}
