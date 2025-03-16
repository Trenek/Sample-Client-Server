#include "GLFW/glfw3.h"

#include "graphicsFunctions.h"
#include "graphicsSetup.h"

#include "definitions.h"
#include "descriptor.h"

static void cleanupSwapChain(struct GraphicsSetup *vulkan) {
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        vkDestroySemaphore(vulkan->device, vulkan->imageAvailableSemaphore[i], NULL);
        vkDestroySemaphore(vulkan->device, vulkan->renderFinishedSemaphore[i], NULL);
        vkDestroyFence(vulkan->device, vulkan->inFlightFence[i], NULL);
    }

    vkDestroyImageView(vulkan->device, vulkan->colorImageView, NULL);
    vkDestroyImage(vulkan->device, vulkan->colorImage, NULL);
    vkFreeMemory(vulkan->device, vulkan->colorImageMemory, NULL);

    vkDestroyImageView(vulkan->device, vulkan->depthImageView, NULL);
    vkDestroyImage(vulkan->device, vulkan->depthImage, NULL);
    vkFreeMemory(vulkan->device, vulkan->depthImageMemory, NULL);

    destroyFramebuffers(vulkan->device, vulkan->swapChainFramebuffers, vulkan->swapChain.imagesCount);

    destroyImageViews(vulkan->swapChainImageViews, vulkan->swapChain.imagesCount, vulkan->device);

    vkDestroySwapchainKHR(vulkan->device, vulkan->swapChain.this, NULL);
}

void recreateSwapChainGraphics(GLFWwindow *window, struct GraphicsSetup *vulkan) {
    vkDeviceWaitIdle(vulkan->device);

    cleanupSwapChain(vulkan);

    vulkan->swapChain = createSwapChain(window, vulkan->surface, vulkan->physicalDevice, vulkan->device);
    vulkan->swapChainImageViews = createImageViews(vulkan->device, vulkan->swapChain);

    createColorResources(&vulkan->colorImage, &vulkan->colorImageMemory, &vulkan->colorImageView, vulkan->device, vulkan->physicalDevice, vulkan->swapChain.extent, vulkan->swapChain.imageFormat, vulkan->msaaSamples);
    createDepthResources(&vulkan->depthImage, &vulkan->depthImageMemory, &vulkan->depthImageView, vulkan->device, vulkan->physicalDevice, vulkan->swapChain.extent, vulkan->msaaSamples, vulkan->transferCommandPool, vulkan->transferQueue);

    vulkan->swapChainFramebuffers = createFramebuffers(vulkan->device, vulkan->swapChainImageViews, vulkan->swapChain.imagesCount, vulkan->swapChain.extent, vulkan->renderPass, vulkan->depthImageView, vulkan->colorImageView);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        vulkan->imageAvailableSemaphore[i] = createSemaphore(vulkan->device);
        vulkan->renderFinishedSemaphore[i] = createSemaphore(vulkan->device);
        vulkan->inFlightFence[i] = createFence(vulkan->device);
    }
}

struct GraphicsSetup setupGraphics(GLFWwindow *window) {
    struct GraphicsSetup vulkan = { 0 };

    vulkan.instance = createInstance(&vulkan.debugMessenger);
    vulkan.surface = createSurface(window, vulkan.instance);
    vulkan.physicalDevice = pickPhysicalDevice(&vulkan.msaaSamples, vulkan.instance, vulkan.surface);
    vulkan.device = createLogicalDevice(vulkan.surface, vulkan.physicalDevice, &vulkan.graphicsQueue, &vulkan.presentQueue, &vulkan.transferQueue);
    vulkan.swapChain = createSwapChain(window, vulkan.surface, vulkan.physicalDevice, vulkan.device);
    vulkan.swapChainImageViews = createImageViews(vulkan.device, vulkan.swapChain);

    vulkan.renderPass = createRenderPass(vulkan.device, vulkan.physicalDevice, vulkan.swapChain.imageFormat, vulkan.msaaSamples);

    vulkan.commandPool = createCommandPool(vulkan.device, vulkan.physicalDevice, vulkan.surface);
    createCommandBuffer(vulkan.commandBuffer, vulkan.device, vulkan.commandPool);

