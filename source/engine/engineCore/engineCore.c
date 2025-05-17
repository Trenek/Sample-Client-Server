#include "engineCore.h"

void recreateSwapChain(struct EngineCore *vulkan) {
    int width = 0;
    int height = 0;

    glfwGetFramebufferSize(vulkan->window.window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(vulkan->window.window, &width, &height);
        glfwWaitEvents();
    }

    recreateSwapChainGraphics(vulkan->window.window, &vulkan->graphics);
}

struct EngineCore setup() {
    struct EngineCore vulkan = {
        .window = initWindowManager(),
        .soundManager = initSoundManager(),
        .deltaTime = initDeltaTimeManager()
    };

    vulkan.graphics = setupGraphics(vulkan.window.window);

    return vulkan;
}

void cleanup(struct EngineCore vulkan) {
    cleanupGraphics(vulkan.graphics);
    cleanupSoundManager(vulkan.soundManager);
    cleanupWindowControl(vulkan.window);
}
