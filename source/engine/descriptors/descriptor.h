#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include <vulkan/vulkan_core.h>

#include "definitions.h"

struct Model;
struct actualModel;

struct descriptor {
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSets[MAX_FRAMES_IN_FLIGHT];
};

VkDescriptorSetLayout createCameraDescriptorSetLayout(VkDevice device);
VkDescriptorPool createCameraDescriptorPool(VkDevice device);
void bindCameraBuffersToDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, VkBuffer uniformBuffers[]);

VkDescriptorSetLayout createObjectDescriptorSetLayout(VkDevice device);
VkDescriptorPool createObjectDescriptorPool(VkDevice device);
void bindObjectBuffersToDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, struct Model model, uint32_t meshQuantity, VkBuffer (*buffers)[MAX_FRAMES_IN_FLIGHT]);

void createDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout);

#endif
