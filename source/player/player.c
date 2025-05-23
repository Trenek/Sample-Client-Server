#include "state.h"
#include "player.h"
#include "entity.h"
#include "camera.h"

#include "instanceBuffer.h"
#include "windowManager.h"
#include "actualModel.h"

bool checkForColisionToAdd(struct player *p, const char *name, vec3 toAdd);
bool checkForColision(struct player *p, const char *name);

void animate(struct Entity *model, struct actualModel *actualModel, size_t animID, float deltaTime);

struct camera updateSplitScreenCamera(struct player *p);
struct camera updateFaceCamera(struct player *p);

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

static void getPlayerDisplacement(struct player *p, struct WindowManager *wc, vec2 displacement) {
    struct playerInstance *player = p->model->instance;
    struct playerInstance *enemy = p->enemy->model->instance;

    vec2 direction; {
        getWalkDirection(wc, p->playerKeys, p->playerJoystick, direction);
    }
    vec2 delta; {
        glm_vec2_sub(enemy->pos, player->pos, delta);
        glm_vec2_normalize(delta);
    }

    displacement[0] = + direction[1] * delta[1] - direction[0] * delta[0];
    displacement[1] = - direction[1] * delta[0] - direction[0] * delta[1];
}

#define MAX(x, y) ((x) > (y) ? (x) : (y)) 
#define MIN(x, y) ((x) < (y) ? (x) : (y)) 

static void hitLock(struct player *p, float deltaTime) {
    struct jointData (*data)[p->actualModel->qJoint] = p->actualModel->anim;
    struct timeFrame *frame = data[p->state][0].transformation;
    size_t qData = data[p->state][0].transformation[0].qData;

    struct {
        const char **hitBoxes;
        size_t qHitBoxes;
        
        enum animation enemyState;
        int damage;
    } names[] = {
        [LEFT_HIGH_PUNCH] = {
            .hitBoxes = (const char *[]) {
                "Hitbox-Hand.l"
            },
            .qHitBoxes = 1,
            .enemyState = DAMAGE_HIT,
            .damage = 10
        },
        [RIGHT_HIGH_PUNCH] = {
            .hitBoxes = (const char *[]) {
                "Hitbox-Hand.r"
            },
            .qHitBoxes = 1,
            .enemyState = DAMAGE_HIT,
            .damage = 10
        },
        [LEFT_LOW_PUNCH] = {
            .hitBoxes = (const char *[]) {
                "Hitbox-Hand.l"
            },
            .qHitBoxes = 1,
            .enemyState = DAMAGE_KICK,
            .damage = 10
        },
        [RIGHT_LOW_PUNCH] = {
            .hitBoxes = (const char *[]) {
                "Hitbox-Hand.r"
            },
            .qHitBoxes = 1,
            .enemyState = DAMAGE_KICK,
            .damage = 10
        },
        [LEFT_KICK] = {
            .hitBoxes = (const char *[]) {
                "Hitbox-Leg1.l",
                "Hitbox-Leg2.l"
            },
            .qHitBoxes = 2,
            .enemyState = DAMAGE_KICK,
            .damage = 10
        },
        [RIGHT_KICK] = {
            (const char *[]) {
                "Hitbox-Leg1.r",
                "Hitbox-Leg2.r"
            },
            .qHitBoxes = 2,
            .enemyState = DAMAGE_KICK,
            .damage = 10
        }
    };

    if (p->hitTime < frame[0].data[qData - 1].time) {
        bool didColide = false;

        for (size_t i = 0; false == didColide && i < names[p->state].qHitBoxes; i += 1) {
            didColide = checkForColision(p, names[p->state].hitBoxes[i]);
        }

        if (didColide && p->didHit == false) {
            p->enemy->state = names[p->state].enemyState;
            p->enemy->currentHealth -= names[p->state].damage;
            p->didHit = didColide;
            p->enemy->hurtLock = true;
            p->enemy->hurtTime = 1;
        }
        p->hitTime += deltaTime;
        p->time = MIN(p->hitTime, frame[0].data[qData - 1].time - 0.1);
    }
    else if (p->hitTime < frame[0].data[qData - 1].time + 1) {
        p->hitLock = false;
        p->doesHitLast = true;
        p->hitTime += deltaTime;
        p->time = frame[0].data[qData - 1].time - 0.1;
    }
    else {
        p->hitLock = false;
        p->doesHitLast = false;
        p->didHit = false;
        p->enemy->hurtLock = false;
        p->hitTime = 0;
    }
}

