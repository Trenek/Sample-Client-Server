#include <string.h>

#include "graphicsSetup.h"
#include "modelLoader.h"

#include "bufferOperations.h"

void loadModel(const char *filePath, struct actualModel *model, struct GraphicsSetup *vulkan) {
    void (*fun)(const char *, struct actualModel *, VkDevice, VkPhysicalDevice, VkSurfaceKHR) =
        NULL != strstr(filePath, ".obj") ? objLoadModel : 
        NULL != strstr(filePath, ".ttf") ? ttfLoadModel :
        NULL != strstr(filePath, ".gltf") ? gltfLoadModel :
        NULL != strstr(filePath, ".glb") ? gltfLoadModel :
        NULL;
    assert(NULL != fun);

    fun(filePath, model, vulkan->device, vulkan->physicalDevice, vulkan->surface);

    for (uint32_t i = 0; i < model->meshQuantity; i += 1) {
        model->mesh[i].vertexBuffer = createVertexBuffer(&model->mesh[i].vertexBufferMemory, vulkan->device, vulkan->physicalDevice, vulkan->surface, vulkan->commandPool, vulkan->transferQueue, model->mesh[i].verticesQuantity, model->mesh[i].vertices);
        model->mesh[i].indexBuffer = createIndexBuffer(&model->mesh[i].indexBufferMemory, vulkan->device, vulkan->physicalDevice, vulkan->surface, vulkan->commandPool, vulkan->transferQueue, model->mesh[i].verticesQuantity, model->mesh[i].indicesQuantity, model->mesh[i].indices);
    }
}

void loadModels(size_t quantity, struct actualModel model[quantity], const char *modelPath[quantity], struct GraphicsSetup *vulkan) {
    for (size_t i = 0; i < quantity; i += 1) {
        loadModel(modelPath[i], &model[i], vulkan);
    }
}

static void unloadMesh(struct Vertex *vertices, uint16_t *indices) {
    free(vertices);
    free(indices);
}

void destroyActualModels(VkDevice device, uint32_t modelQuantity, struct actualModel *model) {
    for (uint32_t i = 0; i < modelQuantity; i += 1) {
        for (uint32_t j = 0; j < model[i].meshQuantity; j += 1) {
            destroyBuffer(device, model[i].mesh[j].indexBuffer, model[i].mesh[j].indexBufferMemory);
            destroyBuffer(device, model[i].mesh[j].vertexBuffer, model[i].mesh[j].vertexBufferMemory);
        }

        for (uint32_t j = 0; j < model[i].meshQuantity; j += 1) {
            unloadMesh(model[i].mesh[j].vertices, model[i].mesh[j].indices);
        }
        free(model[i].mesh);

        destroyStorageBuffer(device, model[i].localMesh.buffers, model[i].localMesh.buffersMemory);
    }
}

