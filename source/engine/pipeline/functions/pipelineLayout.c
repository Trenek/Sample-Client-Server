#include <vulkan/vulkan.h>

#include "MY_ASSERT.h"
#include "pushConstantsBuffer.h"

VkPipelineLayout createPipelineLayout(VkDevice device, size_t qDescriptorLayout, VkDescriptorSetLayout descriptorSetLayout[qDescriptorLayout]) {
    VkPipelineLayout pipelineLayout = NULL;

    VkPushConstantRange pushConstant = {
        .offset = 0,
        .size = sizeof(struct MeshPushConstants),
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
    };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = qDescriptorLayout,
        .pSetLayouts = descriptorSetLayout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &pushConstant
    };

    MY_ASSERT(VK_SUCCESS == vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &pipelineLayout));

    return pipelineLayout;
}