static void hurtLock(struct player *p, float deltaTime) {
    struct playerInstance *player = p->model->instance;
    struct playerInstance *enemy = p->enemy->model->instance;

    if (checkForColision(p, "HurtBox-Torso") ||
        checkForColision(p, "Hurtbox-Head")) {
        vec2 delta; {
            glm_vec2_sub(player->pos, enemy->pos, delta);
            glm_vec2_normalize(delta);
            glm_vec2_scale(delta, 0.01, delta);
        }
        glm_vec2_add(delta, player->pos, player->pos);
        player->pos[2] += deltaTime * 3;
    }
    p->time += deltaTime;
    p->hurtTime -= deltaTime;
}

void move(struct player *p, float deltaTime, vec2 dis) {
    struct playerInstance *player = p->model->instance;

    if (false == checkForColisionToAdd(p, "HurtBox-Torso", (vec3) {
        deltaTime * dis[0],
        deltaTime * dis[1],
        0
    })) {
        player->pos[0] += deltaTime * dis[0];
        player->pos[1] += deltaTime * dis[1];
    }
    else if (false == checkForColisionToAdd(p, "HurtBox-Torso", (vec3) {
        deltaTime * dis[0],
        0,
        0
    })) {
        player->pos[0] += deltaTime * dis[0];
    }
    else if (false == checkForColisionToAdd(p, "HurtBox-Torso", (vec3) {
        0,
        deltaTime * dis[1],
        0
    })) {
        player->pos[1] += deltaTime * dis[1];
    }
    player->fixedRotation[1] = M_PI + atan2f(-dis[0], dis[1]);

    p->time += glm_vec2_norm(dis) * deltaTime;
}

static void newMove(struct player *p, float deltaTime, int wal, vec2 dis) {
    if (wal != p->state) {
        p->state = wal;
        p->time = 0;
        switch (wal) {
            case WALK:
                move(p, deltaTime, dis);
                break;
            case LEFT_HIGH_PUNCH:
            case RIGHT_HIGH_PUNCH:
            case LEFT_LOW_PUNCH:
            case RIGHT_LOW_PUNCH:
            case LEFT_KICK:
            case RIGHT_KICK:
                if (p->currentRest > 2000) {
                    p->hitLock = true;
                    p->currentRest -= 2000;
                }
                else {
                    p->state = STANDING;
                }
                [[fallthrough]];
            case DAMAGE_KICK:
            case DAMAGE_HIT:
                p->time = 0;
                break;
            default:
                p->time = 0;
                break;
        }
    }
    else switch (wal) {
        case WALK:
            move(p, deltaTime, dis);
            break;
        case CHARGE:
            if (p->currentRest < p->maxRest) {
                p->currentRest += deltaTime * 5000;
                if (p->currentRest > p->maxRest) {
                    p->currentRest = p->maxRest;
                }
            }
            p->time += deltaTime;
            break;
        default:
            p->time += deltaTime;
            break;
    }
}

