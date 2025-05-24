#include <GLFW/glfw3.h>
#include <cglm.h>

#include "camera.h"

#include "windowManager.h"

struct camera initCamera() {
    struct camera result = {
        .pos = {
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

void moveCamera(struct WindowManager *windowControl, GLFWwindow *window, struct camera *camera, float deltaTime) {
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

            glfwSetCursorPos(window, 0.5 * width, 0.5 * height);
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
            camera->tilt[0] -= (xpos - prevXPos) * 10 * deltaTime;
            camera->tilt[1] += (ypos - prevYPos) * 10 * deltaTime;
        }
        prevXPos = xpos;
        prevYPos = ypos;
    }


    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT)) camera->tilt[0] += glm_rad(18'000) * deltaTime;
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_RIGHT)) camera->tilt[0] -= glm_rad(18'000) * deltaTime;
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_UP)) camera->tilt[1] -= glm_rad(18'000) * deltaTime;
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_DOWN)) camera->tilt[1] += glm_rad(18'000) * deltaTime;

    if (camera->tilt[1] > 80.0f) camera->tilt[1] = 80.0f;
    if (camera->tilt[1] < -80.0f) camera->tilt[1] = -80.0f;

    glm_rotate(id, glm_rad(camera->tilt[0]), (vec3) { 0.0f, 0.0f, 1.0f });
    glm_rotate(id, glm_rad(camera->tilt[1]), (vec3) { -initialDirection[1], initialDirection[0], 0.0f });
    glm_mat4_mulv3(id, initialDirection, 1.0f, camera->direction);

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_W)) {
        camera->pos[0] += camera->direction[0] * 16.0f * deltaTime;
        camera->pos[1] += camera->direction[1] * 16.0f * deltaTime;
    }
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_S)) {
        camera->pos[0] -= camera->direction[0] * 16.0f * deltaTime;
        camera->pos[1] -= camera->direction[1] * 16.0f * deltaTime;
    }
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_A)) {
        camera->pos[0] -= camera->direction[1] * 16.0f * deltaTime;
        camera->pos[1] += camera->direction[0] * 16.0f * deltaTime;
    }
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_D)) {
        camera->pos[0] += camera->direction[1] * 16.0f * deltaTime;
        camera->pos[1] -= camera->direction[0] * 16.0f * deltaTime;
    }

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_SPACE)) {
        camera->pos[2] += 16.0f * deltaTime;
    }
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
        camera->pos[2] -= 16.0f * deltaTime;
    }
}
