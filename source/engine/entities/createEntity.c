#include <stdlib.h>
#include <string.h>

#include "graphicsSetup.h"

#include "entity.h"
#include "entityBuilder.h"

struct Entity *createEntity(struct EntityBuilder builder, struct GraphicsSetup *vulkan) {
    struct Entity *result = malloc(sizeof(struct Entity));

    *result = (struct Entity){
        .additional = builder.additional,
        .cleanup = builder.cleanup,

        .instanceCount = builder.instanceCount,
        .instance = malloc(builder.instanceSize * builder.instanceCount),
        .instanceUpdater = builder.instanceUpdater,

        .buffer = malloc(sizeof(void *) * (builder.qBuff + 1)),
        .range = malloc(sizeof(size_t) * (builder.qBuff + 1)),
        .mapp = malloc(sizeof(void *) * (builder.qBuff + 1)),

        .meshQuantity = builder.meshQuantity,
        .mesh = builder.mesh,
        .bufferSize = builder.instanceBufferSize,

        .object.descriptorPool = createObjectDescriptorPool(vulkan->device, builder.qBuff + 1),
        .qBuff = builder.qBuff + 1
    };

    result->buffer[0] = malloc(builder.instanceBufferSize * builder.instanceCount);
    for (size_t i = 0; i < builder.qBuff; i += 1) {
        result->buffer[i + 1] = builder.isChangable[i] ? malloc(builder.range[i]) : NULL;
    }

    VkBuffer (*buff2[builder.qBuff + 1])[MAX_FRAMES_IN_FLIGHT];

    createBuffers(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, builder.instanceCount * builder.instanceBufferSize, result->uniformModel.buffers, result->uniformModel.buffersMemory, result->uniformModel.buffersMapped, vulkan->device, vulkan->physicalDevice, vulkan->surface);

    result->mapp[0] = &result->uniformModel.buffersMapped;
    buff2[0] = &result->uniformModel.buffers;
    result->range[0] = builder.instanceCount * builder.instanceBufferSize;
    memcpy(buff2 + 1, builder.buff, sizeof(void *) * builder.qBuff);
    memcpy(result->range + 1, builder.range, sizeof(size_t) * builder.qBuff);
    memcpy(result->mapp + 1, builder.mapp, sizeof(void *) * builder.qBuff);

    memset(result->buffer[0], 0, builder.instanceBufferSize * builder.instanceCount);

    createDescriptorSets(result->object.descriptorSets, vulkan->device, result->object.descriptorPool, builder.objectLayout);

    bindObjectBuffersToDescriptorSets(result->object.descriptorSets, vulkan->device, builder.qBuff + 1, buff2, result->range);

    return result;
}

static void destroyEntity(VkDevice device, struct Entity model) {
    if (model.cleanup != NULL) {
        model.cleanup(model.additional);
    }

    free(model.instance);

    destroyBuffers(device, model.uniformModel.buffers, model.uniformModel.buffersMemory);

    vkDestroyDescriptorPool(device, model.object.descriptorPool, NULL);
}

void destroyEntityArray(uint16_t num, struct Entity *modelArray[num], struct GraphicsSetup *graphics) {
    vkDeviceWaitIdle(graphics->device);

    for (uint16_t i = 0; i < num; i += 1) {
        destroyEntity(graphics->device, *modelArray[i]);
    }
}
