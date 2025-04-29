#ifndef MODEL_BUILDER
#define MODEL_BUILDER
#include <vulkan/vulkan.h>

#include "definitions.h"

struct actualModel;
struct GraphicsSetup;

struct ModelBuilder {
    uint32_t instanceCount;

    size_t instanceSize;
    size_t instanceBufferSize;

    struct actualModel *modelData;

    VkDescriptorSetLayout objectLayout;
};

struct Entity *createModel(struct ModelBuilder builder, struct GraphicsSetup *vulkan);

#endif
