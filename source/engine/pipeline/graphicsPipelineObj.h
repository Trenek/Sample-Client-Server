#ifndef GRAPHICS_PIPELINE_OBJ_H
#define GRAPHICS_PIPELINE_OBJ_H

#include <vulkan/vulkan_core.h>

struct GraphicsSetup;

struct graphicsPipelineBuilder {
    VkDescriptorSetLayout objectLayout;

    struct descriptor *texture;

    const char *vertexShader;
    const char *fragmentShader;

    const float minDepth;
    const float maxDepth;

    VkPrimitiveTopology topology;

    const size_t sizeOfVertex;
    const size_t numOfAttributes;
    VkVertexInputAttributeDescription *attributeDescription;

    VkCompareOp operation;
};

struct graphicsPipeline {
    struct descriptor *texture;

    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;

    size_t modelQuantity;
    struct Entity **model;
};

struct graphicsPipeline createObjGraphicsPipeline(struct graphicsPipelineBuilder builder, struct GraphicsSetup *vulkan);
void destroyObjGraphicsPipeline(VkDevice device, struct graphicsPipeline pipe);

#endif
