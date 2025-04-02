#include <stdint.h>
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include <freetype/freetype.h>
#include <cglm.h>

#include "actualModel.h"
#include "myMalloc.h"

#include "Vertex.h"

#define IF(x, y) if (!(x)) printf("%s", y); else

#define BFR(x) ((struct FontVertex *)(x))

static void lerp_l(vec2 out, FT_Vector from, FT_Vector to, float t) {
    out[0] = glm_lerp(from.x, to.x, t);
    out[1] = glm_lerp(from.y, to.y, t);
}

static size_t countP(int N, unsigned char *tags) {
    size_t result = N;

    for (int i = 0; i < N; i += 1) {
        if ((tags[i] & 1) == (tags[(i + 1) % N] & 1)) {
            result += 1;
        }
    }

    return result / 2;
}

static size_t countPoints(FT_Outline *outline) {
    size_t result = countP(outline->contours[0] + 1, outline->tags);

    for (size_t i = 1; i < outline->n_contours; i += 1) {
        result += countP(
            outline->contours[i] - outline->contours[i - 1],
            outline->tags + outline->contours[i - 1] + 1
        );
    }

    return result;
}

static void toArrays(size_t S, size_t E, FT_Outline *outline, size_t N, vec2 onLine[N], vec2 offLine[N]) {
    size_t quantity = E - S + 1;

    size_t i = 0;
    size_t j = 0;
    size_t k = 0;

    unsigned char *tags = outline->tags + S;
    FT_Vector *points = outline->points + S;

    while ((tags[k] & 1) == 0) k += 1;
    while (i < N || j < N) {
        if ((tags[k] & 1) != (tags[(k + 1) % quantity] & 1)) {
            if ((tags[k] & 1) == 1) {
                onLine[i][0] = points[k].x;
                onLine[i][1] = points[k].y;
                i += 1;
            }
            else {
                offLine[j][0] = points[k].x;
                offLine[j][1] = points[k].y;
                j += 1;
            }
        }
        else { 
            if ((tags[k] & 1) == 1) {
                onLine[i][0] = points[k].x;
                onLine[i][1] = points[k].y;
                
                lerp_l(offLine[j], points[k], points[(k + 1) % quantity], 0.5);
            }
            else {
                offLine[j][0] = points[k].x;
                offLine[j][1] = points[k].y;

                lerp_l(onLine[i], points[k], points[(k + 1) % quantity], 0.5);
            }

            i += 1;
            j += 1;
        }

        k += 1;
        k %= quantity;
    }
}

static bool isLeft(vec2 a, vec2 b, vec2 c) {
    return (b[0] - a[0]) * (c[1] - a[1]) > (b[1] - a[1]) * (c[0] - a[0]);
}

struct contour {
    size_t *arr;
    size_t quantity;
    float mostRight;

    struct contour *next;
    struct contour *hole;
};

float max(float x, float y) { return x > y ? x : y; }

