#include "windowManager.h"

#include "MY_ASSERT.h"

static const uint32_t WIDTH = 800;
static const uint32_t HEIGHT = 600;

#define GET(x) ((struct windowData *)glfwGetWindowUserPointer(x))

static void framebufferResizeCallback(GLFWwindow *window, int, int) {
    GET(window)->framebufferResized = true;
}

static void stateCallback(union keyState *state, int action) {
    if (action == GLFW_REPEAT) {
        state->change = 0;
    }
    else {
        state->change = (state->press == 1 && action == GLFW_RELEASE) ||
                        (state->press == 0 && action == GLFW_PRESS);
        state->press = action == GLFW_PRESS ? 1 : 0;
    }
}

static void keyCallback(GLFWwindow *window, int key, int, int action, int) {
    stateCallback(&GET(window)->key[key], action);
}

static void mouseCallback(GLFWwindow *window, int button, int action, int) {
    stateCallback(&GET(window)->mouseButton[button], action);
}

GLFWwindow *createWindow(struct windowData *data) {
    GLFWwindow *result = NULL;
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    result = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", NULL, NULL);

    glfwSetWindowUserPointer(result, data);
    glfwSetFramebufferSizeCallback(result, framebufferResizeCallback);
    glfwSetKeyCallback(result, keyCallback);
    glfwSetMouseButtonCallback(result, mouseCallback);

    return result;
}

void destroyWindow(GLFWwindow *window) {
    glfwDestroyWindow(window);

    glfwTerminate();
}
