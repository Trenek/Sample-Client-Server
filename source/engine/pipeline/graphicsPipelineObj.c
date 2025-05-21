#include <malloc.h>

#include "graphicsPipelineObj.h"

#include "pipelineFunctions.h"
#include "graphicsSetup.h"

#include "descriptor.h"

struct graphicsPipeline *createObjGraphicsPipeline(struct graphicsPipelineBuilder builder, struct GraphicsSetup *vulkan) {
    VkDescriptorSetLayout descriptorSetLayout[] = {
        builder.objectLayout,
        builder.texture->descriptorSetLayout,
        builder.cameraLayout,
    };
    size_t qDescriptorSetLayout = sizeof(descriptorSetLayout) / sizeof(VkDescriptorSetLayout);

    struct graphicsPipeline *graphics = calloc(1, sizeof(struct graphicsPipeline)); 
    *graphics = (struct graphicsPipeline) {
        .device = vulkan->device,
        .texture = builder.texture,
        .pipelineLayout = createPipelineLayout(vulkan->device, qDescriptorSetLayout, descriptorSetLayout)
    };

    graphics->pipeline = createGraphicsPipeline(builder, vulkan->device, vulkan->renderPass, graphics->pipelineLayout, vulkan->msaaSamples);

    return graphics;
}

void destroyObjGraphicsPipeline(void *pipePtr) {
    struct graphicsPipeline *pipe = pipePtr;
    vkDeviceWaitIdle(pipe->device);

    vkDestroyPipeline(pipe->device, pipe->pipeline, NULL);
    vkDestroyPipelineLayout(pipe->device, pipe->pipelineLayout, NULL);

    free(pipe);
}
