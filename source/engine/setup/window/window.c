#include <GLFW/glfw3.h>

#include "windowControl.h"

#include "MY_ASSERT.h"

static const uint32_t WIDTH = 800;
static const uint32_t HEIGHT = 600;

static bool framebufferResized = false;
struct windowControl windowControl;

static void framebufferResizeCallback(GLFWwindow* window, [[maybe_unused]] int width, [[maybe_unused]] int height) {
    *(bool *)glfwGetWindowUserPointer(window) = true;
}

static void keyCallback([[maybe_unused]] GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
    if (action == GLFW_REPEAT) {
        windowControl.key[key].change = 0;
    }
    else {
        windowControl.key[key].change = (windowControl.key[key].press == 1 && action == GLFW_RELEASE) ||
                                        (windowControl.key[key].press == 0 && action == GLFW_PRESS);
        windowControl.key[key].press = action == GLFW_PRESS ? 1 : 0;
    }
}

static void mouseCallback([[maybe_unused]] GLFWwindow* window, int button, int action, [[maybe_unused]] int mods) {
    if (action == GLFW_REPEAT) {
        windowControl.mouseButton[button].change = 0;
    }
    else {
        windowControl.mouseButton[button].change = (windowControl.mouseButton[button].press == 1 && action == GLFW_RELEASE) ||
                                                   (windowControl.mouseButton[button].press == 0 && action == GLFW_PRESS);
        windowControl.mouseButton[button].press = action == GLFW_PRESS ? 1 : 0;
    }
}

GLFWwindow *createWindow(bool **framebufferResizedPtr, struct windowControl **pWindowControl) {
    GLFWwindow *result = NULL;
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    result = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", NULL, NULL);

    glfwSetWindowUserPointer(result, &framebufferResized);
    glfwSetFramebufferSizeCallback(result, framebufferResizeCallback);
    *framebufferResizedPtr = &framebufferResized;
    *pWindowControl = &windowControl;
    glfwSetKeyCallback(result, keyCallback);
    glfwSetMouseButtonCallback(result, mouseCallback);

    return result;
}

void destroyWindow(GLFWwindow *window) {
    glfwDestroyWindow(window);

    glfwTerminate();
}
