#include <vulkan/vulkan.h>

VkBuffer createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkDeviceSize size, VkBufferUsageFlags usage);
VkDeviceMemory createBufferMemory(VkDevice device, VkPhysicalDevice physicalDevice, VkBuffer vertexBuffer, VkMemoryPropertyFlags properties);
void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDevice device, VkCommandPool transferCommandPool, VkQueue transferQueue);
void destroyBuffer(VkDevice device, VkBuffer buffer, VkDeviceMemory bufferMemory);
