#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "MY_ASSERT.h"

VkSurfaceKHR createSurface(GLFWwindow *window, VkInstance instance) {
    VkSurfaceKHR surface = NULL;

    MY_ASSERT(VK_SUCCESS == glfwCreateWindowSurface(instance, window, NULL, &surface));

    return surface;
}
