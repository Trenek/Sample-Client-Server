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

#endif