void loadBezier(FT_GlyphSlot slot, struct Mesh *mesh, struct contour *contours, size_t pQuantity, size_t pointIDs[pQuantity]) {
    FT_Outline *outline = &slot->outline;
    
    size_t q = 0;
    size_t z = 0;

    mesh->verticesQuantity = pQuantity * 2 + 1;
    mesh->indicesQuantity = pQuantity * 3;
    mesh->vertices = malloc(sizeof(struct FontVertex) * mesh->verticesQuantity);
    mesh->indices = malloc(sizeof(uint16_t) * mesh->indicesQuantity);

    struct FontVertex (*vertices)[2] = (void *)mesh->vertices;
    uint16_t (*indices)[3] = (void *)mesh->indices;

    for (int i = 0; i < outline->n_contours; i += 1) {
        size_t start_point = (i == 0) ? 0 : outline->contours[i - 1] + 1;
        size_t end_point = outline->contours[i];

        size_t N = countP(end_point - start_point + 1, outline->tags + start_point);
        vec2 onLine[N];
        vec2 offLine[N];

        toArrays(start_point, end_point, outline, N, onLine, offLine);

        contours[i].arr = pointIDs + q;
        for (size_t j = 0; j < N; j += 1) {
            bool isLeftV = isLeft(onLine[j], offLine[j], onLine[(j + 1) % N]);

            vertices[z + j][0] = (struct FontVertex) {
                .pos = {
                    onLine[j][0] / slot->metrics.height,
                    onLine[j][1] / slot->metrics.height,
                },
                .color = { 0.0, 0.0, 0.0 },
                .bezzier = { j & 1, j & 1 },
                .inOut = isLeftV ? 1 : 0,
            };
            vertices[z + j][1] = (struct FontVertex) {
                .pos = {
                    offLine[j][0] / slot->metrics.height,
                    offLine[j][1] / slot->metrics.height,
                },
                .color = { 0.0, 0.0, 0.0 },
                .bezzier = { 0.5f, 0.0f },
                .inOut = isLeftV ? 1 : 0
            };

            indices[z + j][0] = 2 * (z + j) + (isLeftV ? 0 : 1);
            indices[z + j][1] = 2 * (z + j) + (isLeftV ? 1 : 0);
            indices[z + j][2] = 2 * (z + j + 1);

            contours[i].mostRight = max(contours[i].mostRight, vertices[z + j][0].pos[0]);
            pointIDs[q] = 2 * (z + j);
            q += 1;
            if (isLeftV) {
                contours[i].mostRight = max(contours[i].mostRight, vertices[z + j][1].pos[0]);
                pointIDs[q] = 2 * (z + j) + 1;

                contours[i].quantity += 1;
                q += 1;
            }

            contours[i].quantity += 1;
        }

        z += N;
        vertices[z][0] = (struct FontVertex) {
            .pos = {
                vertices[0][0].pos[0],
                vertices[0][0].pos[1],
            },
            .color = {
                vertices[0][0].color[0],
                vertices[0][0].color[1],
                vertices[0][0].color[2],
            },
            .bezzier = { N & 1, N & 1 },
            .inOut = vertices[z - 1][0].inOut,
        };
    }
}

int doesIntersect(vec2 p0, vec2 p1, vec2 p2, vec2 p3) {
    vec2 s02 = {
        [0] = p0[0] - p2[0],
        [1] = p0[1] - p2[1]
    };
    vec2 s10 = {
        [0] = p1[0] - p0[0],
        [1] = p1[1] - p0[1]
    };
    vec2 s32 = {
        [0] = p3[0] - p2[0],
        [1] = p3[1] - p2[1]
    };

    float denom = s10[0] * s32[1] - s32[0] * s10[1];
    float s_numer = s10[0] * s02[1] - s10[1] * s02[0];
    float t_numer = s32[0] * s02[1] - s32[1] * s02[0];

    bool denomPositive = denom > 0;

    return (
        denom != 0 && // colinear
        (s_numer < 0) != denomPositive &&
        (t_numer < 0) != denomPositive &&
        (s_numer > denom) != denomPositive &&
        (t_numer > denom) != denomPositive
    ) ? 1 : 0;
}

bool isPointInside(vec2 P, size_t quantity, size_t indices[quantity], struct FontVertex *vertices) {
    int count = 0;
    vec2 Q = {
        [0] = P[0],
        [1] = P[1] + 1.0f,
    };

    size_t i = 0;
    while (i < quantity - 1) {
        count += doesIntersect(P, Q, vertices[indices[i]].pos, vertices[indices[i + 1]].pos);

        i += 1;
    }
    count += doesIntersect(P, Q, vertices[indices[i]].pos, vertices[indices[0]].pos);

    return count % 2 == 1;
}

bool isContourInside(struct contour *this, struct contour *that, struct FontVertex *vertices) {
    bool result = true;

    for (size_t i = 0; result && i < this->quantity; i += 1) {
        result = isPointInside(vertices[this->arr[i]].pos, that->quantity, this->arr, vertices);
    }

    return result;
}

