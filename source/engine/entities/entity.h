#ifndef ENTITY_H
#define ENTITY_H

#include <vulkan/vulkan.h>

#include "descriptor.h"
#include "buffer.h"
#include "entityBuilder.h"

struct Entity {
    uint32_t instanceCount;
    struct instance *instance;
    struct instanceBuffer *instanceBuffer;

    uint32_t texturePointer;
    uint32_t texturesQuantity;

    uint32_t meshQuantity;
    struct Mesh *mesh;
    VkBuffer (*buffer)[MAX_FRAMES_IN_FLIGHT];

    struct ModelGraphics {
        struct buffer uniformModel;

        struct descriptor object;
    } graphics;

    void *additional;
    void (*cleanup)(void *);
};

struct GraphicsSetup;
struct EntityBuilder;

struct Entity createEntity(struct EntityBuilder modelBuilder, struct GraphicsSetup *vulkan);
void destroyEntityArray(uint16_t num, struct Entity modelArray[num], struct GraphicsSetup *graphics);

#endif
