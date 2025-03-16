#ifndef MODEL_BUILDER
#define MODEL_BUILDER
#include <vulkan/vulkan.h>

struct actualModel;

struct ModelBuilder {
    uint32_t texturePointer;
    uint32_t texturesQuantity;

    uint32_t instanceCount;

    struct actualModel *modelPath;

    VkDescriptorSetLayout objectLayout;
};

#endif