int getState(struct player *p, struct WindowManager *wc, vec2 displacement, enum state *eState) {
    int resState = 0;
    getPlayerDisplacement(p, wc, displacement);

    GLFWgamepadstate state;
    if (glfwJoystickIsGamepad(p->playerJoystick) && glfwGetGamepadState(p->playerJoystick, &state)) {
        if (state.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS) {
            *eState = PAUSE;
        }
        resState = (
            state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] > 0.1 ? (
                GLFW_PRESS == state.buttons[GLFW_GAMEPAD_BUTTON_X] ? LEFT_LOW_PUNCH :
                GLFW_PRESS == state.buttons[GLFW_GAMEPAD_BUTTON_Y] ? LEFT_HIGH_PUNCH :
                GLFW_PRESS == state.buttons[GLFW_GAMEPAD_BUTTON_B] ? LEFT_KICK :
                glm_vec2_norm(displacement) > 0.4                  ? WALK :
                                                                     STANDING 
            ) :
            state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] > 0.1 ? (
                GLFW_PRESS == state.buttons[GLFW_GAMEPAD_BUTTON_X] ? RIGHT_LOW_PUNCH :
                GLFW_PRESS == state.buttons[GLFW_GAMEPAD_BUTTON_Y] ? RIGHT_HIGH_PUNCH :
                GLFW_PRESS == state.buttons[GLFW_GAMEPAD_BUTTON_B] ? RIGHT_KICK :
                glm_vec2_norm(displacement) > 0.4                  ? WALK :
                                                                     STANDING 
            ) :
            GLFW_PRESS == state.buttons[GLFW_GAMEPAD_BUTTON_Y] ? CHARGE :
            glm_vec2_norm(displacement) > 0.4                  ? WALK :
                                                                 STANDING 
        );

        if (GLFW_PRESS == state.buttons[GLFW_GAMEPAD_BUTTON_A] && resState == WALK) {
            resState = FAST_WALK;
        }
    }
    else {
        resState = (
            (KEY_PRESS & getKeyState(wc, p->playerKeys[8])) > 0.1 ? (
                (KEY_PRESS & getKeyState(wc, p->playerKeys[4]))   ? LEFT_LOW_PUNCH :
                (KEY_PRESS & getKeyState(wc, p->playerKeys[5]))   ? LEFT_HIGH_PUNCH :
                (KEY_PRESS & getKeyState(wc, p->playerKeys[7]))   ? LEFT_KICK :
                glm_vec2_norm(displacement) > 0.4                 ? WALK :
                                                                    STANDING 
            ) :
            (KEY_PRESS & getKeyState(wc, p->playerKeys[9])) > 0.1 ? (
                (KEY_PRESS & getKeyState(wc, p->playerKeys[4]))   ? RIGHT_LOW_PUNCH :
                (KEY_PRESS & getKeyState(wc, p->playerKeys[5]))   ? RIGHT_HIGH_PUNCH :
                (KEY_PRESS & getKeyState(wc, p->playerKeys[7]))   ? RIGHT_KICK :
                glm_vec2_norm(displacement) > 0.4                 ? WALK :
                                                                    STANDING 
            ) :
            (KEY_PRESS & getKeyState(wc, p->playerKeys[5]))       ? CHARGE :
            glm_vec2_norm(displacement) > 0.4                     ? WALK :
                                                                    STANDING 
        );

        if ((KEY_PRESS & getKeyState(wc, p->playerKeys[6])) && resState == WALK) {
            resState = FAST_WALK;
        }
    }

    return resState;
}

void movePlayer(struct player *p, struct WindowManager *wc, float deltaTime, enum state *state) {
    struct playerInstance *player = p->model->instance;
    vec2 displacement = {};
    int wal = getState(p, wc, displacement, state);

    if (wal == FAST_WALK) {
        wal = WALK;
        if (p->currentRest > 2000 * deltaTime) {
            deltaTime *= 4;
            p->currentRest -= 2000 * deltaTime;
        }
    }

    if (p->hurtLock || p->hurtTime > 0) hurtLock(p, deltaTime);
    else if (p->hitLock) hitLock(p, deltaTime);
    else if (wal != STANDING && p->hitTime != 0) {
        p->hitLock = false;
        p->didHit = false;
        p->enemy->hurtLock = false;
        p->hitTime = 0;
        p->doesHitLast = false;
        p->state = STANDING;

        newMove(p, deltaTime, wal, displacement);
    }
    else if (p->doesHitLast) hitLock(p, deltaTime);
    else {
        p->hitLock = false;
        p->didHit = false;
        p->enemy->hurtLock = false;
        p->hitTime = 0;
        p->doesHitLast = false;

        newMove(p, deltaTime, wal, displacement);
    }

    *p->splitScreen = updateSplitScreenCamera(p);
    *p->face = updateFaceCamera(p);

    *p->healthPercentage = MAX(0, p->currentHealth / (8.0 * p->maxHealth));
    *p->restPercentage = MAX(0, p->currentRest / (8.0 * p->maxRest));

    if (p->currentRest < p->maxRest) {
        p->currentRest += 800 * deltaTime;
    }
    
    if (player->pos[0] < -10) player->pos[0] = -10;
    if (player->pos[0] >  10) player->pos[0] =  10;
    if (player->pos[1] < -10) player->pos[1] = -10;
    if (player->pos[1] >  10) player->pos[1] =  10;
    player->pos[2] = (player->pos[2] > 0) ? player->pos[2] - deltaTime : 0;

    animate(p->model, p->actualModel, p->state, p->time);
}

void posePlayer(struct player *p, float deltaTime) {
    p->time += deltaTime;

    animate(p->model, p->actualModel, p->state, p->time);
}
