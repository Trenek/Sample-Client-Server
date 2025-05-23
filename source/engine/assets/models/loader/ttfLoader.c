#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include <freetype/freetype.h>
#include <cglm.h>
#include <stdint.h>
#include <assert.h>

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

static void lerp_f(vec2 out, vec2 from, vec2 to, float t) {
    out[0] = glm_lerp(from[0], to[0], t);
    out[1] = glm_lerp(from[1], to[1], t);
}

static void divideBezzier(vec2 out, vec2 part1, vec2 part2, vec2 a, vec2 b, vec2 c, float t) {
    vec2 part[2];

    lerp_f(part[0], a, b, t);
    lerp_f(part[1], b, c, t);
    lerp_f(out, part[0], part[1], t);

    memcpy(part1, part[0], sizeof(vec2));
    memcpy(part2, part[1], sizeof(vec2));
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
    size_t *pointIDs;
    size_t qPointsIDs;
    size_t N;

    struct contour *next;
    struct contour *hole;

    struct FontVertex (*vertices)[2];
    uint16_t (*indices)[3];

    struct FontVertex lastVertex;
};

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

static int doesIntersect2(vec2 p0, vec2 p1, vec2 p2, vec2 p3) {
    return 
        p0[1] > p2[1] && p0[1] > p3[1] ? 0 :
        p0[0] > p2[0] && p0[0] > p3[0] ? 0 :
        p0[0] < p2[0] && p0[0] < p3[0] ? 0 :
        p0[0] == p2[0] && p0[0] == p3[0] ? 0 :
        p0[0] == p2[0] && p0[0] > p3[0] ? 0 :
        p0[0] == p3[0] && p0[0] > p2[0] ? 0 :
        p0[1] < p2[1] && p0[1] < p3[1] && p0[0] == p2[0] && p0[0] < p3[0] ? 1 :
        p0[1] < p2[1] && p0[1] < p3[1] && p0[0] == p3[0] && p0[0] < p2[0] ? 1 :
        doesIntersect(p0, p1, p2, p3);
}


#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MAX3(x, y, z) MAX(MAX(x, y), z)
static bool isPointInside(vec2 P, struct contour *that) {
    int count = 0;

    for (size_t i = 0; i < that->N; i += 1) {
        vec2 Q = {
            [0] = P[0],
            [1] = MAX3(P[1], that->vertices[i][0].pos[1], that->vertices[(i + 1) % that->N][0].pos[1]) + 1.0f
        };
        count += doesIntersect2(P, Q, that->vertices[i][0].pos, that->vertices[(i + 1) % that->N][0].pos);
    }

    return count % 2 == 1;
}

static bool isContourInside(struct contour *this, struct contour *that) {
    bool result = true;
    size_t i = 0;

    for (i = 0; result && i < this->N; i += 1) {
        result = isPointInside(this->vertices[i][0].pos, that);
    }

    return result;
}

static struct contour *addToTree(struct contour *tree, struct contour *new) {
    if (isContourInside(tree, new)) {
        struct contour *toAdd = tree->next;
        tree->next = NULL;

        new->hole = tree;
        tree = new;
        while (toAdd != NULL) {
            struct contour *newNew = toAdd;
            toAdd = toAdd->next;

            newNew->next = NULL;

            tree = addToTree(tree, newNew);
        }
    }
    else if (isContourInside(new, tree)) {
        tree->hole = (tree->hole == NULL) ? new : addToTree(tree->hole, new);
    }
    else {
        tree->next = (tree->next == NULL) ? new : addToTree(tree->next, new);
    }

    return tree;
}

