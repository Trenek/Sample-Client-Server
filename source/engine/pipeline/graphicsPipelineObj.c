#include "graphicsPipelineObj.h"

#include "pipelineFunctions.h"
#include "graphicsSetup.h"

#include "descriptor.h"

struct graphicsPipeline createObjGraphicsPipeline(struct graphicsPipelineBuilder builder, struct GraphicsSetup *vulkan) {
    VkDescriptorSetLayout descriptorSetLayout[] = {
        builder.objectLayout,
        builder.texture->descriptorSetLayout,
        builder.cameraLayout,
    };
    size_t qDescriptorSetLayout = sizeof(descriptorSetLayout) / sizeof(VkDescriptorSetLayout);

    struct graphicsPipeline graphics = {
        .texture = builder.texture,
        .pipelineLayout = createPipelineLayout(vulkan->device, qDescriptorSetLayout, descriptorSetLayout)
    };

    graphics.pipeline = createGraphicsPipeline(builder, vulkan->device, vulkan->renderPass, graphics.pipelineLayout, vulkan->msaaSamples);

    return graphics;
}

void destroyObjGraphicsPipeline(VkDevice device, struct graphicsPipeline pipe) {
    vkDeviceWaitIdle(device);

    vkDestroyPipeline(device, pipe.pipeline, NULL);
    vkDestroyPipelineLayout(device, pipe.pipelineLayout, NULL);
}
