#ifndef RENDER_PASS_H
#define RENDER_PASS_H

#include <vulkan/vulkan.h>

#include "definitions.h"
#include "camera.h"

struct Entity;
struct graphicsPipeline;
struct GraphicsSetup;

struct pipelineConnection {
    struct graphicsPipeline *pipe;
    struct descriptor *texture;
    struct Entity **entity;
    size_t qEntity;
};

struct renderPassBuilder {
    double coordinates[4];
    
    void (*updateCameraBuffer)(void *buffersMapped, VkExtent2D swapChainExtent, vec3 cameraPos, vec3 direction);

    struct pipelineConnection *data;
    size_t qData;
};

struct renderPass {
    VkRenderPass renderPass;
    VkFramebuffer *swapChainFramebuffers;
};

struct renderPassObj {
    VkDevice device;
    double coordinates[4];
    
    struct renderPass *renderPass;
    struct camera camera;
    void (*updateCameraBuffer)(void *buffersMapped, VkExtent2D swapChainExtent, vec3 cameraPos, vec3 direction);

    struct pipelineConnection *data;
    size_t qData;

    VkBuffer cameraBuffer[MAX_FRAMES_IN_FLIGHT];
    VkDeviceMemory cameraBufferMemory[MAX_FRAMES_IN_FLIGHT];
    void *cameraBufferMapped[MAX_FRAMES_IN_FLIGHT];

    VkDescriptorSetLayout cameraDescriptorSetLayout;
    VkDescriptorPool cameraDescriptorPool;
    VkDescriptorSet cameraDescriptorSet[MAX_FRAMES_IN_FLIGHT];
};

struct renderPassObj *createRenderPassObj(struct renderPassBuilder builder, struct GraphicsSetup *vulkan);
void destroyRenderPassObj(void *renderPassPtr);
void destroyRenderPassObjArr(size_t qRenderPass, struct renderPassObj **renderPass);

#endif
