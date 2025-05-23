#include <openGJK.h>
#include <assert.h>
#include <string.h>

#include "player.h"
#include "entity.h"

#include "Vertex.h"
#include "instanceBuffer.h"
#include "actualModel.h"

typedef float vec3[3];

static bool checkCubeCollision(vec3 shape3d1[8], vec3 shape3d2[8]) {
    gkPolytope p1 = {
        .numpoints = 8,
        .coord = (gkFloat *[]) {
            (gkFloat[3]) { 0, 0, 0 },
            (gkFloat[3]) { 0, 0, 0 },
            (gkFloat[3]) { 0, 0, 0 },
            (gkFloat[3]) { 0, 0, 0 },
            (gkFloat[3]) { 0, 0, 0 },
            (gkFloat[3]) { 0, 0, 0 },
            (gkFloat[3]) { 0, 0, 0 },
            (gkFloat[3]) { 0, 0, 0 }
        }
    };

    gkPolytope p2 = {
        .numpoints = 8,
        .coord = (gkFloat *[]) {
            (gkFloat[3]) { 0, 0, 0 },
            (gkFloat[3]) { 0, 0, 0 },
            (gkFloat[3]) { 0, 0, 0 },
            (gkFloat[3]) { 0, 0, 0 },
            (gkFloat[3]) { 0, 0, 0 },
            (gkFloat[3]) { 0, 0, 0 },
            (gkFloat[3]) { 0, 0, 0 },
            (gkFloat[3]) { 0, 0, 0 }
        }
    };

    gkSimplex s = {
        .nvrtx = 0
    };

    for (int i = 0; i < 8; i++) {
        p1.coord[i][0] = shape3d1[i][0];
        p1.coord[i][1] = shape3d1[i][1];
        p1.coord[i][2] = shape3d1[i][2];

        p2.coord[i][0] = shape3d2[i][0];
        p2.coord[i][1] = shape3d2[i][1];
        p2.coord[i][2] = shape3d2[i][2];
    }

    gkFloat distance = compute_minimum_distance(p1, p2, &s);

    return distance <= 0.0;
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

static void add(size_t q, vec3 arr[q], vec3 toAdd) {
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

bool checkForColisionToAdd(struct player *p, const char *name, vec3 toAdd) {
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

bool checkForColision(struct player *p, const char *name) {
    return checkForColisionToAdd(p, name, (vec3){});
}
