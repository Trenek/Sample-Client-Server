#include <vulkan/vulkan_core.h>

#include "MY_ASSERT.h"
#include "definitions.h"

VkDescriptorSetLayout createObjectDescriptorSetLayout(VkDevice device, size_t qBindings, VkDescriptorSetLayoutBinding bindings[qBindings]) {
    VkDescriptorSetLayout descriptorSetLayout = NULL;

    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = qBindings,
        .pBindings = bindings,
    };

    MY_ASSERT(VK_SUCCESS == vkCreateDescriptorSetLayout(device, &layoutInfo, NULL, &descriptorSetLayout));

    return descriptorSetLayout;
}

VkDescriptorPool createObjectDescriptorPool(VkDevice device, size_t qBuff) {
    VkDescriptorPool descriptorPool = NULL;

    VkDescriptorPoolSize poolSize[] = {
        [0] = {
            .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = MAX_FRAMES_IN_FLIGHT * qBuff
        }
    };

    VkDescriptorPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = sizeof(poolSize) / sizeof(VkDescriptorPoolSize),
        .pPoolSizes = poolSize,
        .maxSets = MAX_FRAMES_IN_FLIGHT,
        .flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT
    };

    MY_ASSERT(VK_SUCCESS == vkCreateDescriptorPool(device, &poolInfo, NULL, &descriptorPool));

    return descriptorPool;
}

void bindObjectBuffersToDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, size_t qBuff, VkBuffer (*buff[qBuff])[MAX_FRAMES_IN_FLIGHT], size_t range[qBuff]) {
    VkWriteDescriptorSet descriptorWrites[qBuff];
    VkDescriptorBufferInfo bufferInfo[qBuff];

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        for (size_t j = 0; j < qBuff; j += 1) {
            bufferInfo[j] = (VkDescriptorBufferInfo) {
                .buffer = (*buff[j])[i],
                .offset = 0,
                .range = range[j]
            };
            descriptorWrites[j] = (VkWriteDescriptorSet){
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = descriptorSets[i],
                .dstBinding = j,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = 1,
                .pBufferInfo = &bufferInfo[j],
                .pTexelBufferView = NULL
            };
        }

        vkUpdateDescriptorSets(device, qBuff, descriptorWrites, 0, NULL);
    }
}
