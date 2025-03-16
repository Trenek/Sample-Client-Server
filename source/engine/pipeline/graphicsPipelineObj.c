#include "graphicsPipelineObj.h"

#include "pipelineFunctions.h"
#include "graphicsSetup.h"

struct graphicsPipeline createObjGraphicsPipeline(struct graphicsPipelineBuilder builder, struct GraphicsSetup *vulkan) {
    struct graphicsPipeline graphics = {
        .objectLayout = createObjectDescriptorSetLayout(vulkan->device),
        .texture = builder.texture
    };

    graphics.pipelineLayout = createPipelineLayout(vulkan->device, graphics.objectLayout, builder.texture->descriptorSetLayout, vulkan->cameraDescriptorSetLayout);
    graphics.pipeline = createGraphicsPipeline(builder.vertexShader, builder.fragmentShader, builder.minDepth, builder.maxDepth, vulkan->device, vulkan->renderPass, graphics.pipelineLayout, vulkan->msaaSamples);

    return graphics;
}

void destroyObjGraphicsPipeline(VkDevice device, struct graphicsPipeline pipe) {
    vkDeviceWaitIdle(device);

    vkDestroyPipeline(device, pipe.pipeline, NULL);
    vkDestroyPipelineLayout(device, pipe.pipelineLayout, NULL);

    vkDestroyDescriptorSetLayout(device, pipe.objectLayout, NULL);
}
