#define CGLTF_IMPLEMENTATION

#include <vulkan/vulkan.h>
#include <cglm.h>
#include <cgltf.h>
#include <string.h>

#include "actualModel.h"

#include "Vertex.h"

#define BFR(x) ((struct AnimVertex *)(x))

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

static cgltf_accessor *getAccessor(cgltf_attribute_type type, cgltf_primitive* primitive) {
    cgltf_accessor *result = NULL;

    for (size_t i = 0; result == NULL && i < primitive->attributes_count; i += 1) {
        if (primitive->attributes[i].type == type) {
            result = primitive->attributes[i].data;
        }
    }

    return result;
}

static void loadFromAccessor(cgltf_accessor *accessor, void *local, size_t size, uint16_t quantity) {
    if (accessor != NULL) {
        memcpy(local, getAccessorOffset(accessor), size * quantity);
        applySparce(accessor, local, size);
    }
}

static struct Mesh loadMesh(cgltf_mesh *mesh) {
    struct Mesh result = { 0 };
    cgltf_primitive *primitive = mesh->primitives;
    
    cgltf_accessor *index_accessor = primitive->indices;
    cgltf_accessor *vertex_accessor = getAccessor(cgltf_attribute_type_position, primitive);
    cgltf_accessor *texture_accessor = getAccessor(cgltf_attribute_type_texcoord, primitive);
    cgltf_accessor *color_accessor = getAccessor(cgltf_attribute_type_color, primitive);
    cgltf_accessor *joint_accessor = getAccessor(cgltf_attribute_type_joints, primitive);
    cgltf_accessor *weight_accessor = getAccessor(cgltf_attribute_type_weights, primitive);

    result.indicesQuantity = index_accessor->count;
    result.verticesQuantity = vertex_accessor->count;
    result.vertices = malloc(sizeof(struct AnimVertex) * result.verticesQuantity);
    result.indices = malloc(sizeof(uint16_t) * result.indicesQuantity);

    float localPosition[result.verticesQuantity][3];
    float localTexture[result.verticesQuantity][2];
    float localColor[result.verticesQuantity][3];

    loadFromAccessor(index_accessor, result.indices, sizeof(uint16_t), result.indicesQuantity);
    loadFromAccessor(vertex_accessor, localPosition, sizeof(float[3]), result.verticesQuantity);
    loadFromAccessor(texture_accessor, localTexture, sizeof(float[2]), result.verticesQuantity);
    loadFromAccessor(color_accessor, localColor, sizeof(float[3]), result.verticesQuantity);

    for (size_t i = 0; i < result.verticesQuantity; i += 1) {
        cgltf_uint joints[4] = {};
        cgltf_float weights[4] = {};
        if (joint_accessor && weight_accessor) {
            cgltf_accessor_read_uint(joint_accessor, i, joints, 4);
            cgltf_accessor_read_float(weight_accessor, i, weights, 4);
        }

        BFR(result.vertices)[i] = (struct AnimVertex) {
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
            },
            .bone = { joints[0], joints[1], joints[2], joints[3] },
            .weights = { weights[0], weights[1], weights[2], weights[3] }
        };
    }

    return result;
}

