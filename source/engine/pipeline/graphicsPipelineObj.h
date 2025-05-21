#ifndef GRAPHICS_PIPELINE_OBJ_H
#define GRAPHICS_PIPELINE_OBJ_H

#include <vulkan/vulkan_core.h>

struct GraphicsSetup;

struct graphicsPipelineBuilder {
    VkDescriptorSetLayout objectLayout;
    VkDescriptorSetLayout cameraLayout;

    struct descriptor *texture;

    const char *vertexShader;
    const char *fragmentShader;

    const float minDepth;
    const float maxDepth;

    VkPrimitiveTopology topology;

    const size_t sizeOfVertex;
    const size_t numOfAttributes;
    VkVertexInputAttributeDescription *attributeDescription;

    VkCullModeFlags cullFlags;

    VkCompareOp operation;
};

struct graphicsPipeline {
    VkDevice device;
    struct descriptor *texture;

    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
};

struct graphicsPipeline *createObjGraphicsPipeline(struct graphicsPipelineBuilder builder, struct GraphicsSetup *vulkan);
void destroyObjGraphicsPipeline(void *pipePtr);

#endif
