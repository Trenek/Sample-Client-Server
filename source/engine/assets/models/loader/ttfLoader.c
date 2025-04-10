#include <stdint.h>
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include <freetype/freetype.h>
#include <cglm.h>

#include "actualModel.h"
#include "myMalloc.h"

#include "Vertex.h"

#define IF(x, y) if (!(x)) printf("%s", y); else

#define BFR(x) ((struct FontVertex *)(x))

bool isCounterClockwise(size_t N, vec2 *v);

static void lerp_l(vec2 out, FT_Vector from, FT_Vector to, float t) {
    out[0] = glm_lerp(from.x, to.x, t);
    out[1] = glm_lerp(from.y, to.y, t);
}

static size_t countOnlineVerticesInOutline(int N, unsigned char *tags) {
    size_t result = N;

    for (int i = 0; i < N; i += 1) {
        if ((tags[i] & 1) == (tags[(i + 1) % N] & 1)) {
            result += 1;
        }
    }

    return result / 2;
}

static size_t countOnlineVerticesInPolygon(FT_Outline *outline) {
    size_t result = countOnlineVerticesInOutline(outline->contours[0] + 1, outline->tags);

    for (size_t i = 1; i < outline->n_contours; i += 1) {
        result += countOnlineVerticesInOutline(
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

struct contour {
    size_t *arr;
    size_t quantity;

    struct contour *next;
    struct contour *hole;
};

static void loadBezier(FT_GlyphSlot slot, struct Mesh *mesh, struct contour *contours, size_t pQuantity, size_t pointIDs[2 * pQuantity]) {
    FT_Outline *outline = &slot->outline;
    
    size_t q = 0;
    size_t z = 0;

    mesh->verticesQuantity = 4 * pQuantity + outline->n_contours;
    mesh->indicesQuantity = pQuantity * 3;
    mesh->vertices = malloc(sizeof(struct FontVertex) * mesh->verticesQuantity);
    mesh->indices = malloc(sizeof(uint16_t) * mesh->indicesQuantity);

    struct FontVertex (*vertices)[2] = (void *)mesh->vertices;
    uint16_t (*indices)[3] = (void *)mesh->indices;

    for (int i = 0; i < outline->n_contours; i += 1) {
        size_t start_point = (i == 0) ? 0 : outline->contours[i - 1] + 1;
        size_t end_point = outline->contours[i];

        size_t N = countOnlineVerticesInOutline(end_point - start_point + 1, outline->tags + start_point);
        vec2 onLine[N];
        vec2 offLine[N];

        toArrays(start_point, end_point, outline, N, onLine, offLine);

        contours[i].arr = pointIDs + q;
        for (size_t j = 0; j < N; j += 1) {
            bool isLeftV = isCounterClockwise(3, (vec2[]) { 
                { onLine[j][0], onLine[j][1] },
                { offLine[j][0], offLine[j][1] },
                { onLine[(j + 1) % N][0], onLine[(j + 1) % N][1] }
            });

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

            pointIDs[q] = 2 * (z + j);
            q += 1;
            if (isLeftV) {
                pointIDs[q] = 2 * (z + j) + 1;

                contours[i].quantity += 1;
                q += 1;
            }

            contours[i].quantity += 1;
        }

        indices[z + N - 1][2] = 4 * pQuantity + i;
        vertices[2 * pQuantity][i] = (struct FontVertex) {
            .pos = {
                vertices[z][0].pos[0],
                vertices[z][0].pos[1],
            },
            .color = {
                vertices[z][0].color[0],
                vertices[z][0].color[1],
                vertices[z][0].color[2],
            },
            .bezzier = { N & 1, N & 1 },
            .inOut = vertices[z + N - 1][0].inOut,
        };
        z += N;
    }

    printf("pq = %zu, q = %zu\n", pQuantity, q);

    memcpy(BFR(mesh->vertices) + 2 * pQuantity, mesh->vertices, sizeof(struct FontVertex) * 2 * pQuantity);

    for (size_t i = 2 * pQuantity; i < 4 * pQuantity; i += 1) {
        BFR(mesh->vertices)[i].inOut = 2;
    }
}

static int doesIntersect(vec2 p0, vec2 p1, vec2 p2, vec2 p3) {
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

static bool isPointInside(vec2 P, size_t quantity, size_t indices[quantity], struct FontVertex *vertices) {
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

static bool isContourInside(struct contour *this, struct contour *that, struct FontVertex *vertices) {
    bool result = true;

    for (size_t i = 0; result && i < this->quantity; i += 1) {
        result = isPointInside(vertices[this->arr[i]].pos, that->quantity, that->arr, vertices);
    }

    return result;
}

static struct contour *addToTree(struct contour *tree, struct contour *new, struct FontVertex *vertex) {
    printf("Oho\n");
    if (isContourInside(tree, new, vertex)) {
        printf("1");
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
        printf("2");
        tree->hole = (tree->hole == NULL) ? new : addToTree(tree->hole, new, vertex);
    }
    else {
        printf("3");
        tree->next = (tree->next == NULL) ? new : addToTree(tree->next, new, vertex);
    }

    return tree;
}

[[maybe_unused]]
static void printTree(struct contour *tree, int q) {
    if (tree != NULL) {
        printf("%*sThis %zu\n", q, "", tree->quantity);

        printf("%*sHole\n", q, "");
        printTree(tree->hole, q + 4);

        printf("%*sNext\n", q, "");
        printTree(tree->next, q + 4);
    }
}

static size_t countTriangles(const struct contour *const tree) {
    size_t n = tree->quantity - 2;

    const struct contour *t = tree->hole;
    while (t != NULL) {
        n += t->quantity + (t->hole == NULL ? 0 : countTriangles(t->hole)) + 2;
        t = t->next;
    }

    return n + (tree->next == NULL ? 0 : countTriangles(tree->next));
}

static void appendNext(struct contour *tree, struct contour *add) {
    while (tree->next != NULL) {
        tree = tree->next;
    }

    tree->next = add;
}

static void flatten(struct contour *tree) {
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

static void reverse(int N, size_t arr[N]) {
    size_t temp = 0;

    for (int i = 0; i < N - 1 - i; i += 1) {
        temp = arr[i];

        arr[i] = arr[N - 1 - i];
        arr[N - 1 - i] = temp;
    }
}

static bool isCounterClockwiseVertex(size_t N, size_t arr[N], struct FontVertex *vertex) {
    vec2 array[N];

    for (size_t i = 0; i < N; i += 1) {
        array[i][0] = vertex[arr[i]].pos[0];
        array[i][1] = vertex[arr[i]].pos[1];
    }

    return isCounterClockwise(N, array);
}

static void getOrderRight(struct contour *tree, struct FontVertex *vertex) {
    struct contour *hole = NULL;

    while (tree != NULL) {
        if (!isCounterClockwiseVertex(tree->quantity, tree->arr, vertex)) {
            reverse(tree->quantity, tree->arr);
        }

        hole = tree->hole;
        while (hole != NULL) {
            if (isCounterClockwiseVertex(hole->quantity, hole->arr, vertex)) {
                reverse(hole->quantity, hole->arr);
            }
            
            hole = hole->next;
        }

        tree = tree->next;
    }
}

size_t getPolygonQuantity(struct contour *tree) {
    size_t q = 1;
    struct contour *hole = tree->hole;

    while (hole != NULL) {
        q += 1;
        hole = hole->next;
    }

    return q;
}

void polygonToArrays(struct contour *tree, size_t *vq, size_t **vi) {
    struct contour *hole = tree->hole;

    *(vq++) = tree->quantity;
    *(vi++) = tree->arr;

    while (hole != NULL) {
        *(vq++) = hole->quantity;
        *(vi++) = hole->arr;

        hole = hole->next;
    }
}

void triangulate(size_t q, size_t vertexQuantity[q], size_t *vertexIDs[q], struct FontVertex *vertex, uint16_t (*triangles)[3]);

// Works Perfectly:
// '.', ',', 'A', 'B', 'C', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 
// 'M', 'N', 'P', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
//
// Problematic
// 'D', 'O', 'Q'
void ttfLoadModel(const char *objectPath, struct actualModel *model, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    FT_Library library;
    FT_Face face;

    IF (0 == FT_Init_FreeType(&library), "No Library")
    IF (0 == FT_New_Face(library, objectPath, 0, &face), "No Face")
    IF (0 == FT_Set_Pixel_Sizes(face, 1000, 1000), "Size Error")
    IF (0 == FT_Load_Glyph(face, FT_Get_Char_Index(face, 'D'), FT_LOAD_NO_BITMAP), "No Glyph") {
        FT_GlyphSlot slot = face->glyph;
        FT_Outline *outline = &slot->outline;

        size_t qOnlinePoints = countOnlineVerticesInPolygon(outline);
        size_t Arr[2 * qOnlinePoints] = {};

        struct contour contours[outline->n_contours] = {};
        struct contour *tree = contours;

        model->meshQuantity = 1;
        model->mesh = malloc(sizeof(struct Mesh) * model->meshQuantity);
        model->mesh->sizeOfVertex = sizeof(struct FontVertex);

        loadBezier(slot, model->mesh, contours, qOnlinePoints, Arr);
        for (size_t i = 1; i < outline->n_contours; i += 1) {
            tree = addToTree(tree, &contours[i], model->mesh->vertices);
        }

        flatten(tree);
        getOrderRight(tree, model->mesh->vertices);

        size_t ntq = countTriangles(tree);

        model->mesh->indicesQuantity += 3 * ntq;
        model->mesh->indices = realloc(model->mesh->indices, sizeof(uint16_t) * model->mesh->indicesQuantity);

        while (tree != NULL) {
            size_t q = getPolygonQuantity(tree);

            size_t vq[q];
            size_t *vi[q];

            polygonToArrays(tree, vq, vi);

            printf("Number of thingis = %zu\n", q);

            triangulate(q, vq, vi, model->mesh->vertices, (void *)(model->mesh->indices + 3 * qOnlinePoints));

            tree = tree->next;
        }

        for (size_t i = 3 * qOnlinePoints; i < 3 * (qOnlinePoints + ntq); i += 1) {
            model->mesh->indices[i] %= 2 * qOnlinePoints;
            model->mesh->indices[i] += 2 * qOnlinePoints;
        }
    }

    createStorageBuffer(model->meshQuantity * sizeof(mat4), model->localMesh.buffers, model->localMesh.buffersMemory, model->localMesh.buffersMapped, device, physicalDevice, surface);

    for (uint32_t k = 0; k < MAX_FRAMES_IN_FLIGHT; k += 1) {
        glm_mat4_identity(((mat4 **)model->localMesh.buffersMapped)[k][0]);
    }
}
