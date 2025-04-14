#ifndef STRING_BUILDER
#define STRING_BUILDER
#include <vulkan/vulkan.h>

struct actualModel;

struct StringBuilder {
    uint32_t instanceCount;

    const char *string;
    struct actualModel *modelData;

    VkDescriptorSetLayout objectLayout;
};

#endif
