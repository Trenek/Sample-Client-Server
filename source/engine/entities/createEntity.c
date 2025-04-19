#include "graphicsSetup.h"

#include "entityBuilder.h"
#include "instanceBuffer.h"

#include "actualModel.h"

struct Entity createEntity(struct EntityBuilder builder, struct GraphicsSetup *vulkan) {
    struct Entity result = {
        .additional = builder.additional,
        .cleanup = builder.cleanup
    };

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

    result.meshQuantity = builder.meshQuantity;
    result.mesh = builder.mesh;
    result.buffer = builder.buffers;

    result.graphics.object.descriptorPool = createObjectDescriptorPool(vulkan->device);
    createDescriptorSets(result.graphics.object.descriptorSets, vulkan->device, result.graphics.object.descriptorPool, builder.objectLayout);
    bindObjectBuffersToDescriptorSets(result.graphics.object.descriptorSets, vulkan->device, result, result.meshQuantity, builder.buffers);

    return result;
}

static void destroyEntity(VkDevice device, struct Entity model) {
    if (model.cleanup != NULL) {
        model.cleanup(model.additional);
    }

    free(model.instance);

    destroyStorageBuffer(device, model.graphics.uniformModel.buffers, model.graphics.uniformModel.buffersMemory);

    vkDestroyDescriptorPool(device, model.graphics.object.descriptorPool, NULL);
}

void destroyEntityArray(uint16_t num, struct Entity modelArray[num], struct GraphicsSetup *graphics) {
    vkDeviceWaitIdle(graphics->device);

    for (uint16_t i = 0; i < num; i += 1) {
        destroyEntity(graphics->device, modelArray[i]);
    }
}
