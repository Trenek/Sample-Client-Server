#include <string.h>

#include "player.h"
#include "entity.h"
#include "camera.h"

#include "Vertex.h"
#include "instanceBuffer.h"
#include "windowManager.h"
#include "actualModel.h"

bool checkCubeCollision(vec3 cube1[8], vec3 cube2[8]);

static void getWalkDirection(struct WindowManager *wc, int key[4], int joystick, vec2 deltaPos) {
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

    deltaPos[0] = y;
    deltaPos[1] = x;
}

static void stepInterpolation(struct timeFrame *frames, size_t pID, float, float *out) {
    memcpy(out, frames->data[pID].values, sizeof(float[frames->qValues]));
}

static void linearInterpolation(struct timeFrame *frames, size_t pID, float time, float *out) {
    float ratio = (
        (time                       - frames->data[pID].time) / 
        (frames->data[pID + 1].time - frames->data[pID].time)
    );

    for (size_t i = 0; i < frames->qValues; i += 1) {
        out[i] = glm_lerp(frames->data[pID].values[i], frames->data[pID + 1].values[i], ratio);
    }
}

static void cubicInterpolation(struct timeFrame *frames, size_t pID, float, float *out) {
    assert(false);
    memcpy(out, frames->data[pID].values, sizeof(float[frames->qValues]));
}

