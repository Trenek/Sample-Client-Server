#include <time.h>

#include <GLFW/glfw3.h>
#include <cglm.h>

#include "VulkanTools.h"
#include "uniformBufferObject.h"

struct camera initCamera() {
    struct camera result = {
        .cameraPos = {
            [0] = 4.0f,
            [1] = 0.0f,
            [2] = 4.0f,
        },
        .tilt = {
            [0] = -45.0f,
            [1] =  35.0f
        }
    };

    return result;
}

void moveCamera(struct windowControl *windowControl, GLFWwindow *window, vec3 direction, vec3 cameraPos, vec2 cameraTilt, float deltaTime) {
    mat4 id = {
        [0][0] = 1.0f,
        [1][1] = 1.0f,
        [2][2] = 1.0f,
        [3][3] = 1.0f,
    };
    vec3 initialDirection = {
        -1.0f,
        -1.0f,
        0.0f
    };
    static double prevXPos = 0;
    static double prevYPos = 0;

    if ((KEY_PRESS | KEY_CHANGE) == getKeyState(windowControl, GLFW_KEY_C)) {
        if (GLFW_CURSOR_DISABLED == glfwGetInputMode(window, GLFW_CURSOR)) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else {
            int width = 0;
            int height = 0;

            glfwGetWindowSize(window, &width, &height);

            glfwSetCursorPos(window, width / 2, height / 2);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwGetCursorPos(window, &prevXPos, &prevYPos);
        }
    }

    if (GLFW_CURSOR_DISABLED == glfwGetInputMode(window, GLFW_CURSOR)) {
        double xpos;
        double ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        if (prevXPos == 0 && prevYPos == 0);
        else {
            cameraTilt[0] -= (xpos - prevXPos) * 10 * deltaTime;
            cameraTilt[1] += (ypos - prevYPos) * 10 * deltaTime;
        }
        prevXPos = xpos;
        prevYPos = ypos;
    }


    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT)) cameraTilt[0] += glm_rad(4500) * deltaTime;
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_RIGHT)) cameraTilt[0] -= glm_rad(4500) * deltaTime;
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_UP)) cameraTilt[1] -= glm_rad(4500) * deltaTime;
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_DOWN)) cameraTilt[1] += glm_rad(4500) * deltaTime;

    if (cameraTilt[1] > 80.0f) cameraTilt[1] = 80.0f;
    if (cameraTilt[1] < -80.0f) cameraTilt[1] = -80.0f;

    glm_rotate(id, glm_rad(cameraTilt[0]), (vec3) { 0.0f, 0.0f, 1.0f });
    glm_rotate(id, glm_rad(cameraTilt[1]), (vec3) { -initialDirection[1], initialDirection[0], 0.0f });
    glm_mat4_mulv3(id, initialDirection, 1.0f, direction);

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_W)) {
        cameraPos[0] += direction[0] * 16.0f * deltaTime;
        cameraPos[1] += direction[1] * 16.0f * deltaTime;
    }
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_S)) {
        cameraPos[0] -= direction[0] * 16.0f * deltaTime;
        cameraPos[1] -= direction[1] * 16.0f * deltaTime;
    }
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_A)) {
        cameraPos[0] -= direction[1] * 16.0f * deltaTime;
        cameraPos[1] += direction[0] * 16.0f * deltaTime;
    }
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_D)) {
        cameraPos[0] += direction[1] * 16.0f * deltaTime;
        cameraPos[1] -= direction[0] * 16.0f * deltaTime;
    }

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_SPACE)) {
        cameraPos[2] += 16.0f * deltaTime;
    }
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
        cameraPos[2] -= 16.0f * deltaTime;
    }

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_END)) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}
