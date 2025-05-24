#include "engineCore.h"
#include "renderPassCore.h"

void recreateSwapChain(struct EngineCore *vulkan, uint16_t qRenderPassCore, struct renderPassCore **renderPassCore) {
    int width = 0;
    int height = 0;

    glfwGetFramebufferSize(vulkan->window.window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(vulkan->window.window, &width, &height);
        glfwWaitEvents();
    }
    
    vkDeviceWaitIdle(vulkan->graphics.device);

    recreateSwapChainGraphics(vulkan->window.window, &vulkan->graphics);
    for (uint16_t i = 0; i < qRenderPassCore; i += 1) {
        recreateRenderPassCore(renderPassCore[i], &vulkan->graphics);
    }
}

struct EngineCore setup() {
    struct EngineCore vulkan = {
        .window = initWindowManager(),
        .soundManager = initSoundManager(),
        .deltaTime = initDeltaTimeManager(),
        .resource = {}
    };

    vulkan.graphics = setupGraphics(vulkan.window.window);

    return vulkan;
}

void cleanup(struct EngineCore vulkan) {
    cleanupResourcesOrg(&vulkan.resource);

    cleanupGraphics(vulkan.graphics);
    cleanupSoundManager(vulkan.soundManager);
    cleanupWindowControl(vulkan.window);
}