static void sphericalLinearInterpolation(struct timeFrame *frames, size_t pID, float time, float *out) {
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

        if (data[index].transformation[0].qData) {
            vec4 interpolatedData; {
                interpolateData(&data[index].transformation[0], deltaTime, interpolatedData);
            }

            glm_translate(out, interpolatedData);
        }

        if (data[index].transformation[1].qData) {
            vec4 interpolatedData; {
                interpolateData(&data[index].transformation[1], deltaTime, interpolatedData);
            }

            glm_quat_rotate(out, interpolatedData, out);
        }

        if (data[index].transformation[2].qData) {
            vec4 interpolatedData; {
                interpolateData(&data[index].transformation[2], deltaTime, interpolatedData);
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

static struct camera updateSplitScreenCamera(struct player *p) {
    struct playerInstance *player = p->model->instance;
    struct playerInstance *enemy = p->enemy->model->instance;

    vec2 delta; {
        glm_vec2_sub(player->pos, enemy->pos, delta);
        glm_vec2_normalize(delta);
    }

    return (struct camera) {
        .pos = {
            [0] = player->pos[0] + 2 * delta[0] + 0.5 * delta[1] * p->relativeFaceCameraPos[0],
            [1] = player->pos[1] + 2 * delta[1] - 0.5 * delta[0] * p->relativeFaceCameraPos[0],
            [2] = player->pos[2] + 2.5,
        },

        .direction = {
            [0] = enemy->pos[0],
            [1] = enemy->pos[1],
            [2] = 1
        }
    };
}

static struct camera updateFaceCamera(struct player *p) {
    struct playerInstance *player = p->model->instance;

    return (struct camera) {
        .pos = {
            [0] = player->pos[0] + p->relativeFaceCameraPos[0] / 2,
            [1] = player->pos[1] + p->relativeFaceCameraPos[1] / 2,
            [2] = player->pos[2] + p->relativeFaceCameraPos[2] + 0.15,
        },

        .direction = {
            [0] = player->pos[0],
            [1] = player->pos[1],
            [2] = player->pos[2] + 2.25
        }
    };
}

static void getPlayerDisplacement(struct player *p, struct WindowManager *wc, vec2 displacement) {
    struct playerInstance *player = p->model->instance;
    struct playerInstance *enemy = p->enemy->model->instance;

    vec2 direction; {
        getWalkDirection(wc, p->playerKeys, p->playerJoystick, direction);
    }
    vec2 delta;

    glm_vec2_sub(enemy->pos, player->pos, delta);
    glm_vec2_normalize(delta);

    displacement[0] = + direction[1] * delta[1] - direction[0] * delta[0];
    displacement[1] = - direction[1] * delta[0] - direction[0] * delta[1];
}

static struct colisionBox *find(size_t q, struct colisionBox s[q], const char *name) {
    size_t i = 0;

    while (i < q && 0 != strcmp(s[i].name, name)) i += 1;

    return i == q ? NULL : &s[i];
}

static void applyTransformations(struct colisionBox cB, struct Entity *model, vec3 *out) {
    struct AnimVertex **vert = (void *)cB.vertex;
    struct playerInstanceBuffer *ins = model->buffer[0];
    mat4 *mat = model->buffer[2];

    for (size_t i = 0; i < cB.qVertex; i += 1) {
        glm_mat4_mulv3(mat[vert[i]->bone[0]], vert[i]->pos, 1, out[i]);
        glm_mat4_mulv3(ins->modelMatrix, out[i], 1, out[i]);
    }
}

void add(size_t q, vec3 arr[q], vec3 toAdd) {
    for (size_t i = 0; i < q; i += 1) {
        glm_vec3_add(arr[i], toAdd, arr[i]);
    }
}

static bool checkCubesColision(struct colisionBox cB1, struct Entity *model1, struct colisionBox cB2, struct Entity *model2, vec3 toAdd) {
    vec3 transformed1[cB1.qVertex]; {
        applyTransformations(cB1, model1, transformed1);
        add(cB1.qVertex, transformed1, toAdd);
    }
    vec3 transformed2[cB2.qVertex]; {
        applyTransformations(cB2, model2, transformed2);
    }

    bool result = false;

    for (size_t i = 0; result == false && i < cB1.qVertex - 4; i += 4) {
        for (size_t j = 0; result == false && j < cB2.qVertex - 4; j += 4) {
            result = checkCubeCollision(transformed1 + i, transformed2 + j);
        }
    }

    return result;
}

static bool checkForColisionToAdd(struct player *p, const char *name, vec3 toAdd) {
    struct colisionBox *a = find(p->actualModel->qHitbox, p->actualModel->hitBox, name); {
        if (a == NULL) a = find(p->actualModel->qHurtBox, p->actualModel->hurtBox, name); 
        assert(a != NULL);
    }
    bool result = false;

    for (size_t i = 0; result == false && i < p->enemy->actualModel->qHurtBox; i += 1) {
        result = checkCubesColision(
            *a, p->model, 
            p->enemy->actualModel->hurtBox[i], p->enemy->model,
            toAdd
        );
    }

    return result;
}

static bool checkForColision(struct player *p, const char *name) {
    return checkForColisionToAdd(p, name, (vec3){});
}

void movePlayer(struct player *p, struct WindowManager *wc, float deltaTime) {
    struct playerInstance *player = p->model->instance;
    struct playerInstance *enemy = p->enemy->model->instance;

    vec2 displacement; {
        getPlayerDisplacement(p, wc, displacement);
    }

    int wal = 
        (KEY_PRESS & getKeyState(wc, p->playerKeys[4])) ? LEFT_HIGH_PUNCH :
        (KEY_PRESS & getKeyState(wc, p->playerKeys[5])) ? RIGHT_HIGH_PUNCH :
        (KEY_PRESS & getKeyState(wc, p->playerKeys[6])) ? LEFT_LOW_PUNCH :
        (KEY_PRESS & getKeyState(wc, p->playerKeys[7])) ? RIGHT_LOW_PUNCH :
        (KEY_PRESS & getKeyState(wc, p->playerKeys[8])) ? LEFT_KICK :
        (KEY_PRESS & getKeyState(wc, p->playerKeys[9])) ? RIGHT_KICK :
        glm_vec2_norm(displacement) > 0.1               ? BATTLE_WALK :
        p->state == DAMAGE_HIT                          ? DAMAGE_HIT :
        p->state == DAMAGE_KICK                         ? DAMAGE_KICK :
                                                          STANDING;

    if (wal != p->state) {
        p->state = wal;
        p->time = 0;
    }
    else switch (wal) {
        case BATTLE_WALK:
            if (false == checkForColisionToAdd(p, "HurtBox-Torso", (vec3) {
                deltaTime * displacement[0],
                deltaTime * displacement[1],
                0
            })) {
                player->pos[0] += deltaTime * displacement[0];
                player->pos[1] += deltaTime * displacement[1];
            }
            else if (false == checkForColisionToAdd(p, "HurtBox-Torso", (vec3) {
                deltaTime * displacement[0],
                0,
                0
            })) {
                player->pos[0] += deltaTime * displacement[0];
            }
            else if (false == checkForColisionToAdd(p, "HurtBox-Torso", (vec3) {
                0,
                deltaTime * displacement[1],
                0
            })) {
                player->pos[1] += deltaTime * displacement[1];
            }
            player->fixedRotation[1] = M_PI + atan2f(-displacement[0], displacement[1]);

            p->time += glm_vec2_norm(displacement) * deltaTime;
            break;
        case LEFT_HIGH_PUNCH:
            if (checkForColision(p, "Hitbox-Hand.l")) {
                p->enemy->state = DAMAGE_HIT;
            } 
            p->time += deltaTime;
            break;
        case RIGHT_HIGH_PUNCH:
            if (checkForColision(p, "Hitbox-Hand.r")) {
                p->enemy->state = DAMAGE_HIT;
            }
            p->time += deltaTime;
            break;
        case LEFT_LOW_PUNCH:
            if (checkForColision(p, "Hitbox-Hand.l")) {
                p->enemy->state = DAMAGE_KICK;
            }
            p->time += deltaTime;
            break;
        case RIGHT_LOW_PUNCH:
            if (checkForColision(p, "Hitbox-Hand.r")) {
                p->enemy->state = DAMAGE_KICK;
            }
            p->time += deltaTime;
            break;
        case LEFT_KICK:
            if (
                checkForColision(p, "Hitbox-Leg1.l") ||
                checkForColision(p, "Hitbox-Leg2.l")
            ) {
                p->enemy->state = DAMAGE_KICK;
            }
            p->time += deltaTime;
            break;
        case RIGHT_KICK:
            if (
                checkForColision(p, "Hitbox-Leg1.r") ||
                checkForColision(p, "Hitbox-Leg2.r")
            ) {
                p->enemy->state = DAMAGE_KICK;
            }
            p->time += deltaTime;
            break;
        case DAMAGE_KICK:
            if (checkForColision(p, "HurtBox-Torso") ||
                checkForColision(p, "Hurtbox-Head")) {
                vec2 delta; {
                    glm_vec2_sub(player->pos, enemy->pos, delta);
                    glm_vec2_normalize(delta);
                    glm_vec2_scale(delta, 0.01, delta);
                }
                glm_vec2_add(delta, player->pos, player->pos);
                player->pos[2] += 0.01;
            }
            else {
                if (player->pos[2] > 0) {
                    player->pos[2] -= 0.01;
                }
                else player->pos[2] = 0;
            }
            p->time += deltaTime;
            break;
        default:
            p->time += deltaTime;
            break;
    }

    *p->splitScreen = updateSplitScreenCamera(p);
    *p->face = updateFaceCamera(p);

    animate(p->model, p->actualModel, p->state, p->time);
}
