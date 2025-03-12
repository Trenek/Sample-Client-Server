#ifndef MODEL_H
#define MODEL_H

#include <vulkan/vulkan.h>
#include <cglm.h>

#include "definitions.h"
#include "modelLoader.h"

struct Textures {
    const char *path;

    VkImage image;
    VkDeviceMemory imageMemory;
    uint32_t mipLevels;
    VkImageView imageView;
    VkSampler sampler;
};

struct Mesh {
    struct Vertex *vertices;
    uint16_t *indices;
    size_t verticesQuantity;
    size_t indicesQuantity;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
};

struct buffer {
    VkBuffer buffers[MAX_FRAMES_IN_FLIGHT];
    VkDeviceMemory buffersMemory[MAX_FRAMES_IN_FLIGHT];
    void *buffersMapped[MAX_FRAMES_IN_FLIGHT];
};

struct descriptor {
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSets[MAX_FRAMES_IN_FLIGHT];
};

struct actualModel {
    struct buffer localMesh;

    uint32_t meshQuantity;
    struct Mesh *mesh;
};

struct Model {
    uint32_t instanceCount;
    struct instance *instance;
    struct instanceBuffer *instanceBuffer;

    uint32_t texturePointer;
    uint32_t texturesQuantity;

    struct actualModel *actualModel;

    struct ModelGraphics {
        struct buffer uniformModel;

        struct descriptor object;
        struct descriptor *texture;

        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;
    } graphics;
};

struct GraphicsSetup;
struct Model createModels(struct ModelBuilder modelBuilder, struct GraphicsSetup *vulkan);

void destroyModelArray(uint16_t num, struct Model modelArray[num], struct GraphicsSetup *graphics);
void destroyActualModels(VkDevice device, uint32_t modelQuantity, struct actualModel *model);

struct Textures *loadTextures(struct descriptor *textureDescr, struct GraphicsSetup *vulkan, uint32_t texturesQuantity, const char *texturePath[static texturesQuantity]);
void unloadTextures(VkDevice device, uint32_t texturesQuantity, struct Textures texture[texturesQuantity], struct descriptor textureDesc);

#endif
