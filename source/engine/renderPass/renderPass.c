#include "renderPass.h"

#include <vulkan/vulkan.h>
#include <string.h>
#include <stddef.h>

#include "GraphicsSetup.h"
#include "definitions.h"

void createUniformBuffers(VkBuffer uniformBuffers[], VkDeviceMemory uniformBuffersMemory[], void *uniformBuffersMapped[], VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
void destroyUniformBuffers(VkDevice device, VkBuffer uniformBuffers[], VkDeviceMemory uniformBuffersMemory[]);

struct renderPass createRenderPassObj(struct renderPassBuilder builder, struct GraphicsSetup *vulkan) {
    struct renderPass result = {
        .data = builder.data,
        .qData = builder.qData,
        .cameraDescriptorPool = createCameraDescriptorPool(vulkan->device),
        .cameraDescriptorSetLayout = createCameraDescriptorSetLayout(vulkan->device),
        .camera = builder.camera,
    };

    memcpy(result.p, builder.p, sizeof(double[4]));

    createUniformBuffers(result.cameraBuffer, result.cameraBufferMemory, result.cameraBufferMapped, vulkan->device, vulkan->physicalDevice, vulkan->surface);

    createDescriptorSets(result.cameraDescriptorSet, vulkan->device, result.cameraDescriptorPool, result.cameraDescriptorSetLayout);
    bindCameraBuffersToDescriptorSets(result.cameraDescriptorSet, vulkan->device, result.cameraBuffer);

    return result;
}

void destroyRenderPassObj(size_t qRenderPass, struct renderPass *renderPass, struct GraphicsSetup *vulkan) {
    for (size_t i = 0; i < qRenderPass; i += 1) {
        vkDestroyDescriptorPool(vulkan->device, renderPass[i].cameraDescriptorPool, NULL);
        vkDestroyDescriptorSetLayout(vulkan->device, renderPass[i].cameraDescriptorSetLayout, NULL);
        destroyUniformBuffers(vulkan->device, renderPass[i].cameraBuffer, renderPass[i].cameraBufferMemory);
    }
}
