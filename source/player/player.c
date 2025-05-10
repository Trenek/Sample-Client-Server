#include <string.h>
#include <assert.h>

#include "player.h"
#include "GLFW/glfw3.h"
#include "entity.h"

#include "instanceBuffer.h"
#include "windowControl.h"
#include "actualModel.h"

static void walk(struct windowControl *wc, int key[4], int joystick, vec2 deltaPos) {
    bool isUpClicked = KEY_PRESS & getKeyState(wc, key[0]);
    bool isLeftClicked = KEY_PRESS & getKeyState(wc, key[1]);
    bool isDownClicked = KEY_PRESS & getKeyState(wc, key[2]);
    bool isRightClicked = KEY_PRESS & getKeyState(wc, key[3]);

    float x = 0;
    float y = 0;

    GLFWgamepadstate state;
    if (glfwJoystickIsGamepad(joystick) && glfwGetGamepadState(joystick, &state)) {
        x = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
        y = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
    }
    else {
        int ly = isUpClicked - isDownClicked;
        int lx = isLeftClicked - isRightClicked;

        x = ((lx == -1) - (lx == 1));
        y = ((ly == -1) - (ly == 1));

        if (x != 0 && y != 0) {
            x /= sqrt(2);
            y /= sqrt(2);
        }
    }

    if (sqrtf(x * x + y * y) > 0.1) {
        deltaPos[0] = y;
        deltaPos[1] = x;
    }
}

void stepInterpolation(struct timeFrame *frames, size_t pID, float, float *out) {
    memcpy(out, frames->data[pID].values, sizeof(float[frames->qValues]));
}

void linearInterpolation(struct timeFrame *frames, size_t pID, float time, float *out) {
    float ratio = (
        (time                       - frames->data[pID].time) / 
        (frames->data[pID + 1].time - frames->data[pID].time)
    );

    for (size_t i = 0; i < frames->qValues; i += 1) {
        out[i] = glm_lerp(frames->data[pID].values[i], frames->data[pID + 1].values[i], ratio);
    }
}

void cubicInterpolation(struct timeFrame *frames, size_t pID, float, float *out) {
    assert(false);
    memcpy(out, frames->data[pID].values, sizeof(float[frames->qValues]));
}

void sphericalLinearInterpolation(struct timeFrame *frames, size_t pID, float time, float *out) {
    vec4 prevQuat; {
        glm_vec4_dup(frames->data[pID].values, prevQuat);
        glm_quat_normalize(prevQuat);
    }
    vec4 nextQuat; {
        glm_vec4_dup(frames->data[pID + 1].values, nextQuat);
        glm_quat_normalize(nextQuat);
    }
    float dotProduct = glm_quat_dot(prevQuat, nextQuat);
    float ratio = (
        (time                       - frames->data[pID].time) / 
        (frames->data[pID + 1].time - frames->data[pID].time)
    );
    assert(ratio >= 0 && ratio <= 1);

    if (dotProduct < 0.0) {
        glm_vec4_scale(nextQuat, -1, nextQuat);
        dotProduct = -dotProduct;
    }

    if (dotProduct > 1 - 10e-6) {
        linearInterpolation(frames, pID, time, out);
    }
    else {
        float theta_0 = acos(dotProduct);
        float theta = ratio * theta_0;

        float scaleNextQuat = sin(theta) / sin(theta_0);
        float scalePreviousQuat = cos(theta) - dotProduct * scaleNextQuat;

        glm_vec4_scale(prevQuat, scalePreviousQuat, prevQuat);
        glm_vec4_scale(nextQuat, scaleNextQuat, nextQuat);
        glm_vec4_add(prevQuat, nextQuat, out);
    }
}

static void interpolateData(struct timeFrame *frames, float deltaTime, float *data) {
    size_t qData = frames->qData;
    float maxTime = frames->data[qData - 1].time;
    float time = maxTime == 0.0f ? 0.0f : fmodf(deltaTime, maxTime);

    void (*interpolation[])(struct timeFrame *, size_t, float, vec4) = {
        linearInterpolation,
        stepInterpolation,
        cubicInterpolation,
        sphericalLinearInterpolation,
    };

    if (time < frames->data[0].time) {
        memcpy(data, frames->data[0].values, sizeof(float[frames->qValues]));
    }
    else {
        size_t i = 0; {
            while (frames->data[i + 1].time < time) i += 1;
        }

        interpolation[frames->interpolationType](frames, i, time, data);
    }
}

static void calculateJointData(size_t index, struct jointData *data, mat4 out, float deltaTime) {
    glm_mat4_identity(out);

    if (data[index].isJoint) {
        if (data[index].father >= 0) {
            calculateJointData(data[index].father, data, out, deltaTime);
        }

        if (data[index].t[0].qData) {
            vec4 interpolatedData; {
                interpolateData(&data[index].t[0], deltaTime, interpolatedData);
            }

            glm_translate(out, interpolatedData);
        }

        if (data[index].t[1].qData) {
            vec4 interpolatedData; {
                interpolateData(&data[index].t[1], deltaTime, interpolatedData);
            }

            glm_quat_rotate(out, interpolatedData, out);
        }

        if (data[index].t[2].qData) {
            vec4 interpolatedData; {
                interpolateData(&data[index].t[2], deltaTime, interpolatedData);
            }

            glm_scale(out, interpolatedData);
        }
    }
}

static void animate(struct Entity *model, struct actualModel *actualModel, size_t animID, float deltaTime) {
    struct jointData (*data)[actualModel->qJoint] = actualModel->anim;
    mat4 *mat = model->buffer[2];

    for (uint32_t i = 0; i < actualModel->qJoint; i += 1) {
        calculateJointData(i, data[animID], mat[i], deltaTime);
        glm_mat4_mul(mat[i], data[animID][i].inverseMatrix, mat[i]);
    }
}

void movePlayer(struct player *p, struct windowControl *wc, float deltaTime) {
    int wal = STANDING;

    vec2 deltaPos = { 0 };
    walk(wc, p->playerKeys, p->playerJoystick, deltaPos);
    struct playerInstance *instance = p->model->instance;
    struct playerInstance *enemy = p->enemy->model->instance;
    vec2 delta;
    glm_vec2_sub(enemy->pos, instance->pos, delta);
    glm_vec2_normalize(delta);

    vec2 a = {
        + deltaPos[1] * delta[1] - deltaPos[0] * delta[0],
        - deltaPos[1] * delta[0] - deltaPos[0] * delta[1]
    };

    if (sqrtf(a[0] * a[0] + a[1] * a[1]) > 0.1) {
        wal = BATTLE_WALK;
        instance->pos[0] += deltaTime * a[0];
        instance->pos[1] += deltaTime * a[1];
        instance->fixedRotation[1] = M_PI + atan2f(-a[0], a[1]);
    }

    p->time += deltaTime;
    wal = 
        (KEY_PRESS & getKeyState(wc, GLFW_KEY_1)) ? LEFT_HIGH_PUNCH :
        (KEY_PRESS & getKeyState(wc, GLFW_KEY_2)) ? RIGHT_HIGH_PUNCH :
        (KEY_PRESS & getKeyState(wc, GLFW_KEY_3)) ? LEFT_LOW_PUNCH :
        (KEY_PRESS & getKeyState(wc, GLFW_KEY_4)) ? RIGHT_LOW_PUNCH : wal;

    if (wal != p->state) {
        p->state = wal;
        p->time = 0;
    }

    animate(p->model, p->actualModel, p->state, p->time);
}
