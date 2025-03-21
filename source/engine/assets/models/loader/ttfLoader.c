#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include <freetype/freetype.h>
#include <cglm.h>

#include "actualModel.h"
#include "myMalloc.h"

#define IF(x, y) if (!(x)) printf("%s", y); else

static void lerp_l(vec2 out, FT_Vector from, FT_Vector to, float t) {
    out[0] = glm_lerp(from.x, to.x, t);
    out[1] = glm_lerp(from.y, to.y, t);
}

static void lerp_f(vec2 out, vec2 from, vec2 to, float t) {
    out[0] = glm_lerp(from[0], to[0], t);
    out[1] = glm_lerp(from[1], to[1], t);
}

static void BezierInterpolation(vec2 out, vec2 p0, vec2 p1, vec2 p2, float t) {
    vec2 a1; {
        lerp_f(a1, p0, p1, t);
    }
    vec2 a2; {
        lerp_f(a2, p1, p2, t);
    }

    lerp_f(out, a1, a2, t);
}

static struct Vertex create(vec2 point, float div) {
    return (struct Vertex) {
        .pos = {
            point[0] / div,
            -1 * point[1] / div,
            0.0f
        },
        .texCoord = { 0.0f, 0.0f },
        .color = { 1.0f, 1.0f, 1.0f }
    };
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

void ttfLoadModel(const char *objectPath, struct actualModel *model, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    FT_Library library;
    FT_Face face;

    size_t resolution = 20;

    IF (0 == FT_Init_FreeType(&library), "No Library")
    IF (0 == FT_New_Face(library, objectPath, 0, &face), "No Face")
    IF (0 == FT_Set_Pixel_Sizes(face, 100, 100), "Size Error")
    IF (0 == FT_Load_Glyph(face, FT_Get_Char_Index(face, 'a'), FT_LOAD_NO_BITMAP), "No Glyph") {
        FT_GlyphSlot slot = face->glyph;
        FT_Outline *outline = &slot->outline;

        model->meshQuantity = 1;
        model->mesh = malloc(sizeof(struct Mesh) * model->meshQuantity);

        model->mesh->verticesQuantity = countPoints(outline) * resolution;
        model->mesh->indicesQuantity = model->mesh->verticesQuantity * 2;
        model->mesh->vertices = malloc(sizeof(struct Vertex) * model->mesh->verticesQuantity);
        model->mesh->indices = malloc(sizeof(uint16_t) * model->mesh->indicesQuantity);

        size_t z = 0;
        for (int i = 0; i < outline->n_contours; i += 1) {
            size_t start_point = (i == 0) ? 0 : outline->contours[i - 1] + 1;
            size_t end_point = outline->contours[i];

            size_t N = countP(end_point - start_point + 1, outline->tags + start_point);
            vec2 onLine[N];
            vec2 offLine[N];
            toArrays(start_point, end_point, outline, N, onLine, offLine);

            for (size_t j = 0; j < N; j += 1) {
                for (size_t k = 0; k < resolution; k += 1) {
                    float t = (float)k / resolution;
                    size_t id = z + resolution * j + k;
                    vec2 point; {
                        BezierInterpolation(point, onLine[j], offLine[j], onLine[(j + 1) % N], t);
                    }

                    model->mesh->vertices[id] = create(point, slot->metrics.height);
                    model->mesh->indices[2 * id + 0] = id + 0;
                    model->mesh->indices[2 * id + 1] = id + 1;
                }
            }
            model->mesh->indices[2 * (z + resolution * N) - 1] = z;
            z += N * resolution;
        }
    }

    createStorageBuffer(model->meshQuantity * sizeof(mat4), model->localMesh.buffers, model->localMesh.buffersMemory, model->localMesh.buffersMapped, device, physicalDevice, surface);

    for (uint32_t k = 0; k < MAX_FRAMES_IN_FLIGHT; k += 1) {
        glm_mat4_identity(((mat4 **)model->localMesh.buffersMapped)[k][0]);
    }
}
