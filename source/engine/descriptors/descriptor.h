#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include <vulkan/vulkan_core.h>

#include "definitions.h"

struct Entity;

struct descriptorSetLayout {
    VkDevice device;
    VkDescriptorSetLayout descriptorSetLayout;
};

struct descriptorSetLayout *createDescriptorSetLayout(VkDescriptorSetLayout layout, VkDevice device);
void destroyDescriptorSetLayout(void *layoutPtr);

struct descriptor {
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSets[MAX_FRAMES_IN_FLIGHT];
};

VkDescriptorSetLayout createCameraDescriptorSetLayout(VkDevice device);
VkDescriptorPool createCameraDescriptorPool(VkDevice device);
void bindCameraBuffersToDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, VkBuffer uniformBuffers[]);

VkDescriptorSetLayout createObjectDescriptorSetLayout(VkDevice device, size_t qBindings, VkDescriptorSetLayoutBinding bindings[qBindings]);
VkDescriptorPool createObjectDescriptorPool(VkDevice device, size_t qBuff);
void bindObjectBuffersToDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, size_t qBuff, VkBuffer (*buff[qBuff])[MAX_FRAMES_IN_FLIGHT], size_t range[qBuff]);

void createDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout);

#endif
