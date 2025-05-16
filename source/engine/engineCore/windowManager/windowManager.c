#include <malloc.h>

#include "windowManager.h"

GLFWwindow *createWindow(struct windowData *data);
void destroyWindow(GLFWwindow *window);

struct WindowManager initWindowManager(void) {
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

static uint8_t updateKeyState(union keyState *state) {
    uint8_t result = (state->press ? KEY_PRESS : KEY_RELEASE) |
                     (state->change ? KEY_CHANGE : KEY_REPEAT);
    state->change = 0;

    return result;
}

uint8_t getKeyState(struct WindowManager *windowControl, int key) {
    return updateKeyState(&windowControl->data->key[key]);
}

uint8_t getMouseState(struct WindowManager *windowControl, int key) {
    return updateKeyState(&windowControl->data->mouseButton[key]);
}

bool shouldWindowClose(struct WindowManager windowControl) {
    if (GLFW_PRESS == glfwGetKey(windowControl.window, GLFW_KEY_END)) {
        glfwSetWindowShouldClose(windowControl.window, GLFW_TRUE);
    }

    return glfwWindowShouldClose(windowControl.window);
}
