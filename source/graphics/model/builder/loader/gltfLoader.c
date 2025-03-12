#define CGLTF_IMPLEMENTATION
#include <vulkan/vulkan.h>
#include <cgltf.h>
#include <string.h>

#include "Vertex.h"
#include "modelBuilder.h"
#include "model.h"
#include "modelFunctions.h"

static void *getBufferOffset(cgltf_buffer_view *bufferView) {
    return (uint8_t *)bufferView->buffer->data + bufferView->offset;
}

static void *getAccessorOffset(cgltf_accessor *accessor) {
    return (uint8_t *)getBufferOffset(accessor->buffer_view) + accessor->offset;
}

static void applySparce(cgltf_accessor *accessor, void *dest, size_t size) {
    uint16_t *indices = NULL;
    uint8_t *newVal = NULL;

    if (accessor->is_sparse) {
        indices = getBufferOffset(accessor->sparse.indices_buffer_view);
        newVal = getBufferOffset(accessor->sparse.values_buffer_view);

        for (uint16_t i = 0; i < accessor->sparse.count; i += 1) {
            memcpy((uint8_t *)dest + size * indices[i], newVal + size * i, size);
        }
    }
}

static int countMeshes(uint16_t n, cgltf_node x[n]) {
    int quantity = 0;

    for (uint16_t i = 0; i < n; i += 1) {
        if (x[i].mesh != NULL) {
            quantity += 1;
        }
    }

    return quantity;
}

cgltf_accessor *getAccessor(cgltf_attribute_type type, cgltf_primitive* primitive) {
    cgltf_accessor *result = NULL;

    for (size_t i = 0; result == NULL && i < primitive->attributes_count; i += 1) {
        if (primitive->attributes[i].type == type) {
            result = primitive->attributes[i].data;
        }
    }

    return result;
}

void loadFromAccessor(cgltf_accessor *accessor, void *local, size_t size, uint16_t quantity) {
    if (accessor != NULL) {
        memcpy(local, getAccessorOffset(accessor), size * quantity);
        applySparce(accessor, local, size);
    }
}

static struct Mesh loadMesh(cgltf_mesh *mesh) {
    struct Mesh result = { 0 };
    cgltf_primitive* primitive = mesh->primitives;

    cgltf_accessor *index_accessor = primitive->indices;
    cgltf_accessor *vertex_accessor = getAccessor(cgltf_attribute_type_position, primitive);
    cgltf_accessor *texture_accessor = getAccessor(cgltf_attribute_type_texcoord, primitive);
    cgltf_accessor *color_accessor = getAccessor(cgltf_attribute_type_color, primitive);

    result.indicesQuantity = index_accessor->count;
    result.verticesQuantity = vertex_accessor->count;
    result.vertices = malloc(sizeof(struct Vertex) * result.verticesQuantity);
    result.indices = malloc(sizeof(uint16_t) * result.indicesQuantity);

    float localPosition[result.verticesQuantity][3];
    float localTexture[result.verticesQuantity][2];
    float localColor[result.verticesQuantity][3];

    loadFromAccessor(index_accessor, result.indices, sizeof(uint16_t), result.indicesQuantity);
    loadFromAccessor(vertex_accessor, localPosition, sizeof(float[3]), result.verticesQuantity);
    loadFromAccessor(texture_accessor, localTexture, sizeof(float[2]), result.verticesQuantity);
    loadFromAccessor(color_accessor, localColor, sizeof(float[3]), result.verticesQuantity);

    for (size_t i = 0; i < result.verticesQuantity; i += 1) {
        result.vertices[i] = (struct Vertex) {
            .pos = {
                [0] = vertex_accessor == NULL ? 0.0f : localPosition[i][0],
                [1] = vertex_accessor == NULL ? 0.0f : localPosition[i][1],
                [2] = vertex_accessor == NULL ? 0.0f : localPosition[i][2]
            },
            .texCoord = {
                [0] = texture_accessor == NULL ? 0.0f : localTexture[i][0],
                [1] = texture_accessor == NULL ? 0.0f : localTexture[i][1]
            },
            .color = {
                [0] = color_accessor == NULL ? 1.0f : localColor[i][0],
                [1] = color_accessor == NULL ? 1.0f : localColor[i][1],
                [2] = color_accessor == NULL ? 1.0f : localColor[i][2]
            }
        };
    }

    return result;
}

void loadTransformations(mat4 transformations, cgltf_node *node) {
    glm_mat4_identity(transformations);

    if (node->has_matrix) {
        memcpy(transformations, node->matrix, sizeof(mat4));
    }

    if (node->has_translation) {
        glm_translate(transformations, node->translation);
    }

    if (node->has_rotation) {
        float a[3] = {
            node->rotation[0],
            node->rotation[1],
            node->rotation[2]
        };

        glm_rotate(transformations, acosf(node->rotation[3]) * 2, a);
    }

    if (node->has_scale) {
        glm_scale(transformations, node->scale);
    }
}

static void loadModel(const char *filePath, struct actualModel *model, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    cgltf_options options = { 0 };
    cgltf_data *data = NULL;

    if (cgltf_result_success == cgltf_parse_file(&options, filePath, &data))
    if (cgltf_result_success == cgltf_load_buffers(&options, data, filePath)) {
        model->meshQuantity = countMeshes(data->nodes_count, data->nodes);
        model->mesh = malloc(sizeof(struct Mesh) * model->meshQuantity);

        createStorageBuffer(model->meshQuantity * sizeof(mat4), model->localMesh.buffers, model->localMesh.buffersMemory, model->localMesh.buffersMapped, device, physicalDevice, surface);

        int i = 0;
        for (uint32_t j = 0; j < data->nodes_count; j += 1) if (data->nodes[j].mesh != NULL) {
            model->mesh[i] = loadMesh(data->nodes[j].mesh);
            for (uint32_t k = 0; k < MAX_FRAMES_IN_FLIGHT; k += 1) {
                loadTransformations(((mat4 **)model->localMesh.buffersMapped)[k][i], &data->nodes[j]);
            }

            i += 1;
        }

        /*
        cgltf_accessor *time_accessor = data->animations[0].channels[0].sampler->input;
        cgltf_accessor *rotation_accessor = data->animations[0].channels[0].sampler->output;

        uint16_t timeCount = time_accessor->count;
        uint16_t rotationCount = rotation_accessor->count;

        float *time = getAccessorOffset(time_accessor);
        float (*rotation)[4] = getAccessorOffset(rotation_accessor);

        for (uint16_t i = 0; i < timeCount; i += 1) {
            printf("%f\n", time[i]);
        }

        for (uint16_t i = 0; i < rotationCount; i += 1) {
            printf("%f %f %f %f\n", rotation[i][0], rotation[i][1], rotation[i][2], rotation[i][3]);
        }
        */

        cgltf_free(data);
    }
}

void gltfLoadModel(const char *filePath, struct actualModel *model, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    loadModel(filePath, model, device, physicalDevice, surface);
}
