#ifndef ENTITY_BUILDER
#define ENTITY_BUILDER
#include <vulkan/vulkan.h>

#include "definitions.h"

struct actualModel;

struct EntityBuilder {
    uint32_t instanceCount;
    void (*instanceUpdater)(void *instancePtr, void *instanceBufferPtr, uint32_t instanceCount, float deltaTime);

    uint32_t meshQuantity;
    struct Mesh *mesh;

    VkBuffer (**buff)[MAX_FRAMES_IN_FLIGHT];
    void *(**mapp)[MAX_FRAMES_IN_FLIGHT];
    bool *isChangable;
    size_t *range;
    size_t qBuff;

    VkDescriptorSetLayout objectLayout;

    size_t instanceSize;
    size_t instanceBufferSize;

    void *additional;
    void (*cleanup)(void *);
};

#endif
