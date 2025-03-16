#include <vulkan/vulkan_core.h>

#include "texture.h"
#include "MY_ASSERT.h"
#include "definitions.h"

VkDescriptorSetLayout createTextureDescriptorSetLayout(VkDevice device, uint32_t textureQuantity) {
    VkDescriptorSetLayout descriptorSetLayout = NULL;

    VkDescriptorSetLayoutBinding samplerLayoutBinding = {
        .binding = 0,
        .descriptorCount = textureQuantity,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImmutableSamplers = NULL,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
    };

    VkDescriptorSetLayoutBinding bindings[] = {
        samplerLayoutBinding
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = sizeof(bindings) / sizeof(VkDescriptorSetLayoutBinding),
        .pBindings = bindings,
    };

    MY_ASSERT(VK_SUCCESS == vkCreateDescriptorSetLayout(device, &layoutInfo, NULL, &descriptorSetLayout));

    return descriptorSetLayout;
}

VkDescriptorPool createTextureDescriptorPool(VkDevice device, uint32_t texturesCount) {
    VkDescriptorPool descriptorPool = NULL;

    VkDescriptorPoolSize poolSize[] = {
        [0] = {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = texturesCount * MAX_FRAMES_IN_FLIGHT
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

void bindTextureBuffersToDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, uint32_t texturesQuantity, struct Textures *texture) {
    VkDescriptorImageInfo imageInfoArray[texturesQuantity];
    for (uint32_t i = 0; i < texturesQuantity; i += 1) {
        imageInfoArray[i] = (VkDescriptorImageInfo) {
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .imageView = texture->data[i].imageView,
            .sampler = texture->data[i].sampler
        };
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        VkWriteDescriptorSet descriptorWrites[] = {
            [0] = {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = descriptorSets[i],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = sizeof(imageInfoArray) / sizeof(VkDescriptorImageInfo),
                .pImageInfo = imageInfoArray,
                .pTexelBufferView = NULL
            }
        };

        vkUpdateDescriptorSets(device, sizeof(descriptorWrites) / sizeof(VkWriteDescriptorSet), descriptorWrites, 0, NULL);
    }
}
