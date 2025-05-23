#include <stdlib.h>
#include <string.h>

#include "graphicsSetup.h"

#include "entity.h"
#include "entityBuilder.h"

struct Entity *createEntity(struct EntityBuilder builder, struct GraphicsSetup *vulkan) {
    struct Entity *result = calloc(1, sizeof(struct Entity));

    *result = (struct Entity){
        .device = vulkan->device,
        .additional = builder.additional,
        .cleanup = builder.cleanup,

        .instanceCount = builder.instanceCount,
        .instance = malloc(builder.instanceSize * builder.instanceCount),
        .instanceUpdater = builder.instanceUpdater,

        .buffer = calloc(builder.qBuff + 1, sizeof(void *)),
        .range = calloc(builder.qBuff + 1, sizeof(size_t)),
        .mapp = calloc(builder.qBuff + 1, sizeof(void *)),

        .meshQuantity = builder.meshQuantity,
        .mesh = builder.mesh,
        .bufferSize = builder.instanceBufferSize,

        .object.descriptorPool = createObjectDescriptorPool(vulkan->device, builder.qBuff + 1),
        .qBuff = builder.qBuff + 1
    };

    result->buffer[0] = calloc(builder.instanceCount, builder.instanceBufferSize);
    for (size_t i = 0; i < builder.qBuff; i += 1) {
        result->buffer[i + 1] = builder.isChangable[i] ? calloc(1, builder.range[i]) : NULL;
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

void destroyEntity(void *modelPtr) {
    struct Entity *model = modelPtr;

    if (model->cleanup != NULL) {
        model->cleanup(model->additional);
    }

    for (size_t i = 0; i < model->qBuff; i += 1) {
        free(model->buffer[i]);
    }

    free(model->instance);
    free(model->buffer);
    free(model->range);
    free(model->mapp);

    destroyBuffers(model->device, model->uniformModel.buffers, model->uniformModel.buffersMemory);

    vkDestroyDescriptorPool(model->device, model->object.descriptorPool, NULL);

    free(model);
}

void destroyEntityArray(uint16_t num, struct Entity *modelArray[num], struct GraphicsSetup *graphics) {
    vkDeviceWaitIdle(graphics->device);

    for (uint16_t i = 0; i < num; i += 1) {
        destroyEntity(modelArray[i]);
    }
}
