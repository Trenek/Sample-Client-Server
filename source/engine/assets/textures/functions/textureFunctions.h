#include "texture.h"

VkDescriptorSetLayout createTextureDescriptorSetLayout(VkDevice device, uint32_t textureQuantity);
VkDescriptorPool createTextureDescriptorPool(VkDevice device, uint32_t texturesCount);
void bindTextureBuffersToDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, uint32_t texturesQuantity, struct Textures *texture);

VkImage createTextureBuffer(VkDeviceMemory *textureImageMemory, uint32_t *mipLevels, const char *texturePath, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkCommandPool commandPool, VkQueue queue);
VkImageView createTextureImageView(VkDevice device, VkImage image, uint32_t mipmap);
VkSampler createTextureSampler(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t mipLevels);
