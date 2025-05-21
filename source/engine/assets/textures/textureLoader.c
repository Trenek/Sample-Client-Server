#include "graphicsSetup.h"

#include "myMalloc.h"
#include "textureFunctions.h"
#include "descriptor.h"

VkImage createCubeMapTexture(VkDeviceMemory *textureImageMemory, uint32_t *mipLevels, const char *texturePath[6], VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkCommandPool commandPool, VkQueue queue);

VkImageView createCubeMapImageView(VkDevice device, VkImage image, uint32_t mipmap);
static struct Data loadCubeMap(const char *texturePath[6], VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkCommandPool commandPool, VkQueue queue) {
    struct Data result = { 0 };

    result.image = createCubeMapTexture(&result.imageMemory, &result.mipLevels, texturePath, device, physicalDevice, surface, commandPool, queue);
    result.imageView = createCubeMapImageView(device, result.image, result.mipLevels);
    result.sampler = createTextureSampler(device, physicalDevice, result.mipLevels);

    return result;
}

struct Textures *loadCubeMaps(struct GraphicsSetup *vulkan, const char *texturePath[6]) {
    struct Textures *texture = calloc(1, sizeof(struct Textures));
    *texture = (struct Textures){
        .device = &vulkan->device,
        .data = calloc(1, sizeof(struct Data)),
        .quantity = 1,
        .descriptor = {
            .descriptorSetLayout = createTextureDescriptorSetLayout(vulkan->device, 1),
            .descriptorPool = createTextureDescriptorPool(vulkan->device, 1)
        }
    };
    struct descriptor *desc = &texture->descriptor;

    texture->data[0] = loadCubeMap(texturePath, vulkan->device, vulkan->physicalDevice, vulkan->surface, vulkan->commandPool, vulkan->transferQueue);

    createDescriptorSets(desc->descriptorSets, vulkan->device, desc->descriptorPool, desc->descriptorSetLayout);
    bindTextureBuffersToDescriptorSets(desc->descriptorSets, vulkan->device, 1, texture);

    return texture;
}

static struct Data loadTexture(const char *texturePath, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkCommandPool commandPool, VkQueue queue) {
    struct Data result = { 0 };

    result.image = createTextureBuffer(&result.imageMemory, &result.mipLevels, texturePath, device, physicalDevice, surface, commandPool, queue);
    result.imageView = createTextureImageView(device, result.image, result.mipLevels);
    result.sampler = createTextureSampler(device, physicalDevice, result.mipLevels);

    return result;
}

struct Textures *loadTextures(struct GraphicsSetup *vulkan, uint32_t texturesQuantity, const char *texturePath[static texturesQuantity]) {
    struct Textures *texture = calloc(1, sizeof(struct Textures));
    *texture = (struct Textures) {
        .device = &vulkan->device,
        .data = calloc(texturesQuantity, sizeof(struct Data)),
        .quantity = texturesQuantity,
        .descriptor = {
            .descriptorSetLayout = createTextureDescriptorSetLayout(vulkan->device, texturesQuantity),
            .descriptorPool = createTextureDescriptorPool(vulkan->device, texturesQuantity)
        }
    };
    struct descriptor *desc = &texture->descriptor;

    for (uint32_t i = 0; i < texturesQuantity; i += 1) {
        texture->data[i] = loadTexture(texturePath[i], vulkan->device, vulkan->physicalDevice, vulkan->surface, vulkan->commandPool, vulkan->transferQueue);
    }

    createDescriptorSets(desc->descriptorSets, vulkan->device, desc->descriptorPool, desc->descriptorSetLayout);
    bindTextureBuffersToDescriptorSets(desc->descriptorSets, vulkan->device, texturesQuantity, texture);

    return texture;
}

void unloadTextures(void *texturePtr) {
    struct Textures *texture = texturePtr;
    for (uint32_t i = 0; i < texture->quantity; i += 1) {
        vkDestroySampler(*texture->device, texture->data[i].sampler, NULL);
        vkDestroyImageView(*texture->device, texture->data[i].imageView, NULL);

        vkDestroyImage(*texture->device, texture->data[i].image, NULL);
        vkFreeMemory(*texture->device, texture->data[i].imageMemory, NULL);
    }

    free(texture->data);

    vkDestroyDescriptorPool(*texture->device, texture->descriptor.descriptorPool, NULL);
    vkDestroyDescriptorSetLayout(*texture->device, texture->descriptor.descriptorSetLayout, NULL);

    free(texture);
}
