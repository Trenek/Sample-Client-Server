#ifndef ENTITY_H
#define ENTITY_H

#include <vulkan/vulkan.h>

#include "descriptor.h"
#include "buffer.h"
#include "entityBuilder.h"
#include "vulkan/vulkan_core.h"

struct Entity {
    VkDevice device;
    uint32_t instanceCount;
    void *instance;
    void (*instanceUpdater)(void *instancePtr, void *instanceBufferPtr, uint32_t instanceCount, float deltaTime);

    void **buffer;

    size_t bufferSize;
    void *(**mapp)[MAX_FRAMES_IN_FLIGHT];
    size_t *range;

    uint32_t meshQuantity;
    struct Mesh *mesh;

    size_t qBuff;

    struct buffer uniformModel;
    struct descriptor object;

    void *additional;
    void (*cleanup)(void *);
};

struct GraphicsSetup;
struct EntityBuilder;

struct Entity *createEntity(struct EntityBuilder modelBuilder, struct GraphicsSetup *vulkan);
void destroyEntity(void *modelPtr);
void destroyEntityArray(uint16_t num, struct Entity *modelArray[num], struct GraphicsSetup *graphics);

#endif
