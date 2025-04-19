#include <string.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <vulkan/vulkan.h>

#include "MY_ASSERT.h"
#include "bufferOperations.h"
#include "imageOperations.h"

#define MAX(x, y) ((x) > (y) ? (x) : (y))

// transfer command and queue
VkImage createTextureBuffer(VkDeviceMemory *textureImageMemory, uint32_t *mipLevels, const char *texturePath, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkCommandPool commandPool, VkQueue queue) {
    VkImage textureImage = NULL;

    int width = 0;
    int height = 0;
    int channels = 0;

    stbi_uc *pixels = stbi_load(texturePath, &width, &height, &channels, STBI_rgb_alpha);
    *mipLevels = floor(log2(MAX(width, height))) + 1;
    VkDeviceSize imageSize = width * height * 4;

    MY_ASSERT(NULL != pixels);

    VkBuffer stagingBuffer = createBuffer(device, physicalDevice, surface, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    VkDeviceMemory stagingBufferMemory = createBufferMemory(device, physicalDevice, stagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void *data;
    vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, imageSize);
    vkUnmapMemory(device, stagingBufferMemory);

    stbi_image_free(pixels);

    textureImage = createImage(
        device,
        width, height, *mipLevels, VK_SAMPLE_COUNT_1_BIT,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        1,
        0
    );
    *textureImageMemory = createImageMemory(device, physicalDevice, textureImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, *mipLevels, device, commandPool, queue, 1);
    copyBufferToImage(stagingBuffer, textureImage, width, height, device, commandPool, queue, 1);
    //transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, *mipLevels, device, commandPool, queue);
    generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_SRGB, width, height, *mipLevels, device, physicalDevice, commandPool, queue);


    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);

    return textureImage;
}

VkImage createCubeMapTexture(VkDeviceMemory *textureImageMemory, uint32_t *mipLevels, const char *texturePath[6], VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkCommandPool commandPool, VkQueue queue) {
    VkImage textureImage = NULL;

    int width = 0;
    int height = 0;
    int channels = 0;

    stbi_uc *pixels[6] = {};

    for (int i = 0; i < 6; i += 1) {
        pixels[i] = stbi_load(texturePath[i], &width, &height, &channels, STBI_rgb_alpha);
    }
    VkDeviceSize layerSize = width * height * 4;
    VkDeviceSize imageSize = 6 * layerSize;

    *mipLevels = 1;

    for (int i = 0; i < 6; i += 1) {
        MY_ASSERT(NULL != pixels[i]);
    }

    VkBuffer stagingBuffer = createBuffer(device, physicalDevice, surface, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    VkDeviceMemory stagingBufferMemory = createBufferMemory(device, physicalDevice, stagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void *data;
    vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
    for (int i = 0; i < 6; i += 1) {
        memcpy((char *)data + i * layerSize, pixels[i], layerSize);
    }
    vkUnmapMemory(device, stagingBufferMemory);

    for (int i = 0; i < 6; i += 1) {
        stbi_image_free(pixels[i]);
    }

    textureImage = createImage(
        device,
        width, height, *mipLevels, VK_SAMPLE_COUNT_1_BIT,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        6,
        VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
    );
    *textureImageMemory = createImageMemory(device, physicalDevice, textureImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, *mipLevels, device, commandPool, queue, 6);
    copyBufferToImage(stagingBuffer, textureImage, width, height, device, commandPool, queue, 6);
    transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, *mipLevels, device, commandPool, queue, 6);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);

    return textureImage;
}
