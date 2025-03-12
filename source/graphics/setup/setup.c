#include <stdio.h>

#include "VulkanTools.h"

#include "graphicsSetup.h"
#include "window.h"

void recreateSwapChain(struct VulkanTools *vulkan) {
    int width = 0;
    int height = 0;

    glfwGetFramebufferSize(vulkan->window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(vulkan->window, &width, &height);
        glfwWaitEvents();
    }

    recreateSwapChainGraphics(vulkan->window, &vulkan->graphics);
}

struct VulkanTools setup() {
    struct VulkanTools vulkan = { 0 };

    vulkan.window = createWindow(&vulkan.framebufferResized, &vulkan.windowControl);
    vulkan.graphics = setupGraphics(vulkan.window);
    vulkan.deltaTime = initDeltaTime();

    return vulkan;
}

void cleanup(struct VulkanTools vulkan) {
    cleanupGraphics(vulkan.graphics);
    destroyWindow(vulkan.window);
}
