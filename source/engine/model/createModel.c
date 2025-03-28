#include "graphicsSetup.h"

#include "modelBuilder.h"
#include "instanceBuffer.h"

struct Model createModels(struct ModelBuilder builder, struct GraphicsSetup *vulkan) {
    struct Model result = { 0 };

    createStorageBuffer(builder.instanceCount * sizeof(struct instanceBuffer), result.graphics.uniformModel.buffers, result.graphics.uniformModel.buffersMemory, result.graphics.uniformModel.buffersMapped, vulkan->device, vulkan->physicalDevice, vulkan->surface);
    result.instanceBuffer = malloc(builder.instanceCount * sizeof(struct instanceBuffer));

    result.instanceCount = builder.instanceCount;
    result.instance = malloc(sizeof(struct instance) * result.instanceCount);
    for (uint32_t i = 0; i < result.instanceCount; i += 1) {
        result.instanceBuffer[i] = (struct instanceBuffer) {
            .modelMatrix = { { 0 } },
            .textureIndex = 0
        };
    }

    result.texturePointer = builder.texturePointer;
    result.texturesQuantity = builder.texturesQuantity;

    result.actualModel = builder.modelPath;

    result.graphics.object.descriptorPool = createObjectDescriptorPool(vulkan->device);
    createDescriptorSets(result.graphics.object.descriptorSets, vulkan->device, result.graphics.object.descriptorPool, builder.objectLayout);
    bindObjectBuffersToDescriptorSets(result.graphics.object.descriptorSets, vulkan->device, result, result.actualModel);

    return result;
}

static void destroyModels(VkDevice device, struct Model model) {
    free(model.instance);

    destroyStorageBuffer(device, model.graphics.uniformModel.buffers, model.graphics.uniformModel.buffersMemory);

    vkDestroyDescriptorPool(device, model.graphics.object.descriptorPool, NULL);
}

void destroyModelArray(uint16_t num, struct Model modelArray[num], struct GraphicsSetup *graphics) {
    vkDeviceWaitIdle(graphics->device);

    for (uint16_t i = 0; i < num; i += 1) {
        destroyModels(graphics->device, modelArray[i]);
    }
}
