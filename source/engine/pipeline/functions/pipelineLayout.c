#include <vulkan/vulkan.h>

#include "MY_ASSERT.h"
#include "pushConstantsBuffer.h"

VkPipelineLayout createPipelineLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSetLayout descriptorSetLayout2, VkDescriptorSetLayout descriptorSetLayout3) {
    VkPipelineLayout pipelineLayout = NULL;

    VkPushConstantRange pushConstant = {
        .offset = 0,
        .size = sizeof(struct MeshPushConstants),
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
    };

    VkDescriptorSetLayout layouts[] = {
        descriptorSetLayout,
        descriptorSetLayout2,
        descriptorSetLayout3
    };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = sizeof(layouts) / sizeof(VkDescriptorSetLayout),
        .pSetLayouts = layouts,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &pushConstant
    };

    MY_ASSERT(VK_SUCCESS == vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &pipelineLayout));

    return pipelineLayout;
}
