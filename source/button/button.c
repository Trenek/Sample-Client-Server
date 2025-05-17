#include <cglm.h>
#include <GLFW/glfw3.h>

#include "button.h"

#include "graphicsSetup.h"
#include "windowManager.h"

#include "entity.h"
#include "actualModel.h"
#include "cameraBufferObject.h"
#include "instanceBuffer.h"

#include "Vertex.h"

#define MAX(x, y) ((x) > (y) ? (x) : (y)) 
#define MIN(x, y) ((y) > (x) ? (x) : (y))
void shadowButton(struct GraphicsSetup gs, struct WindowManager wm, struct Button *button) {
    vec3 p; {
        double pp[2];

        glfwGetCursorPos(wm.window, pp, pp + 1);

        p[0] = 2 * pp[0] / gs.swapChain.extent.width - 1;
        p[1] = 2 * pp[1] / gs.swapChain.extent.height - 1;
    }

    struct AnimVertex *temp = button->model->mesh[0].vertices;

    button->chosen = -1;
    for (size_t i = 0; i < button->entity->instanceCount; i += 1) {
        vec3 temp2[4]; {
            mat4 tempMat; {
                glm_mat4_mul(button->camera->proj, ((struct instanceBuffer *)button->entity->buffer[0])[i].modelMatrix, tempMat);
            }
            glm_mat4_mulv3(tempMat, temp[0].pos, 1, temp2[0]);
            glm_mat4_mulv3(tempMat, temp[1].pos, 1, temp2[1]);
            glm_mat4_mulv3(tempMat, temp[2].pos, 1, temp2[2]);
            glm_mat4_mulv3(tempMat, temp[3].pos, 1, temp2[3]);
        }

        float left = MIN(MIN(temp2[0][0], temp2[1][0]), MIN(temp2[2][0], temp2[3][0]));
        float right = MAX(MAX(temp2[0][0], temp2[1][0]), MAX(temp2[2][0], temp2[3][0]));
        float down = MIN(MIN(temp2[0][1], temp2[1][1]), MIN(temp2[2][1], temp2[3][1]));
        float up = MAX(MAX(temp2[0][1], temp2[1][1]), MAX(temp2[2][1], temp2[3][1]));

        if (((struct instance *)button->entity->instance)[i].shadow = p[0] > left && p[0] < right && p[1] > down && p[1] < up) {
            button->chosen = i;
        }
    }
}
