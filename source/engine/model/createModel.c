#include "graphicsSetup.h"

#include "modelBuilder.h"

#include "modelFunctions.h"
#include "instanceBuffer.h"

#include "descriptor.h"

struct Model createModels(struct ModelBuilder modelBuilder, struct GraphicsSetup *vulkan) {
    struct Model result = { 0 };

    createStorageBuffer(modelBuilder.instanceCount * sizeof(struct instanceBuffer), result.graphics.uniformModel.buffers, result.graphics.uniformModel.buffersMemory, result.graphics.uniformModel.buffersMapped, vulkan->device, vulkan->physicalDevice, vulkan->surface);
    result.instanceBuffer = malloc(modelBuilder.instanceCount * sizeof(struct instanceBuffer));

    result.instanceCount = modelBuilder.instanceCount;
    result.instance = malloc(sizeof(struct instance) * result.instanceCount);
    for (uint32_t i = 0; i < result.instanceCount; i += 1) {
        result.instanceBuffer[i] = (struct instanceBuffer) {
            .modelMatrix = { { 0 } },
            .textureIndex = 0
        };
    }

    result.texturePointer = modelBuilder.texturePointer;
    result.texturesQuantity = modelBuilder.texturesQuantity;
    result.graphics.texture = modelBuilder.texture;

    result.actualModel = modelBuilder.modelPath;

    result.graphics.object.descriptorSetLayout = createObjectDescriptorSetLayout(vulkan->device);
    result.graphics.object.descriptorPool = createObjectDescriptorPool(vulkan->device);
    createDescriptorSets(result.graphics.object.descriptorSets, vulkan->device, result.graphics.object.descriptorPool, result.graphics.object.descriptorSetLayout);
    bindObjectBuffersToDescriptorSets(result.graphics.object.descriptorSets, vulkan->device, result, result.actualModel);

    result.graphics.pipelineLayout = createPipelineLayout(vulkan->device, result.graphics.object.descriptorSetLayout, modelBuilder.texture->descriptorSetLayout, vulkan->cameraDescriptorSetLayout);
    result.graphics.graphicsPipeline = createGraphicsPipeline(modelBuilder.vertexShader, modelBuilder.fragmentShader, modelBuilder.minDepth, modelBuilder.maxDepth, vulkan->device, vulkan->renderPass, result.graphics.pipelineLayout, vulkan->msaaSamples);

    return result;
}

static void destroyModels(VkDevice device, struct Model model) {
    free(model.instance);

    destroyStorageBuffer(device, model.graphics.uniformModel.buffers, model.graphics.uniformModel.buffersMemory);

    vkDestroyPipeline(device, model.graphics.graphicsPipeline, NULL);
    vkDestroyPipelineLayout(device, model.graphics.pipelineLayout, NULL);

    vkDestroyDescriptorPool(device, model.graphics.object.descriptorPool, NULL);
    vkDestroyDescriptorSetLayout(device, model.graphics.object.descriptorSetLayout, NULL);
}

void destroyModelArray(uint16_t num, struct Model modelArray[num], struct GraphicsSetup *graphics) {
    vkDeviceWaitIdle(graphics->device);

    for (uint16_t i = 0; i < num; i += 1) {
        destroyModels(graphics->device, modelArray[i]);
    }
}
