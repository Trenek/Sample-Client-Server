#include "graphicsSetup.h"

#include "model.h"
#include "modelFunctions.h"
#include "modelBuilder.h"

static struct Textures loadTexture(const char *texturePath, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkCommandPool commandPool, VkQueue queue) {
    struct Textures result = { 0 };

    result.image = createTextureBuffer(&result.imageMemory, &result.mipLevels, texturePath, device, physicalDevice, surface, commandPool, queue);
    result.imageView = createTextureImageView(device, result.image, result.mipLevels);
    result.sampler = createTextureSampler(device, physicalDevice, result.mipLevels);

    return result;
}

struct Textures *loadTextures(struct descriptor *textureDescr, struct GraphicsSetup *vulkan, uint32_t texturesQuantity, const char *texturePath[static texturesQuantity]) {
    struct Textures *texture = malloc(texturesQuantity * sizeof(struct Textures));
    for (uint32_t i = 0; i < texturesQuantity; i += 1) {
        texture[i] = loadTexture(texturePath[i], vulkan->device, vulkan->physicalDevice, vulkan->surface, vulkan->commandPool, vulkan->transferQueue);
    }

    textureDescr->descriptorSetLayout = createTextureDescriptorSetLayout(vulkan->device, texturesQuantity);
    textureDescr->descriptorPool = createTextureDescriptorPool(vulkan->device, texturesQuantity);
    createDescriptorSets(textureDescr->descriptorSets, vulkan->device, textureDescr->descriptorPool, textureDescr->descriptorSetLayout);
    bindTextureBuffersToDescriptorSets(textureDescr->descriptorSets, vulkan->device, texturesQuantity, texture);

    return texture;
}

void unloadTextures(VkDevice device, uint32_t texturesQuantity, struct Textures texture[texturesQuantity], struct descriptor textureDesc) {
    for (uint32_t i = 0; i < texturesQuantity; i += 1) {
        vkDestroySampler(device, texture[i].sampler, NULL);
        vkDestroyImageView(device, texture[i].imageView, NULL);

        vkDestroyImage(device, texture[i].image, NULL);
        vkFreeMemory(device, texture[i].imageMemory, NULL);
    }

    free(texture);

    vkDestroyDescriptorPool(device, textureDesc.descriptorPool, NULL);
    vkDestroyDescriptorSetLayout(device, textureDesc.descriptorSetLayout, NULL);
}
