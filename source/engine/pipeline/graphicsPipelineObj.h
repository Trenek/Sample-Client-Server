#ifndef GRAPHICS_PIPELINE_OBJ_H
#define GRAPHICS_PIPELINE_OBJ_H

#include <vulkan/vulkan_core.h>

struct GraphicsSetup;

struct graphicsPipelineBuilder {
    struct descriptor *texture;

    const char *vertexShader;
    const char *fragmentShader;

    const float minDepth;
    const float maxDepth;
};

struct graphicsPipeline {
    VkDescriptorSetLayout objectLayout;

    struct descriptor *texture;

    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;

    size_t modelQuantity;
    struct Model *model;
};

struct graphicsPipeline createObjGraphicsPipeline(struct graphicsPipelineBuilder builder, struct GraphicsSetup *vulkan);
void destroyObjGraphicsPipeline(VkDevice device, struct graphicsPipeline pipe);

#endif
