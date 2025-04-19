#include "graphicsSetup.h"

#include "modelBuilder.h"

#include "actualModel.h"

struct Entity createModel(struct ModelBuilder builder, struct GraphicsSetup *vulkan) {
    return createEntity((struct EntityBuilder) {
        .instanceCount = builder.instanceCount,
        .texturesQuantity = 1,
        .texturePointer = builder.texturePointer,
        .meshQuantity = builder.modelData->meshQuantity,
        .mesh = builder.modelData->mesh,
        .buffers = &builder.modelData->localMesh.buffers,
        .objectLayout = builder.objectLayout,

        .additional = NULL,
        .cleanup = NULL,
    }, vulkan);
}
