#include <stdlib.h>

#include "renderPassCore.h"
#include "graphicsSetup.h"

VkFramebuffer *createFramebuffers(VkDevice device, VkImageView *swapChainImageViews, uint32_t swapChainImagesCount, VkExtent2D swapChainExtent, VkRenderPass renderPass, VkImageView depthImageView, VkImageView colorImageView);
void destroyFramebuffers(VkDevice device, VkFramebuffer *swapChainFramebuffers, uint32_t swapChainImagesCount);
VkRenderPass createRenderPass(VkDevice device, VkPhysicalDevice physicalDevice, VkFormat swapChainImageFormat, VkSampleCountFlagBits msaaSamples, int loadOp, int initLayout);

struct renderPassCore *createRenderPassCore(struct renderPassCoreBuilder builder, struct GraphicsSetup *vulkan) {
    struct renderPassCore *result = malloc(sizeof(struct renderPassCore));

    result->device = vulkan->device;
    result->swapChain = &vulkan->swapChain;
    result->renderPass = createRenderPass(vulkan->device, vulkan->physicalDevice, vulkan->swapChain.imageFormat, vulkan->msaaSamples, builder.loadOp, builder.initLayout);
    result->swapChainFramebuffers = createFramebuffers(vulkan->device, vulkan->swapChainImageViews, vulkan->swapChain.imagesCount, vulkan->swapChain.extent, result->renderPass, vulkan->depthImageView, vulkan->colorImageView);

    return result;
}

void freeRenderPassCore(void *thisPtr) {
    struct renderPassCore *this = thisPtr;

    destroyFramebuffers(this->device, this->swapChainFramebuffers, this->swapChain->imagesCount);
    vkDestroyRenderPass(this->device, this->renderPass, NULL);

    free(this);
}

void recreateRenderPassCore(struct renderPassCore *this, struct GraphicsSetup *vulkan) {
    destroyFramebuffers(this->device, this->swapChainFramebuffers, this->swapChain->imagesCount);

    this->swapChainFramebuffers = createFramebuffers(vulkan->device, vulkan->swapChainImageViews, vulkan->swapChain.imagesCount, vulkan->swapChain.extent, this->renderPass, vulkan->depthImageView, vulkan->colorImageView);
}
