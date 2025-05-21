#include "player.h"
#include "entity.h"
#include "camera.h"

#include "instanceBuffer.h"

struct camera updateSplitScreenCamera(struct player *p) {
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

struct camera updateFaceCamera(struct player *p) {
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

