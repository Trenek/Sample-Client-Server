#include <vulkan/vulkan.h>
#include <string.h>
#include <stddef.h>

#include "renderPass.h"

#include "graphicsSetup.h"
#include "definitions.h"

#include "buffer.h"
#include "descriptor.h"
#include "cameraBufferObject.h"

struct renderPass createRenderPassObj(struct renderPassBuilder builder, struct GraphicsSetup *vulkan) {
    struct renderPass result = {
        .data = builder.data,
        .qData = builder.qData,
        .cameraDescriptorPool = createCameraDescriptorPool(vulkan->device),
        .cameraDescriptorSetLayout = createCameraDescriptorSetLayout(vulkan->device),
        .updateCameraBuffer = builder.updateCameraBuffer
    };

    memcpy(result.coordinates, builder.coordinates, sizeof(double[4]));

    createBuffers(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(struct CameraBuffer), result.cameraBuffer, result.cameraBufferMemory, result.cameraBufferMapped, vulkan->device, vulkan->physicalDevice, vulkan->surface);

    createDescriptorSets(result.cameraDescriptorSet, vulkan->device, result.cameraDescriptorPool, result.cameraDescriptorSetLayout);
    bindCameraBuffersToDescriptorSets(result.cameraDescriptorSet, vulkan->device, result.cameraBuffer);

    return result;
}

void destroyRenderPassObj(size_t qRenderPass, struct renderPass *renderPass, struct GraphicsSetup *vulkan) {
    for (size_t i = 0; i < qRenderPass; i += 1) {
        vkDestroyDescriptorPool(vulkan->device, renderPass[i].cameraDescriptorPool, NULL);
        vkDestroyDescriptorSetLayout(vulkan->device, renderPass[i].cameraDescriptorSetLayout, NULL);
        destroyBuffers(vulkan->device, renderPass[i].cameraBuffer, renderPass[i].cameraBufferMemory);
    }
}
