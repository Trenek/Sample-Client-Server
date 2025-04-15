#ifndef MODEL_H
#define MODEL_H

#include <vulkan/vulkan.h>

#include "descriptor.h"
#include "buffer.h"
#include "modelBuilder.h"

struct Model {
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
struct ModelBuilder;

struct Model createModels(struct ModelBuilder modelBuilder, struct GraphicsSetup *vulkan);
void destroyModelArray(uint16_t num, struct Model modelArray[num], struct GraphicsSetup *graphics);

#endif
