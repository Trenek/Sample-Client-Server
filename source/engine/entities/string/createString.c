#include <string.h>
#include <stdlib.h>

#include <cglm.h>

#include "graphicsSetup.h"

#include "stringBuilder.h"
#include "model.h"
#include "modelBuilder.h"

#include "actualModel.h"

size_t getGlyphID(char a);

size_t count(const char *buffer) {
    size_t i = 0;

    while (*buffer != 0) {
        i += *buffer != ' ';

        buffer += 1;
    }

    return i;
}

struct Model createString(struct StringBuilder builder, struct GraphicsSetup *vulkan) {
    uint32_t meshQuantity = count(builder.string);
    struct Mesh *mesh = malloc(sizeof(struct Mesh) * meshQuantity);

    struct buffer localMesh;

    createStorageBuffer(meshQuantity * sizeof(mat4), localMesh.buffers, localMesh.buffersMemory, localMesh.buffersMapped, vulkan->device, vulkan->physicalDevice, vulkan->surface);

    mat4 **thisBuffer = (void *)localMesh.buffersMapped;
    mat4 **transform = (void *)builder.modelData->localMesh.buffersMapped;

    uint32_t i = 0;
    const char *buffer = builder.string;
    char aaa;
    mat4 space; {
        glm_mat4_identity(space);
    }
    while (i < meshQuantity) {
        if (*buffer == ' ') {
            glm_mat4_mul(space, transform[0][getGlyphID(' ')], space);
        }
        else {
            mesh[i] = builder.modelData->mesh[getGlyphID(*buffer)];

            for (uint32_t k = 0; k < MAX_FRAMES_IN_FLIGHT; k += 1) {
                if (i == 0) {
                    glm_mat4_identity(thisBuffer[k][i]);
                }
                else { 
                    glm_mat4_mul(thisBuffer[k][i - 1], transform[k][getGlyphID(aaa)], thisBuffer[k][i]);
                    glm_mat4_mul(space, thisBuffer[k][i], thisBuffer[k][i]);
                }

            }

            glm_mat4_identity(space);
            aaa = *buffer;

            i += 1;
        }

        buffer += 1;
    }

    return createModels((struct ModelBuilder) {
        .meshQuantity = meshQuantity,
        .mesh = mesh,
        .buffers = &localMesh.buffers,
        .instanceCount = builder.instanceCount,
        .objectLayout = builder.objectLayout,
        .texturePointer = 0,
        .texturesQuantity = 1
    }, vulkan);
}
