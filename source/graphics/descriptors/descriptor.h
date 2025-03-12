#include "VulkanTools.h"


VkDescriptorSetLayout createCameraDescriptorSetLayout(VkDevice device);
VkDescriptorPool createCameraDescriptorPool(VkDevice device);
void bindCameraBuffersToDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, VkBuffer uniformBuffers[]);
void createDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout);
