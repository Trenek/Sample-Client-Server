#ifndef VULKAN_TOOLS
#define VULKAN_TOOLS

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "model.h"
#include "definitions.h"

#include "windowControl.h"
#include "deltaTime.h"

#include "graphicsSetup.h"
#include "camera.h"

struct VulkanTools {
    GLFWwindow *window;
    struct windowControl *windowControl;
    struct deltaTimeManager deltaTime;

    bool *framebufferResized;

    struct camera camera;

    struct GraphicsSetup graphics;
};

struct VulkanTools setup();
void recreateSwapChain(struct VulkanTools *vulkan);
void cleanup(struct VulkanTools vulkan);

void drawFrame(struct VulkanTools *vulkan, uint16_t modelQuantity, struct Model model[modelQuantity]);

void getViewProj(struct VulkanTools vulkan, mat4 resultViewProj, vec4 resultViewPort);
void getCoordinate(vec3 result, struct VulkanTools vulkan);

#endif // !VULKAN_TOOLS
