#include <malloc.h>

#include "windowManager.h"

GLFWwindow *createWindow(struct windowData *data);
void destroyWindow(GLFWwindow *window);

struct WindowManager initWindowControl(void) {
    struct WindowManager result = {
        .data = calloc(1, sizeof(struct windowData))
    };

    result.window = createWindow(result.data);

    return result;
}

void cleanupWindowControl(struct WindowManager this) {
    destroyWindow(this.window);

    free(this.data);
}

uint8_t getKeyState(struct WindowManager *windowControl, int key) {
    uint8_t result = (windowControl->data->key[key].press ? KEY_PRESS : KEY_RELEASE) |
                     (windowControl->data->key[key].change ? KEY_CHANGE : KEY_REPEAT);
    windowControl->data->key[key].change = 0;

    return result;
}

uint8_t getMouseState(struct WindowManager *windowControl, int key) {
    uint8_t result = (windowControl->data->mouseButton[key].press ? KEY_PRESS : KEY_RELEASE) |
                     (windowControl->data->mouseButton[key].change ? KEY_CHANGE : KEY_REPEAT);

    windowControl->data->mouseButton[key].change = 0;

    return result;
}

bool shouldWindowClose(struct WindowManager windowControl) {
    if (GLFW_PRESS == glfwGetKey(windowControl.window, GLFW_KEY_END)) {
        glfwSetWindowShouldClose(windowControl.window, GLFW_TRUE);
    }

    return glfwWindowShouldClose(windowControl.window);
}
