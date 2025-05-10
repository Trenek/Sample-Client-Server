#ifndef VULKAN_TOOLS
#define VULKAN_TOOLS

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "entity.h"
#include "definitions.h"

#include "windowControl.h"
#include "deltaTime.h"

#include "graphicsSetup.h"
#include "camera.h"
#include "graphicsPipelineObj.h"
#include "renderPass.h"

struct VulkanTools {
    GLFWwindow *window;
    struct windowControl *windowControl;
    struct deltaTimeManager deltaTime;

    bool *framebufferResized;

    struct GraphicsSetup graphics;
};

struct VulkanTools setup();
void recreateSwapChain(struct VulkanTools *vulkan);
void cleanup(struct VulkanTools vulkan);

void drawFrame(struct VulkanTools *vulkan, uint16_t modelQuantity, struct renderPass model[modelQuantity]);

#endif // !VULKAN_TOOLS
