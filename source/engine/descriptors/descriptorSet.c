#include <vulkan/vulkan.h>

#include "MY_ASSERT.h"
#include "definitions.h"

static void fillArray(int num, VkDescriptorSetLayout layouts[], VkDescriptorSetLayout elem) {
    while (num --> 0) {
        layouts[num] = elem;
    }
}

void createDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout) {
    VkDescriptorSetLayout layouts[MAX_FRAMES_IN_FLIGHT]; {
        fillArray(MAX_FRAMES_IN_FLIGHT, layouts, descriptorSetLayout);
    };
    VkDescriptorSetAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = MAX_FRAMES_IN_FLIGHT,
        .pSetLayouts = layouts
    };

    MY_ASSERT(VK_SUCCESS == vkAllocateDescriptorSets(device, &allocInfo, descriptorSets));
}
