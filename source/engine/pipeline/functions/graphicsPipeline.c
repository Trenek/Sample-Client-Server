#include <stdio.h>
#include <myMalloc.h>
#include <stddef.h>

#include <vulkan/vulkan.h>

#include "MY_ASSERT.h"

#include "graphicsPipelineObj.h"

#define UNUSED_RETVAL(x) if (x) {}

static char *loadFile(const char *filename, int *fileCount) {
    FILE *file = fopen(filename, "rb");

    char *buffor = NULL;

    MY_ASSERT(NULL != file);

    fseek(file, 0, SEEK_END);
    *fileCount = ftell(file);
    buffor = malloc(sizeof(char) * *fileCount);

    fseek(file, 0, SEEK_SET);

    UNUSED_RETVAL(fread(buffor, sizeof(char), *fileCount, file));

    fclose(file);

    return buffor;
}

static VkShaderModule createShaderModule(VkDevice device, const char *filename) {
    VkShaderModule shaderModule = NULL;

    int size = 0;
    char *code = loadFile(filename, &size);

    VkShaderModuleCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = size,
        .pCode = (const uint32_t *)code, // might potentially cause problems
        .pNext = NULL,
        .flags = 0
    };

    MY_ASSERT(VK_SUCCESS == vkCreateShaderModule(device, &createInfo, NULL, &shaderModule));

    free(code);

    return shaderModule;
}

VkPipeline createGraphicsPipeline(struct graphicsPipelineBuilder builder, VkDevice device, VkRenderPass renderPass, VkPipelineLayout pipelineLayout, VkSampleCountFlagBits msaaSamples) {
    VkPipeline graphicsPipeline = NULL;

    VkShaderModule vertShaderModule = createShaderModule(device, builder.vertexShader);
    VkShaderModule fragShaderModule = createShaderModule(device, builder.fragmentShader);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertShaderModule,
        .pName = "main",
        .flags = 0,
        .pNext = NULL,
        .pSpecializationInfo = NULL
    };

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragShaderModule,
        .pName = "main",
        .flags = 0,
        .pNext = NULL,
        .pSpecializationInfo = NULL
    };

    VkPipelineShaderStageCreateInfo shaderStages[] = {
        vertShaderStageInfo,
        fragShaderStageInfo
    };

    // fixed-function stages from now on

    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = sizeof(dynamicStates) / sizeof(VkDynamicState),
        .pDynamicStates = dynamicStates
    };

    VkPipelineViewportStateCreateInfo viewportState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1
    };

    VkVertexInputBindingDescription bindingDescription = {
        .binding = 0,
        .stride = builder.sizeOfVertex,
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &bindingDescription,
        .vertexAttributeDescriptionCount = builder.numOfAttributes,
        .pVertexAttributeDescriptions = builder.attributeDescription
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = builder.topology,
        .primitiveRestartEnable = VK_FALSE
    };

    VkPipelineRasterizationStateCreateInfo rasterizer = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .lineWidth = 1.0f,
        .cullMode = builder.cullFlags,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f, // optional
        .depthBiasClamp = 0.0f, // optional
        .depthBiasSlopeFactor = 0.0f // optional
    };

    VkPipelineMultisampleStateCreateInfo multisampling = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable = VK_FALSE,
        .rasterizationSamples = msaaSamples,
        .minSampleShading = 1.0f, // optional
        .pSampleMask = NULL, // optional
        .alphaToCoverageEnable = VK_FALSE, // optional
        .alphaToOneEnable = VK_FALSE // optional
    };

    VkPipelineColorBlendAttachmentState colorBlendAttackment = {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                          VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT |
                          VK_COLOR_COMPONENT_A_BIT,
        .blendEnable = VK_FALSE, // false == everything below is optional
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD
    };

    VkPipelineColorBlendStateCreateInfo colorBlending = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttackment,
        .blendConstants = {
            [0] = 0.0f,
            [1] = 0.0f,
            [2] = 0.0f,
            [3] = 0.0f
        }
    };

    VkPipelineDepthStencilStateCreateInfo depthStencil = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = builder.operation,
        .depthBoundsTestEnable = VK_FALSE,
        .minDepthBounds = builder.minDepth,
        .maxDepthBounds = builder.maxDepth,
        .stencilTestEnable = VK_FALSE,
        .front = { 0 }, // optional
        .back = { 0 }, // optional
    };

    VkGraphicsPipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = &depthStencil,
        .pColorBlendState = &colorBlending,
        .pDynamicState = &dynamicState,
        .layout = pipelineLayout,
        .renderPass = renderPass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1
    };

    MY_ASSERT(VK_SUCCESS == vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &graphicsPipeline));

    vkDestroyShaderModule(device, fragShaderModule, NULL);
    vkDestroyShaderModule(device, vertShaderModule, NULL);

    return graphicsPipeline;
}
