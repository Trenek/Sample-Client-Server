#include <string.h>
#include <stdlib.h>

#include <cglm.h>

#include "affine-pre.h"
#include "graphicsSetup.h"

#include "mat4.h"
#include "stringBuilder.h"
#include "entity.h"
#include "entityBuilder.h"

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

    destroyBuffers(toClean->device, toClean->localMesh.buffers, toClean->localMesh.buffersMemory);

    free(toClean->mesh);

    free(toClean);
}

struct Entity *createString(struct StringBuilder builder, struct GraphicsSetup *vulkan) {
    uint32_t meshQuantity = count(builder.string);

    struct toCleanup *info = malloc(sizeof(struct toCleanup));
    info->device = vulkan->device;
    info->mesh = malloc(sizeof(struct Mesh) * meshQuantity);

    createBuffers(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, meshQuantity * sizeof(mat4), info->localMesh.buffers, info->localMesh.buffersMemory, info->localMesh.buffersMapped, vulkan->device, vulkan->physicalDevice, vulkan->surface);

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
    for (uint32_t k = 0; k < MAX_FRAMES_IN_FLIGHT; k += 1) {
        mat4 length;
        glm_mat4_mul(thisBuffer[k][meshQuantity - 1], transform[k][getGlyphID(prev)], length);
        for (uint32_t j = 0; j < meshQuantity; j += 1) {
            mat4 temp;
            glm_mat4_identity(temp);
            glm_translate(temp, (vec4) { -length[3][0] * 0.5 * (builder.center + 1), 0, 0, 1 });
            glm_mat4_mul(thisBuffer[k][j], temp, thisBuffer[k][j]);
        }
    }

    VkBuffer (*buff[])[MAX_FRAMES_IN_FLIGHT] = {
        &info->localMesh.buffers
    };

    bool isChangable[] = {
        false,
    };

    void *(*mapp[])[MAX_FRAMES_IN_FLIGHT] = {
        &info->localMesh.buffersMapped,
    };

    size_t range[] = {
        meshQuantity * sizeof(mat4),
    };

    return createEntity((struct EntityBuilder) {
        .meshQuantity = meshQuantity,
        .mesh = info->mesh,

        .buff = buff,
        .mapp = mapp,
        .isChangable = isChangable,
        .range = range,
        .qBuff = 1,

        .instanceCount = builder.instanceCount,
        .objectLayout = builder.objectLayout,
        .instanceSize = builder.instanceSize,
        .instanceBufferSize = builder.instanceBufferSize,
        .instanceUpdater = builder.instanceUpdater,

        .additional = info,
        .cleanup = cleanupFont
    }, vulkan);
}
