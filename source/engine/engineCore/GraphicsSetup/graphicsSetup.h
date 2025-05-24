#ifndef GRAPHICS_SETUP_H
#define GRAPHICS_SETUP_H

#include <vulkan/vulkan.h>

#include "definitions.h"
#include "swapChain.h"

struct GraphicsSetup {
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    VkQueue presentQueue;
    VkQueue graphicsQueue;
    VkQueue transferQueue;
    VkDevice device;

    struct swapChain swapChain;
    VkImageView *swapChainImageViews;

    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer[MAX_FRAMES_IN_FLIGHT];
    VkCommandPool transferCommandPool;

    VkSemaphore imageAvailableSemaphore[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore renderFinishedSemaphore[MAX_FRAMES_IN_FLIGHT];
    VkFence inFlightFence[MAX_FRAMES_IN_FLIGHT];

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    VkSampleCountFlagBits msaaSamples;

    VkImage colorImage;
    VkDeviceMemory colorImageMemory;
    VkImageView colorImageView;
};

typedef struct GLFWwindow GLFWwindow;

void recreateSwapChainGraphics(GLFWwindow *window, struct GraphicsSetup *vulkan);
struct GraphicsSetup setupGraphics(GLFWwindow *window);
void cleanupGraphics(struct GraphicsSetup vulkan);

#endif
