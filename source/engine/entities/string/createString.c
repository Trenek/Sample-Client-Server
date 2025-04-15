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

struct toCleanup {
    VkDevice device;

    struct Mesh *mesh;

    struct buffer localMesh;
};

void cleanupFont(void *toCleanArg) {
    struct toCleanup *toClean = toCleanArg;

    destroyStorageBuffer(toClean->device, toClean->localMesh.buffers, toClean->localMesh.buffersMemory);

    free(toClean->mesh);

    free(toClean);
}

struct Model createString(struct StringBuilder builder, struct GraphicsSetup *vulkan) {
    uint32_t meshQuantity = count(builder.string);

    struct toCleanup *info = malloc(sizeof(struct toCleanup));
    info->device = vulkan->device;
    info->mesh = malloc(sizeof(struct Mesh) * meshQuantity);

    createStorageBuffer(meshQuantity * sizeof(mat4), info->localMesh.buffers, info->localMesh.buffersMemory, info->localMesh.buffersMapped, vulkan->device, vulkan->physicalDevice, vulkan->surface);

    mat4 **thisBuffer = (void *)info->localMesh.buffersMapped;
    mat4 **transform = (void *)builder.modelData->localMesh.buffersMapped;

    uint32_t i = 0;
    const char *buffer = builder.string;
    char prev = 0;
    mat4 space; {
        glm_mat4_identity(space);
    }
    while (i < meshQuantity) {
        if (*buffer == ' ') {
            glm_mat4_mul(space, transform[0][getGlyphID(' ')], space);
        }
        else {
            info->mesh[i] = builder.modelData->mesh[getGlyphID(*buffer)];

            for (uint32_t k = 0; k < MAX_FRAMES_IN_FLIGHT; k += 1) {
                if (i == 0) {
                    glm_mat4_identity(thisBuffer[k][i]);
                }
                else { 
                    glm_mat4_mul(thisBuffer[k][i - 1], transform[k][getGlyphID(prev)], thisBuffer[k][i]);
                    glm_mat4_mul(space, thisBuffer[k][i], thisBuffer[k][i]);
                }
            }

            glm_mat4_identity(space);
            prev = *buffer;

            i += 1;
        }

        buffer += 1;
    }

    return createModels((struct ModelBuilder) {
        .meshQuantity = meshQuantity,
        .mesh = info->mesh,
        .buffers = &info->localMesh.buffers,
        .instanceCount = builder.instanceCount,
        .objectLayout = builder.objectLayout,
        .texturePointer = 0,
        .texturesQuantity = 1,
        .additional = info,
        .cleanup = cleanupFont
    }, vulkan);
}