static size_t countTriangles(const struct contour *const tree) {
    size_t n = tree->qPointsIDs - 2;

    const struct contour *t = tree->hole;
    while (t != NULL) {
        n += t->qPointsIDs + (t->hole == NULL ? 0 : countTriangles(t->hole)) + 2;
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

static void getOrderRight(struct contour *tree, struct FontVertex *) {
    struct contour *hole = NULL;

    while (tree != NULL) {
        reverse(tree->qPointsIDs, tree->pointIDs);

        hole = tree->hole;
        while (hole != NULL) {
            reverse(hole->qPointsIDs, hole->pointIDs);
            
            hole = hole->next;
        }

        tree = tree->next;
    }
}

static size_t getPolygonQuantity(struct contour *tree) {
    size_t q = 0;
    struct contour *hole = tree->hole;

    do {
        hole = tree->hole;
        q += 1;

        while (hole != NULL) {
            q += 1;
            hole = hole->next;
        }

        tree = tree->next;
    } while (tree != NULL);

    return q;
}

static void polygonToArrays(struct contour *tree, size_t *vq, size_t **vi) {
    struct contour *hole = tree->hole;

    do {
        *(vq++) = tree->qPointsIDs;
        *(vi++) = tree->pointIDs;

        hole = tree->hole;
        while (hole != NULL) {
            *(vq++) = hole->qPointsIDs;
            *(vi++) = hole->pointIDs;

            hole = hole->next;
        }

        tree = tree->next;
    } while (tree != NULL);
}

static struct contour createContour(FT_Face face, size_t start_point, size_t end_point) {
    FT_Outline *outline = &face->glyph->outline;

    size_t N = countOnlineVerticesInOutline(end_point - start_point + 1, outline->tags + start_point);
    struct contour new = {
        .N = N,
        .vertices = malloc(sizeof(struct FontVertex[2]) * N),
        .indices = malloc(sizeof(uint16_t[3]) * N),
        .pointIDs = malloc(sizeof(size_t) * 2 * N),
        .qPointsIDs = 0
    };

    vec2 onLine[new.N];
    vec2 offLine[new.N];
    toArrays(start_point, end_point, outline, new.N, onLine, offLine);

    for (size_t i = 0; i < new.N; i += 1) {
        bool isLeftV = isCounterClockwise(3, (vec2[]) {
            { onLine[i][0], onLine[i][1] },
            { offLine[i][0], offLine[i][1] },
            { onLine[(i + 1) % new.N][0], onLine[(i + 1) % new.N][1] }
        });

        new.vertices[i][0] = (struct FontVertex) {
            .pos = {
                onLine[i][0] / face->max_advance_width,
                onLine[i][1] / face->max_advance_width,
            },
            .color = { 0.0, 0.0, 0.0 },
            .bezzier = { i & 1, i & 1 },
            .inOut = isLeftV ? 1 : 0,
        };
        new.vertices[i][1] = (struct FontVertex) {
            .pos = {
                offLine[i][0] / face->max_advance_width,
                offLine[i][1] / face->max_advance_width,
            },
            .color = { 0.0, 0.0, 0.0 },
            .bezzier = { 0.5f, 0.0f },
            .inOut = isLeftV ? 1 : 0
        };

        new.indices[i][0] = 2 * i + (isLeftV ? 0 : 1);
        new.indices[i][1] = 2 * i + (isLeftV ? 1 : 0);
        new.indices[i][2] = 2 * i + 2;

        new.pointIDs[new.qPointsIDs] = 2 * i;
        new.qPointsIDs += 1;
        if (isLeftV) {
            new.pointIDs[new.qPointsIDs] = 2 * i + 1;
            new.qPointsIDs += 1;
        }
    }

    return new;
}

static void addBezzier(struct contour *toAdd, size_t index) {
    toAdd->N += 1;

    toAdd->vertices = realloc(toAdd->vertices, toAdd->N * sizeof(struct FontVertex[2]));
    toAdd->indices = realloc(toAdd->indices, toAdd->N * sizeof(uint16_t[3]));
    toAdd->pointIDs = realloc(toAdd->pointIDs, 2 * toAdd->N * sizeof(size_t));

    for (size_t i = toAdd->N - 1; i > index + 1; i -= 1) {
        memcpy(toAdd->vertices[i], toAdd->vertices[i - 1], sizeof(struct FontVertex[2]));
    }

    divideBezzier(
        toAdd->vertices[index + 1][0].pos,
        toAdd->vertices[index][1].pos,
        toAdd->vertices[index + 1][1].pos,
        toAdd->vertices[index][0].pos,
        toAdd->vertices[index][1].pos,
        toAdd->vertices[(index + 2) % toAdd->N][0].pos,
        0.5f
    );

    memcpy(toAdd->vertices[index + 1][0].color, toAdd->vertices[index][0].color, sizeof(vec3));
    memcpy(toAdd->vertices[index + 0][1].color, toAdd->vertices[index][0].color, sizeof(vec3));
    memcpy(toAdd->vertices[index + 1][1].color, toAdd->vertices[index][0].color, sizeof(vec3));

    toAdd->qPointsIDs = 0;
    for (size_t i = 0; i < toAdd->N; i += 1) {
        bool isLeftV = isCounterClockwise(3, (vec2[]) {
            {
                toAdd->vertices[i][0].pos[0],
                toAdd->vertices[i][0].pos[1]
            },
            {
                toAdd->vertices[i][1].pos[0],
                toAdd->vertices[i][1].pos[1]
            },
            {
                toAdd->vertices[(i + 1) % toAdd->N][0].pos[0],
                toAdd->vertices[(i + 1) % toAdd->N][0].pos[1],
            }
        });

        toAdd->vertices[i][0].bezzier[0] = i & 1;
        toAdd->vertices[i][0].bezzier[1] = i & 1;
        toAdd->vertices[i][0].inOut = isLeftV ? 1 : 0;

        toAdd->vertices[i][1].bezzier[0] = 0.5f;
        toAdd->vertices[i][1].bezzier[1] = 0.0f;
        toAdd->vertices[i][1].inOut = isLeftV ? 1 : 0;

        toAdd->indices[i][0] = 2 * i + (isLeftV ? 0 : 1);
        toAdd->indices[i][1] = 2 * i + (isLeftV ? 1 : 0);
        toAdd->indices[i][2] = 2 * i + 2;

        toAdd->pointIDs[toAdd->qPointsIDs] = 2 * i;
        toAdd->qPointsIDs += 1;
        if (isLeftV) {
            toAdd->pointIDs[toAdd->qPointsIDs] = 2 * i + 1;
            toAdd->qPointsIDs += 1;
        }
    }
}

static void doHR(struct contour *suspect, struct contour *other) {
    for (size_t i = 0; i < suspect->qPointsIDs; i += 1) {
        for (size_t j = 0; j < other->qPointsIDs; j += 1) {
            size_t index[4] = {
                suspect->pointIDs[i],
                suspect->pointIDs[(i + 1) % suspect->qPointsIDs],
                  other->pointIDs[j],
                  other->pointIDs[(j + 1) % other->qPointsIDs]
            };
            if (doesIntersect(
                suspect->vertices[index[0] / 2][index[0] & 1].pos,
                suspect->vertices[index[1] / 2][index[1] & 1].pos,
                  other->vertices[index[2] / 2][index[2] & 1].pos,
                  other->vertices[index[3] / 2][index[3] & 1].pos
            ))
            if      (index[0] & 1) addBezzier(suspect, index[0] / 2);
            else if (index[1] & 1) addBezzier(suspect, index[1] / 2);
            else if (index[2] & 1) addBezzier(other,   index[2] / 2);
            else if (index[3] & 1) addBezzier(other,   index[3] / 2);
            else assert(false);
        }
    }
}

static void doHigherResolution(struct contour *contours, int max) {
    struct contour *suspect = &contours[max--];

    while (max >= 0) {
        doHR(suspect, &contours[max--]);
    }
}

static struct contour *loadBezier(FT_Face face, struct Mesh *mesh, struct contour *contours, size_t *outPQuantity) {
    struct contour *tree = NULL;
    FT_Outline *outline = &face->glyph->outline;
    
    size_t z = 0;
    size_t pQuantity = 0;

    for (int i = 0; i < outline->n_contours; i += 1) {
        size_t start_point = (i == 0) ? 0 : outline->contours[i - 1] + 1;
        size_t end_point = outline->contours[i];

        contours[i] = createContour(face, start_point, end_point);
        doHigherResolution(contours, i);

        tree = tree == NULL ? &contours[i] : addToTree(tree, &contours[i]);
    }

    for (int i = 0; i < outline->n_contours; i += 1) {
        pQuantity += contours[i].N;
    }

    mesh->verticesQuantity = 4 * pQuantity + outline->n_contours;
    mesh->indicesQuantity = pQuantity * 3;
    mesh->vertices = malloc(sizeof(struct FontVertex) * mesh->verticesQuantity);
    mesh->indices = malloc(sizeof(uint16_t) * mesh->indicesQuantity);

    struct FontVertex (*vertices)[2] = mesh->vertices;
    uint16_t (*indices)[3] = (void *)mesh->indices;

    z = 0;
    for (int i = 0; i < outline->n_contours; i += 1) {
        for (size_t j = 0; j < contours[i].N; j += 1) {
            contours[i].indices[j][0] += 2 * z;
            contours[i].indices[j][1] += 2 * z;
            contours[i].indices[j][2] += 2 * z;
        }
        for (size_t j = 0; j < contours[i].qPointsIDs; j += 1) {
            contours[i].pointIDs[j] += 2 * z;
        }

        memcpy(vertices + z, contours[i].vertices, sizeof(struct FontVertex[2]) * contours[i].N);
        memcpy(indices + z, contours[i].indices, sizeof(uint16_t[3]) * contours[i].N);

        z += contours[i].N;
    }

    z = 0;
    for (int i = 0; i < outline->n_contours; i += 1) {
        indices[z + contours[i].N - 1][2] = 4 * pQuantity + i;
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
            .bezzier = { contours[i].N & 1, contours[i].N & 1 },
            .inOut = vertices[z + contours[i].N - 1][0].inOut,
        };

        z += contours[i].N;
    }

    memcpy(BFR(mesh->vertices) + 2 * pQuantity, mesh->vertices, sizeof(struct FontVertex) * 2 * pQuantity);

    for (size_t i = 2 * pQuantity; i < 4 * pQuantity; i += 1) {
        BFR(mesh->vertices)[i].inOut = 2;
    }

    flatten(tree);
    getOrderRight(tree, mesh->vertices);

    *outPQuantity = pQuantity;
    return tree;
}

void triangulate(size_t q, size_t vertexQuantity[q], size_t *vertexIDs[q], struct FontVertex *vertex, uint16_t (*triangles)[3]);

static void generateTriangles(struct contour *tree, struct Mesh *mesh, size_t qOnlinePoints) {
    size_t ntq = countTriangles(tree);

    mesh->indicesQuantity += 3 * ntq;
    mesh->indices = realloc(mesh->indices, sizeof(uint16_t) * mesh->indicesQuantity);

    size_t q = getPolygonQuantity(tree);
    size_t vq[q];
    size_t *vi[q];

    polygonToArrays(tree, vq, vi);

    triangulate(q, vq, vi, mesh->vertices, (void *)(mesh->indices + 3 * qOnlinePoints));

    for (size_t i = 3 * qOnlinePoints; i < 3 * (qOnlinePoints + ntq); i += 1) {
        mesh->indices[i] %= 2 * qOnlinePoints;
        mesh->indices[i] += 2 * qOnlinePoints;
    }
}

static void loadCharacter(FT_Face face, struct Mesh *mesh, char character, float *space) {
    size_t qOnlinePoints = 0;

    IF (0 == FT_Load_Glyph(face, FT_Get_Char_Index(face, character), FT_LOAD_NO_BITMAP), "No Glyph") {
        FT_GlyphSlot slot = face->glyph;
        FT_Outline *outline = &slot->outline;

        *space = (float)(slot->advance.x) / face->max_advance_width;

        struct contour contours[outline->n_contours] = {};
        struct contour *tree = contours;

        tree = loadBezier(face, mesh, contours, &qOnlinePoints);
        generateTriangles(tree, mesh, qOnlinePoints);

        for (int i = 0; i < outline->n_contours; i += 1) {
            free(contours[i].indices);
            free(contours[i].vertices);
            free(contours[i].pointIDs);
        }
    }
}

static float loadSpaceOffset(FT_Face face) {
    float result = 0;

    IF (0 == FT_Load_Glyph(face, FT_Get_Char_Index(face, ' '), FT_LOAD_NO_BITMAP), "No Glyph") {
        FT_GlyphSlot slot = face->glyph;

        result = (float)slot->advance.x / face->max_advance_width;
    }

    return result;
}

const char *buffer =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "!?"
;

size_t getGlyphID(char a) {
    size_t i = 0;

    while (buffer[i] != 0 && buffer[i] != a) i += 1;

    return 
        a == ' ' ? i :
        buffer[i] == 0 ? i - 1 : i;
}

void LoadCharacter(struct actualModel *model, FT_Face face) {
    float space = 0;
    mat4 **glyphOffset = (mat4 **)model->localMesh.buffersMapped;

    for (size_t i = 0; i < model->meshQuantity; i += 1) {
        loadCharacter(face, &model->mesh[i], buffer[i], &space);

        for (uint32_t k = 0; k < MAX_FRAMES_IN_FLIGHT; k += 1) {
            model->mesh[i].sizeOfVertex = sizeof(struct FontVertex);
            glm_mat4_identity(glyphOffset[k][i]);
            glm_translate(glyphOffset[k][i], (vec3) {
                space, 0, 0
            });
        }
    }
    for (uint32_t k = 0; k < MAX_FRAMES_IN_FLIGHT; k += 1) {
        glm_mat4_identity(glyphOffset[k][model->meshQuantity]);
        glm_translate(glyphOffset[k][model->meshQuantity], (vec3) {
            loadSpaceOffset(face), 0, 0
        });
    }
}

void ttfLoadModel(const char *objectPath, struct actualModel *model, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    FT_Library library;
    FT_Face face;

    model->meshQuantity = strlen(buffer);
    model->mesh = malloc(sizeof(struct Mesh) * (model->meshQuantity));

    createBuffers(
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        model->meshQuantity * sizeof(mat4) + sizeof(mat4),
        model->localMesh.buffers, 
        model->localMesh.buffersMemory, 
        model->localMesh.buffersMapped, 
        device, 
        physicalDevice, 
        surface
    );

    IF (0 == FT_Init_FreeType(&library), "No Library") {
        IF (0 == FT_New_Face(library, objectPath, 0, &face), "No Face") {
            IF (0 == FT_Set_Pixel_Sizes(face, 100, 100), "Size Error") {
                LoadCharacter(model, face);
            }
            FT_Done_Face(face);
        }
        FT_Done_FreeType(library);
    }
}
