#include <vulkan/vulkan_core.h>

#include "instanceBuffer.h"
#include "model.h"
#include "modelBuilder.h"
#include "MY_ASSERT.h"
#include "definitions.h"

VkDescriptorSetLayout createObjectDescriptorSetLayout(VkDevice device) {
    VkDescriptorSetLayout descriptorSetLayout = NULL;

    VkDescriptorSetLayoutBinding uboModelLayoutBinding = {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = NULL
    };

    VkDescriptorSetLayoutBinding uboMeshLayoutBinding = {
        .binding = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = NULL
    };

    VkDescriptorSetLayoutBinding bindings[] = {
        uboModelLayoutBinding,
        uboMeshLayoutBinding
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = sizeof(bindings) / sizeof(VkDescriptorSetLayoutBinding),
        .pBindings = bindings,
    };

    MY_ASSERT(VK_SUCCESS == vkCreateDescriptorSetLayout(device, &layoutInfo, NULL, &descriptorSetLayout));

    return descriptorSetLayout;
}

VkDescriptorPool createObjectDescriptorPool(VkDevice device) {
    VkDescriptorPool descriptorPool = NULL;

    VkDescriptorPoolSize poolSize[] = {
        [0] = {
            .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = MAX_FRAMES_IN_FLIGHT * 2
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

void bindObjectBuffersToDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, struct Model model, struct actualModel *actualModel) {
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        VkDescriptorBufferInfo modelBufferInfo[] = {
            {
                .buffer = model.graphics.uniformModel.buffers[i],
                .offset = 0,
                .range = model.instanceCount * sizeof(struct instanceBuffer)
            }
        };

        VkDescriptorBufferInfo meshBufferInfo[] = {
            {
                .buffer = actualModel->localMesh.buffers[i],
                .offset = 0,
                .range = actualModel->meshQuantity * sizeof(mat4)
            }
        };

        VkWriteDescriptorSet descriptorWrites[] = {
            [0] = {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = descriptorSets[i],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = sizeof(modelBufferInfo) / sizeof(VkDescriptorBufferInfo),
                .pBufferInfo = modelBufferInfo,
                .pTexelBufferView = NULL
            },
            [1] = {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = descriptorSets[i],
                .dstBinding = 1,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = sizeof(meshBufferInfo) / sizeof(VkDescriptorBufferInfo),
                .pBufferInfo = meshBufferInfo,
                .pTexelBufferView = NULL
            }
        };

        vkUpdateDescriptorSets(device, sizeof(descriptorWrites) / sizeof(VkWriteDescriptorSet), descriptorWrites, 0, NULL);
    }
}
