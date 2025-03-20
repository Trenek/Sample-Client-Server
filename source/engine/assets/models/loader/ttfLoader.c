#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include <freetype/freetype.h>

#include "actualModel.h"
#include "myMalloc.h"

#define IF(x, y) if (!(x)) printf("%s", y); else

void ttfLoadModel(const char *objectPath, struct actualModel *model, [[maybe_unused]] VkDevice device, [[maybe_unused]] VkPhysicalDevice physicalDevice, [[maybe_unused]] VkSurfaceKHR surface) {
    FT_Library library;
    FT_Face face;

    IF (0 == FT_Init_FreeType(&library), "No Library")
    IF (0 == FT_New_Face(library, objectPath, 0, &face), "No Face")
    IF (0 == FT_Set_Pixel_Sizes(face, 1, 1), "Size Error")
    IF (0 == FT_Load_Glyph(face, FT_Get_Char_Index(face, 'a'), FT_LOAD_NO_BITMAP), "No Glyph") {
        FT_GlyphSlot slot = face->glyph;
        FT_Outline* outline = &slot->outline;

        model->meshQuantity = 1;
        model->mesh = malloc(sizeof(struct Mesh) * model->meshQuantity);

        model->mesh->verticesQuantity = outline->n_points;
        model->mesh->indicesQuantity = model->mesh->verticesQuantity * 2;
        model->mesh->vertices = malloc(sizeof(struct Vertex) * model->mesh->verticesQuantity);
        model->mesh->indices = malloc(sizeof(uint16_t) * model->mesh->indicesQuantity);

        for (int i = 0; i < outline->n_contours; i += 1) {
            size_t start_point = (i == 0) ? 0 : outline->contours[i - 1] + 1;
            size_t end_point = outline->contours[i];

            for (size_t j = start_point; j <= end_point; j += 1) {
                FT_Vector point = outline->points[j];
                model->mesh->vertices[j] = (struct Vertex) {
                    .pos = {
                        (float)point.x / slot->metrics.height,
                        -1 * (float)point.y / slot->metrics.height,
                        0.0f
                    },
                    .texCoord = { 0.0f, 0.0f },
                    .color = { 1.0f, 1.0f, 1.0f }
                };
                model->mesh->indices[2 * j + 0] = j + 0;
                model->mesh->indices[2 * j + 1] = j + 1;
            }
            model->mesh->indices[2 * end_point + 1] = start_point;
        }
    }

    createStorageBuffer(model->meshQuantity * sizeof(mat4), model->localMesh.buffers, model->localMesh.buffersMemory, model->localMesh.buffersMapped, device, physicalDevice, surface);

    for (uint32_t k = 0; k < MAX_FRAMES_IN_FLIGHT; k += 1) {
        glm_mat4_identity(((mat4 **)model->localMesh.buffersMapped)[k][0]);
    }
}
