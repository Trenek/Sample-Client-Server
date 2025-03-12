#ifndef MODEL_BUILDER
#define MODEL_BUILDER
#include <vulkan/vulkan.h>

struct Model;
struct actualModel;
struct Textures;

struct ModelBuilder {
    struct descriptor *texture;
    uint32_t texturePointer;
    uint32_t texturesQuantity;

    const char *vertexShader;
    const char *fragmentShader;

    uint32_t instanceCount;

    float minDepth;
    float maxDepth;

    struct actualModel *modelPath;

    VkDescriptorSetLayout (*createDescriptorSetLayout)(VkDevice device);
    VkDescriptorPool (*createDescriptorPool)(VkDevice device);
    void (*bindBuffersToDescriptorSets)(VkDescriptorSet descriptorSets[], VkDevice device, VkBuffer uniformBuffers[], struct Model model);
};


#include "modelLoader.h"

VkDescriptorPool createTextureDescriptorPool(VkDevice device, uint32_t texturesCount);
VkDescriptorSetLayout createTextureDescriptorSetLayout(VkDevice device, uint32_t textureQuantity);
void bindTextureBuffersToDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, uint32_t texturesQuantity, struct Textures *texture);

VkDescriptorPool createObjectDescriptorPool(VkDevice device);
VkDescriptorSetLayout createObjectDescriptorSetLayout(VkDevice device);
void bindObjectBuffersToDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, struct Model model, struct actualModel *actualModel);

VkDescriptorPool createCameraDescriptorPool(VkDevice device);
VkDescriptorSetLayout createCameraDescriptorSetLayout(VkDevice device);
void bindCameraBuffersToDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, VkBuffer uniformBuffers[]);

#endif
