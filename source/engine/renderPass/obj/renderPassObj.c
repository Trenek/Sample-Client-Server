#include <vulkan/vulkan.h>
#include <string.h>
#include <stddef.h>

#include "renderPassObj.h"

#include "graphicsSetup.h"
#include "definitions.h"

#include "buffer.h"
#include "descriptor.h"
#include "cameraBufferObject.h"
#include "graphicsPipelineObj.h"

struct renderPassObj *createRenderPassObj(struct renderPassBuilder builder, struct GraphicsSetup *vulkan) {
    struct renderPassObj *result = calloc(1, sizeof(struct renderPassObj));
    *result = (struct renderPassObj){
        .device = vulkan->device,
        .renderPass = builder.renderPass,
        .data = malloc(sizeof(struct pipelineConnection) * builder.qData),
        .qData = builder.qData,
        .cameraDescriptorPool = createCameraDescriptorPool(vulkan->device),
        .cameraDescriptorSetLayout = createCameraDescriptorSetLayout(vulkan->device),
        .updateCameraBuffer = builder.updateCameraBuffer
    };

    memcpy(result->data, builder.data, sizeof(struct pipelineConnection) * builder.qData);
    for (size_t i = 0; i < result->qData; i += 1) {
        while (result->renderPass != result->data[i].pipe->pipeline[result->data[i].pipeNum].core) result->data[i].pipeNum += 1;

        result->data[i].entity = malloc(sizeof(struct Entity *) * result->data[i].qEntity);
        memcpy(result->data[i].entity, builder.data[i].entity, sizeof(struct Entity *) * result->data[i].qEntity);
    }
    memcpy(result->coordinates, builder.coordinates, sizeof(double[4]));

    createBuffers(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(struct CameraBuffer), result->cameraBuffer, result->cameraBufferMemory, result->cameraBufferMapped, vulkan->device, vulkan->physicalDevice, vulkan->surface);

    createDescriptorSets(result->cameraDescriptorSet, vulkan->device, result->cameraDescriptorPool, result->cameraDescriptorSetLayout);
    bindCameraBuffersToDescriptorSets(result->cameraDescriptorSet, vulkan->device, result->cameraBuffer);

    return result;
}

void destroyRenderPassObj(void *renderPassPtr) {
    struct renderPassObj *renderPass = renderPassPtr;
    vkDestroyDescriptorPool(renderPass->device, renderPass->cameraDescriptorPool, NULL);
    vkDestroyDescriptorSetLayout(renderPass->device, renderPass->cameraDescriptorSetLayout, NULL);
    destroyBuffers(renderPass->device, renderPass->cameraBuffer, renderPass->cameraBufferMemory);

    for (size_t i = 0; i < renderPass->qData; i += 1) {
        free(renderPass->data[i].entity);
    }
    free(renderPass->data);
    free(renderPass);
}

void destroyRenderPassObjArr(size_t qRenderPass, struct renderPassObj **renderPass) {
    for (size_t i = 0; i < qRenderPass; i += 1) {
        destroyRenderPassObj(renderPass[i]);
    }
}
