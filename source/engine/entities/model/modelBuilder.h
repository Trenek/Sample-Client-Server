#ifndef MODEL_BUILDER
#define MODEL_BUILDER
#include <vulkan/vulkan.h>

#include "definitions.h"

struct actualModel;
struct GraphicsSetup;

struct ModelBuilder {
    uint32_t texturePointer;
    uint32_t texturesQuantity;

    uint32_t instanceCount;

    struct actualModel *modelData;

    VkDescriptorSetLayout objectLayout;
};

struct Entity createModel(struct ModelBuilder builder, struct GraphicsSetup *vulkan);

#endif
