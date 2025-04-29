#ifndef STRING_BUILDER
#define STRING_BUILDER
#include <vulkan/vulkan.h>

struct actualModel;
struct GraphicsSetup;

struct StringBuilder {
    uint32_t instanceCount;

    size_t instanceSize;
    size_t instanceBufferSize;

    const char *string;
    struct actualModel *modelData;

    VkDescriptorSetLayout objectLayout;
};

struct Entity *createString(struct StringBuilder builder, struct GraphicsSetup *vulkan);

#endif