struct contour *addToTree(struct contour *tree, struct contour *new, struct FontVertex *vertex) {
    if (isContourInside(tree, new, vertex)) {
        struct contour *toAdd = tree->next;
        tree->next = NULL;

        new->hole = tree;
        tree = new;
        while (toAdd != NULL) {
            struct contour *newNew = toAdd;
            toAdd = toAdd->next;

            newNew->next = NULL;

            tree = addToTree(tree, newNew, vertex);
        }
    }
    else if (isContourInside(new, tree, vertex)) {
        tree->hole = (tree->hole == NULL) ? new : addToTree(tree->hole, new, vertex);
    }
    else {
        tree->next = (tree->next == NULL) ? new : addToTree(tree->next, new, vertex);
    }

    return tree;
}

void printTree(struct contour *tree, int q) {
    if (tree != NULL) {
        printf("%*sThis %zu\n", q, "", tree->quantity);

        printf("%*sHole\n", q, "");
        printTree(tree->hole, q + 4);

        printf("%*sNext\n", q, "");
        printTree(tree->next, q + 4);
    }
}

size_t countTriangles(struct contour *tree) {
    size_t n = tree->quantity - 2;

    struct contour *t = tree->hole;
    while (t != NULL) {
        n += t->quantity + (t->hole == NULL ? 0 : countTriangles(t->hole)) + 2;
        t = t->next;
    }

    return n + (tree->next == NULL ? 0 : countTriangles(tree->next));
}

struct contour *sortHoles(struct contour *tree) {
    if (tree != NULL) {
        struct contour *head = tree;
        struct contour *rest = tree->next = sortHoles(tree->next);

        tree->hole = sortHoles(tree->hole);

        while (rest != NULL && tree->mostRight < rest->mostRight) {
            rest = (head = rest)->next;
        }

        if (head != tree) {
            (head->next = tree)->next = rest;
            tree = head;
        }
    }

    return tree;
}

void appendNext(struct contour *tree, struct contour *add) {
    while (tree->next != NULL) {
        tree = tree->next;
    }

    tree->next = add;
}

void flatten(struct contour *tree) {
    while (tree != NULL) {
        struct contour *hole = tree->hole;
        while (hole != NULL) {
            appendNext(tree, hole->hole);
            hole->hole = NULL;

            hole = hole->next;
        }

        tree = tree->next;
    }
}

size_t getRightmost(size_t *arr, float rightMost, struct FontVertex *vertex) {
    size_t i = 0;

    while (vertex[arr[i]].pos[0] != rightMost) {
        i += 1;
    }

    return i;
}

bool isVecEqual(vec2 a, vec2 b) {
    return a[0] == b[0] && a[1] == b[1];
}

bool isDiagonal(size_t i, size_t j, size_t quantity, size_t indices[quantity], struct FontVertex *vertex) {
    bool isDiagonal = true;
    size_t k = 0;

    while (isDiagonal && k < quantity) {
        if (i != k && i != k + 1)
        if (j != k && j != k + 1)
        isDiagonal = 0 == doesIntersect(
            vertex[indices[i]].pos,
            vertex[indices[j]].pos,
            vertex[indices[k]].pos,
            vertex[indices[(k + 1) % quantity]].pos
        );

        k += 1;
    }

    return isDiagonal;
}

