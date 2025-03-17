#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include <freetype/freetype.h>

#include "actualModel.h"
#include "myMalloc.h"

void ttfLoadModel(const char *objectPath, struct actualModel *model, [[maybe_unused]] VkDevice device, [[maybe_unused]] VkPhysicalDevice physicalDevice, [[maybe_unused]] VkSurfaceKHR surface) {
    FT_Library library;
    FT_Face face;

    FT_Init_FreeType(&library);
    FT_New_Face(library, objectPath, 0, &face);
    FT_Set_Pixel_Sizes(face, 0, 0);
    FT_UInt glyph_index = FT_Get_Char_Index(face, ',');

    FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_BITMAP);
    FT_GlyphSlot slot = face->glyph;

    FT_Outline* outline = &slot->outline;

    model->meshQuantity = outline->n_contours;

    model->mesh = malloc(sizeof(struct Mesh) * model->meshQuantity);
    for (int i = 0; i < outline->n_contours; i++) {
        size_t start_point = (i == 0) ? 0 : outline->contours[i - 1] + 1;
        size_t end_point = outline->contours[i];
        model->mesh[i].verticesQuantity = end_point - start_point + 1;
        model->mesh[i].indicesQuantity = model->mesh[i].verticesQuantity * 2;
        model->mesh[i].vertices = malloc(sizeof(struct Vertex) * model->mesh[i].verticesQuantity);
        model->mesh[i].indices = malloc(sizeof(uint16_t) * model->mesh[i].indicesQuantity);

        for (size_t j = start_point; j <= end_point; j += 1) {
            FT_Vector point = outline->points[j];
            model->mesh[i].vertices[j] = (struct Vertex) {
                .pos = {
                    (point.x / 100.f),
                    (point.y / 100.f),
                    0.0f
                },
                .texCoord = {
                    0.0f,
                    0.0f
                },
                .color = {
                    1.0f,
                    1.0f,
                    1.0f
                }
            };

            model->mesh[i].indices[2 * j + 0] = j + 0;
            model->mesh[i].indices[2 * j + 1] = j + 1;
        }
        model->mesh[i].indices[2 * end_point + 1] = start_point;
    }

    createStorageBuffer(model->meshQuantity * sizeof(mat4), model->localMesh.buffers, model->localMesh.buffersMemory, model->localMesh.buffersMapped, device, physicalDevice, surface);

    for (uint32_t k = 0; k < MAX_FRAMES_IN_FLIGHT; k += 1) {
        glm_mat4_identity(((mat4 **)model->localMesh.buffersMapped)[k][0]);
    }
}
