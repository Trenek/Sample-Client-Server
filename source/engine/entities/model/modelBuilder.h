#ifndef MODEL_BUILDER
#define MODEL_BUILDER
#include <vulkan/vulkan.h>

#include "definitions.h"

struct actualModel;

struct ModelBuilder {
    uint32_t texturePointer;
    uint32_t texturesQuantity;

    uint32_t instanceCount;

    uint32_t meshQuantity;
    struct Mesh *mesh;
    VkBuffer (*buffers)[MAX_FRAMES_IN_FLIGHT];

    VkDescriptorSetLayout objectLayout;
};

#endif
