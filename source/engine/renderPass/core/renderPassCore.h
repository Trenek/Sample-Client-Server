#ifndef RENDER_PASS_CORE_H
#define RENDER_PASS_CORE_H

#include <vulkan/vulkan.h>

#include "definitions.h"

struct GraphicsSetup;

struct renderPassCoreBuilder {
    int loadOp;
    int initLayout;
};

struct renderPassCore {
    VkDevice device;
    struct swapChain *swapChain;

    VkRenderPass renderPass;
    VkFramebuffer *swapChainFramebuffers;
};

struct renderPassCore *createRenderPassCore(struct renderPassCoreBuilder builder, struct GraphicsSetup *vulkan);
void freeRenderPassCore(void *thisPtr);

void recreateRenderPassCore(struct renderPassCore *this, struct GraphicsSetup *vulkan);

#endif
