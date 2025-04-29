#ifndef ENTITY_H
#define ENTITY_H

#include <vulkan/vulkan.h>

#include "descriptor.h"
#include "buffer.h"
#include "entityBuilder.h"

struct Entity {
    uint32_t instanceCount;
    void *instance;

    void **buffer;

    size_t bufferSize;
    void *(**mapp)[MAX_FRAMES_IN_FLIGHT];
    size_t *range;

    uint32_t meshQuantity;
    struct Mesh *mesh;

    size_t qBuff;

    struct ModelGraphics {
        struct buffer uniformModel;

        struct descriptor object;
    } graphics;

    void *additional;
    void (*cleanup)(void *);
};

struct GraphicsSetup;
struct EntityBuilder;

struct Entity *createEntity(struct EntityBuilder modelBuilder, struct GraphicsSetup *vulkan);
void destroyEntityArray(uint16_t num, struct Entity *modelArray[num], struct GraphicsSetup *graphics);

#endif