    vulkan.transferCommandPool = createTransferCommandPool(vulkan.device, vulkan.physicalDevice, vulkan.surface);

    createColorResources(&vulkan.colorImage, &vulkan.colorImageMemory, &vulkan.colorImageView, vulkan.device, vulkan.physicalDevice, vulkan.swapChain.extent, vulkan.swapChain.imageFormat, vulkan.msaaSamples);
    createDepthResources(&vulkan.depthImage, &vulkan.depthImageMemory, &vulkan.depthImageView, vulkan.device, vulkan.physicalDevice, vulkan.swapChain.extent, vulkan.msaaSamples, vulkan.transferCommandPool, vulkan.transferQueue);
    vulkan.swapChainFramebuffers = createFramebuffers(vulkan.device, vulkan.swapChainImageViews, vulkan.swapChain.imagesCount, vulkan.swapChain.extent, vulkan.renderPass, vulkan.depthImageView, vulkan.colorImageView);

    createUniformBuffers(vulkan.uniformBuffers, vulkan.uniformBuffersMemory, vulkan.uniformBuffersMapped, vulkan.device, vulkan.physicalDevice, vulkan.surface);

    vulkan.cameraDescriptorPool = createCameraDescriptorPool(vulkan.device);
    vulkan.cameraDescriptorSetLayout = createCameraDescriptorSetLayout(vulkan.device);
    createDescriptorSets(vulkan.cameraDescriptorSet, vulkan.device, vulkan.cameraDescriptorPool, vulkan.cameraDescriptorSetLayout);
    bindCameraBuffersToDescriptorSets(vulkan.cameraDescriptorSet, vulkan.device, vulkan.uniformBuffers);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        vulkan.imageAvailableSemaphore[i] = createSemaphore(vulkan.device);
        vulkan.renderFinishedSemaphore[i] = createSemaphore(vulkan.device);
        vulkan.inFlightFence[i] = createFence(vulkan.device);
    }

    return vulkan;
}

void cleanupGraphics(struct GraphicsSetup vulkan) {
    vkDeviceWaitIdle(vulkan.device);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        vkDestroySemaphore(vulkan.device, vulkan.imageAvailableSemaphore[i], NULL);
        vkDestroySemaphore(vulkan.device, vulkan.renderFinishedSemaphore[i], NULL);
        vkDestroyFence(vulkan.device, vulkan.inFlightFence[i], NULL);
    }

    vkDestroyDescriptorPool(vulkan.device, vulkan.cameraDescriptorPool, NULL);
    vkDestroyDescriptorSetLayout(vulkan.device, vulkan.cameraDescriptorSetLayout, NULL);

    vkDestroyImageView(vulkan.device, vulkan.colorImageView, NULL);
    vkDestroyImage(vulkan.device, vulkan.colorImage, NULL);
    vkFreeMemory(vulkan.device, vulkan.colorImageMemory, NULL);

    vkDestroyImageView(vulkan.device, vulkan.depthImageView, NULL);
    vkDestroyImage(vulkan.device, vulkan.depthImage, NULL);
    vkFreeMemory(vulkan.device, vulkan.depthImageMemory, NULL);

    destroyUniformBuffers(vulkan.device, vulkan.uniformBuffers, vulkan.uniformBuffersMemory);

    vkDestroyCommandPool(vulkan.device, vulkan.commandPool, NULL);
    vkDestroyCommandPool(vulkan.device, vulkan.transferCommandPool, NULL);

    destroyFramebuffers(vulkan.device, vulkan.swapChainFramebuffers, vulkan.swapChain.imagesCount);

    vkDestroyRenderPass(vulkan.device, vulkan.renderPass, NULL);

    destroyImageViews(vulkan.swapChainImageViews, vulkan.swapChain.imagesCount, vulkan.device);
    vkDestroySwapchainKHR(vulkan.device, vulkan.swapChain.this, NULL);
    vkDestroyDevice(vulkan.device, NULL);
    DestroyDebugUtilsMessengerEXT(vulkan.instance, vulkan.debugMessenger, NULL);
    vkDestroySurfaceKHR(vulkan.instance, vulkan.surface, NULL);
    vkDestroyInstance(vulkan.instance, NULL);
}