static void loadTransformations(mat4 transformations, cgltf_node *node) {
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

struct timeFrame importantThings(cgltf_animation_sampler *sampler) {
    struct timeFrame result = {};

    cgltf_accessor *input = sampler->input;
    cgltf_accessor *output = sampler->output;

    cgltf_size num_components = cgltf_num_components(output->type);

    result.qData = input->count;
    result.qValues = num_components;
    result.interpolationType = sampler->interpolation;

    result.data = malloc(sizeof(struct { float a; float * b; }) * input->count);
    result.data->values = malloc(sizeof(float) * input->count * num_components);

    for (cgltf_size k = 0; k < input->count; k++) {
        result.data[k].values = result.data->values + k * num_components;

        assert(cgltf_accessor_read_float(input, k, &result.data[k].time, 1));
        assert(cgltf_accessor_read_float(output, k, result.data[k].values, num_components));
    }

    return result;
}

static cgltf_size getNodeID(cgltf_data *data, cgltf_node *node) {
    cgltf_size result = 0;

    while (result < data->skins->joints_count && node != data->skins->joints[result]) result += 1;

    return (result < data->skins->joints_count) ? result : SIZE_MAX;
}

void loadAnimation(cgltf_data *data, struct jointData foo2[data->animations_count][data->skins->joints_count]) {
    for (cgltf_size i = 0; i < data->animations_count; i++) {
        cgltf_animation *animation = &data->animations[i];
        cgltf_accessor *accessor = data->skins->inverse_bind_matrices;
        mat4 inverseMatrix[data->skins->joints_count];

        loadFromAccessor(accessor, inverseMatrix, sizeof(mat4), data->skins->joints_count);

        for (cgltf_size j = 0; j < animation->channels_count; j++) {
            cgltf_animation_channel *channel = &animation->channels[j];
            cgltf_node *node = channel->target_node;

            cgltf_size k = getNodeID(data, node);
            if (k != SIZE_MAX) {
                foo2[i][k].isJoint = true;
                foo2[i][k].t[channel->target_path - 1] = importantThings(channel->sampler);

                if (channel->target_path == cgltf_animation_path_type_rotation &&
                    foo2[i][k].t[channel->target_path - 1].interpolationType == cgltf_interpolation_type_linear) {
                    foo2[i][k].t[channel->target_path - 1].interpolationType = 3;
                }
            }
        }

        for (cgltf_size j = 0; j < data->skins->joints_count; j += 1) {
            memcpy(foo2[i][j].inverseMatrix, inverseMatrix[j], sizeof(mat4));

            foo2[i][j].father = -1;
        }

        for (cgltf_size j = 0; j < data->skins->joints_count; j += 1) {
            if (foo2[i][j].isJoint)
            for (cgltf_size k = 0; k < data->skins->joints[j]->children_count; k += 1) {
                cgltf_size z = getNodeID(data, data->skins->joints[j]->children[k]);

                if (z != SIZE_MAX) {
                    if (foo2[i][z].isJoint) {
                        foo2[i][z].father = j;
                    }
                }
            }
        }
    }
}

void gltfLoadModel(const char *filePath, struct actualModel *model, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    cgltf_options options = { 0 };
    cgltf_data *data = NULL;

    if (cgltf_result_success == cgltf_parse_file(&options, filePath, &data))
    if (cgltf_result_success == cgltf_load_buffers(&options, data, filePath)) {
        model->meshQuantity = countMeshes(data->nodes_count, data->nodes);
        model->mesh = malloc(sizeof(struct Mesh) * model->meshQuantity);
        model->qAnim = 0;
        model->qJoint = 0;
        model->anim = NULL;

        createStorageBuffer(model->meshQuantity * sizeof(mat4), model->localMesh.buffers, model->localMesh.buffersMemory, model->localMesh.buffersMapped, device, physicalDevice, surface);

        int i = 0;
        for (uint32_t j = 0; j < data->nodes_count; j += 1) if (data->nodes[j].mesh != NULL) {
            model->mesh[i] = loadMesh(data->nodes[j].mesh);
            model->mesh[i].sizeOfVertex = sizeof(struct AnimVertex);

            for (uint32_t k = 0; k < MAX_FRAMES_IN_FLIGHT; k += 1) {
                loadTransformations(((mat4 **)model->localMesh.buffersMapped)[k][i], &data->nodes[j]);
            }

            i += 1;
        }

        if (data->animations_count != 0) {
            model->qAnim = data->animations_count;
            model->qJoint = data->skins->joints_count;
            model->anim = calloc(model->qAnim * model->qJoint, sizeof(struct jointData));

            loadAnimation(data, model->anim);
        }

        cgltf_free(data);
    }
}
