#include "Vertex.h"

VkImage createTextureBuffer(VkDeviceMemory *textureImageMemory, uint32_t *mipLevels, const char *texturePath, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkCommandPool commandPool, VkQueue queue);
VkImageView createTextureImageView(VkDevice device, VkImage image, uint32_t mipmap);
VkSampler createTextureSampler(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t mipLevels);
VkBuffer createVertexBuffer(VkDeviceMemory *vertexBufferMemory, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkCommandPool transferCommandPool, VkQueue transferQueue, uint32_t vertexQuantity, struct Vertex vertices[static vertexQuantity]);
VkBuffer createIndexBuffer(VkDeviceMemory *indexBufferMemory, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkCommandPool transferCommandPool, VkQueue transferQueue, uint32_t vertexQuantity, uint32_t indicesQuantity, uint16_t indices[static indicesQuantity]);
void createStorageBuffer(uint32_t quantity, VkBuffer uniformBuffers[], VkDeviceMemory uniformBuffersMemory[], void *uniformBuffersMapped[], VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
void destroyStorageBuffer(VkDevice device, VkBuffer uniformBuffers[], VkDeviceMemory uniformBuffersMemory[]);
VkPipelineLayout createPipelineLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSetLayout descriptorSetLayout2, VkDescriptorSetLayout descriptorSetLayout3);
VkPipeline createGraphicsPipeline(const char *vertexShader, const char *fragmentShader, float minDepth, float maxDepth, VkDevice device, VkRenderPass renderPass, VkPipelineLayout pipelineLayout, VkSampleCountFlagBits msaaSamples);
void createDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout);
