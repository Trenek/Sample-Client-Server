#ifndef STRUCT_VERTEX
#define STRUCT_VERTEX

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <cglm.h>
#include <vulkan/vulkan_core.h>

struct Vertex {
    vec3 pos;
    vec3 color;
    vec2 texCoord;
};

[[maybe_unused]]
static VkVertexInputAttributeDescription vertexAttributeDescriptions[] = {
    [0] = {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(struct Vertex, pos)
    },
    [1] = {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(struct Vertex, color)
    },
    [2] = {
        .binding = 0,
        .location = 2,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(struct Vertex, texCoord)
    }
};

struct FontVertex {
    vec2 pos;
    vec3 color;
    vec2 bezzier;
    uint32_t inOut;
};

[[maybe_unused]]
static VkVertexInputAttributeDescription fontVertexAttributeDescriptions[] = {
    [0] = {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(struct FontVertex, pos)
    },
    [1] = {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(struct FontVertex, color)
    },
    [2] = {
        .binding = 0,
        .location = 2,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(struct FontVertex, bezzier)
    },
    [3] = {
        .binding = 0,
        .location = 3,
        .format = VK_FORMAT_R32_UINT,
        .offset = offsetof(struct FontVertex, inOut)
    }
};

#endif
