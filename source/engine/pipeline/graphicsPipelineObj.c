#include <malloc.h>

#include "graphicsPipelineObj.h"
#include "renderPassCore.h"

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

    graphics->qPipelines = builder.qRenderPassCore;
    graphics->pipeline = malloc(sizeof(struct renderPipeline) * graphics->qPipelines);
    for (size_t i = 0; i < graphics->qPipelines; i += 1) {
        graphics->pipeline[i].pipeline = createGraphicsPipeline(
            builder, 
            vulkan->device, 
            builder.renderPassCore[i]->renderPass, 
            graphics->pipelineLayout, 
            vulkan->msaaSamples
        );
        graphics->pipeline[i].core = builder.renderPassCore[i];
    }

    return graphics;
}

void destroyObjGraphicsPipeline(void *pipePtr) {
    struct graphicsPipeline *pipe = pipePtr;
    vkDeviceWaitIdle(pipe->device);

    for (size_t i = 0; i < pipe->qPipelines; i += 1) {
        vkDestroyPipeline(pipe->device, pipe->pipeline[i].pipeline, NULL);
    }
    free(pipe->pipeline);

    vkDestroyPipelineLayout(pipe->device, pipe->pipelineLayout, NULL);

    free(pipe);
}
