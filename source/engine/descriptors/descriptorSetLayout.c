#include <vulkan/vulkan.h>

#include <stdlib.h>

#include "descriptor.h"

struct descriptorSetLayout *createDescriptorSetLayout(VkDescriptorSetLayout layout, VkDevice device) {
    struct descriptorSetLayout *result = calloc(1, sizeof(struct descriptorSetLayout));

    *result = (struct descriptorSetLayout) {
        .descriptorSetLayout = layout,
        .device = device
    };

    return result;
}

void destroyDescriptorSetLayout(void *layoutPtr) {
    struct descriptorSetLayout *layout = layoutPtr;

    vkDestroyDescriptorSetLayout(layout->device, layout->descriptorSetLayout, NULL);
    free(layout);
} 
