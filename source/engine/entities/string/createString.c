#include <string.h>
#include <stdlib.h>

#include "graphicsSetup.h"

#include "stringBuilder.h"
#include "model.h"
#include "modelBuilder.h"

#include "actualModel.h"

struct Model createString(struct StringBuilder builder, struct GraphicsSetup *vulkan) {
    uint32_t meshQuantity = strlen(builder.string);
    struct Mesh *mesh = malloc(sizeof(struct Mesh) * meshQuantity);

    for (uint32_t i = 0; i < meshQuantity; i += 1) {
        mesh[i] = builder.modelData->mesh[builder.string[i] - 'A'];
    }

    return createModels((struct ModelBuilder) {
        .meshQuantity = meshQuantity,
        .mesh = mesh,
        .buffers = &builder.modelData->localMesh.buffers,
        .instanceCount = builder.instanceCount,
        .objectLayout = builder.objectLayout,
        .texturePointer = 0,
        .texturesQuantity = 1
    }, vulkan);
}
