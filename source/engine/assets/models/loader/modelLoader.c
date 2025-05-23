#include <string.h>

#include "entityBuilder.h"
#include "graphicsSetup.h"
#include "modelLoader.h"

#include "bufferOperations.h"

#include "MY_ASSERT.h"

struct actualModel *loadModel(const char *filePath, struct GraphicsSetup *vulkan) {
    struct actualModel *result = calloc(1, sizeof(struct actualModel));

    void (*fun)(const char *, struct actualModel *, VkDevice, VkPhysicalDevice, VkSurfaceKHR) =
        NULL != strstr(filePath, ".obj") ? objLoadModel : 
        NULL != strstr(filePath, ".ttf") ? ttfLoadModel :
        NULL != strstr(filePath, ".gltf") ? gltfLoadModel :
        NULL != strstr(filePath, ".glb") ? gltfLoadModel :
        NULL;
    assert(NULL != fun);

    fun(filePath, result, vulkan->device, vulkan->physicalDevice, vulkan->surface);

    for (uint32_t i = 0; i < result->meshQuantity; i += 1) {
        result->device = vulkan->device;
        result->mesh[i].vertexBuffer = createVertexBuffer(&result->mesh[i].vertexBufferMemory, vulkan->device, vulkan->physicalDevice, vulkan->surface, vulkan->commandPool, vulkan->transferQueue, result->mesh[i].verticesQuantity, result->mesh[i].vertices, result->mesh[i].sizeOfVertex);
        result->mesh[i].indexBuffer = createIndexBuffer(&result->mesh[i].indexBufferMemory, vulkan->device, vulkan->physicalDevice, vulkan->surface, vulkan->commandPool, vulkan->transferQueue, result->mesh[i].verticesQuantity, result->mesh[i].indicesQuantity, result->mesh[i].indices, result->mesh[i].sizeOfVertex);
    }

    return result;
}

void loadModels(size_t quantity, struct actualModel *model[quantity], const char *modelPath[quantity], struct GraphicsSetup *vulkan) {
    for (size_t i = 0; i < quantity; i += 1) {
        model[i] = loadModel(modelPath[i], vulkan);
    }
}

void freeTransformation(struct timeFrame transformation) {
    if (transformation.data != NULL) {
        free(transformation.data->values);
    }

    free(transformation.data);
}

void freeAnimations(size_t qAnim, size_t qJoints, struct jointData animation[qAnim][qJoints]) {
    for (size_t i = 0; i < qAnim; i += 1) {
        for (size_t j = 0; j < qJoints; j += 1) {
            freeTransformation(animation[i][j].transformation[0]);
            freeTransformation(animation[i][j].transformation[1]);
            freeTransformation(animation[i][j].transformation[2]);
        }
    }

    free(animation);
}

void cleanupColisionBox(size_t qBox, struct colisionBox *box) {
    for (uint32_t i = 0; i < qBox; i += 1) {
        free(box[i].name);
        free(box[i].vertex);
    }
    free(box);
}

void destroyActualModel(void *modelPtr) {
    struct actualModel *model = modelPtr;

    for (uint32_t j = 0; j < model->meshQuantity; j += 1) {
        destroyBuffer(model->device, model->mesh[j].indexBuffer, model->mesh[j].indexBufferMemory);
        destroyBuffer(model->device, model->mesh[j].vertexBuffer, model->mesh[j].vertexBufferMemory);
    }

    for (uint32_t j = 0; j < model->meshQuantity; j += 1) {
        free(model->mesh[j].vertices);
        free(model->mesh[j].indices);
    }
    free(model->mesh);

    freeAnimations(model->qAnim, model->qJoint, model->anim);

    cleanupColisionBox(model->qHitbox, model->hitBox);
    cleanupColisionBox(model->qHurtBox, model->hurtBox);

    destroyBuffers(model->device, model->localMesh.buffers, model->localMesh.buffersMemory);

    free(model);
}

void destroyActualModels(uint32_t modelQuantity, struct actualModel *model[modelQuantity]) {
    for (uint32_t i = 0; i < modelQuantity; i += 1) {
        destroyActualModel(model[i]);
    }
}