size_t createOutline(struct contour *tree, size_t *arr, struct FontVertex *vertex) {
    struct contour *hole = tree->hole;
    size_t q = tree->quantity;
    memcpy(arr, tree->arr, sizeof(size_t) * tree->quantity);
    size_t rm = getRightmost(arr, tree->mostRight, vertex);

    while (NULL != hole) {
        [[maybe_unused]] size_t rightMost = getRightmost(hole->arr, hole->mostRight, vertex);
        [[maybe_unused]] size_t i = rm;

        while (isDiagonal(rightMost, i, q, arr, vertex) || isDiagonal(rightMost, i, hole->quantity, hole->arr, vertex)) {
            i += 1;
            i %= q;
        }

        memcpy(arr + i + 1 + hole->quantity + 2, arr + i + 1, (q - i - 1) * sizeof(size_t));
        for (size_t j = 0; j < hole->quantity; j += 1) {
            (arr + i + 1)[j] = hole->arr[(rightMost + j) % hole->quantity];
        }
        (arr + i + 1)[hole->quantity] = hole->arr[rightMost];
        (arr + i + 2)[hole->quantity] = arr[i];

        q += hole->quantity + 2;

        hole = hole->next;
    }

    return q;
}

#define LOG(x, y) printf("%s: %f %f, ", #y, x[0], x[1])

void avg(vec2 out, vec2 a, vec2 b, vec2 c) {
    out[0] = (a[0] + b[0] + c[0]) / 3;
    out[1] = (a[1] + b[1] + c[1]) / 3;
}

void triangulate(int N, size_t vertexIDs[N], struct FontVertex *vertex, uint16_t (*indices)[3]);

void ttfLoadModel(const char *objectPath, struct actualModel *model, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    FT_Library library;
    FT_Face face;

    IF (0 == FT_Init_FreeType(&library), "No Library")
    IF (0 == FT_New_Face(library, objectPath, 0, &face), "No Face")
    IF (0 == FT_Set_Pixel_Sizes(face, 100, 100), "Size Error")
    IF (0 == FT_Load_Glyph(face, FT_Get_Char_Index(face, 'I'), FT_LOAD_NO_BITMAP), "No Glyph") {
        FT_GlyphSlot slot = face->glyph;
        FT_Outline *outline = &slot->outline;

        size_t Points = countPoints(outline);
        size_t Arr[Points] = {};

        struct contour contours[outline->n_contours] = {};
        struct contour *tree = contours;

        model->meshQuantity = 1;
        model->mesh = malloc(sizeof(struct Mesh) * model->meshQuantity);
        model->mesh[0].sizeOfVertex = sizeof(struct FontVertex);

        loadBezier(slot, model->mesh, contours, Points, Arr);
        for (size_t i = 1; i < outline->n_contours; i += 1) {
            tree = addToTree(tree, &contours[i], model->mesh->vertices);
        }

        flatten(tree = sortHoles(tree));
        size_t ntq = countTriangles(tree);

        model->mesh->indicesQuantity = 3 * (Points + ntq);
        model->mesh->indices = realloc(model->mesh->indices, sizeof(uint16_t) * model->mesh->indicesQuantity);

        triangulate(tree->quantity, tree->arr, model->mesh->vertices, (void *)(model->mesh->indices + 3 * Points));

        model->mesh->verticesQuantity = 4 * Points + 1;
        model->mesh->vertices = realloc(model->mesh->vertices, sizeof(struct FontVertex) * model->mesh->verticesQuantity);
        memcpy(BFR(model->mesh->vertices) + 2 * Points + 1, model->mesh->vertices, sizeof(struct FontVertex) * 2 * Points);

        for (size_t i = 2 * Points + 1; i < 4 * Points; i += 1) {
            BFR(model->mesh->vertices)[i].inOut = 2;
        }

        for (size_t i = 3 * Points; i < 3 * (Points + ntq); i += 1) {
            model->mesh->indices[i] += 2 * Points + 1;
        }
    }

    createStorageBuffer(model->meshQuantity * sizeof(mat4), model->localMesh.buffers, model->localMesh.buffersMemory, model->localMesh.buffersMapped, device, physicalDevice, surface);

    for (uint32_t k = 0; k < MAX_FRAMES_IN_FLIGHT; k += 1) {
        glm_mat4_identity(((mat4 **)model->localMesh.buffersMapped)[k][0]);
    }
}
