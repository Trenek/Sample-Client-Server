#include <vulkan/vulkan.h>

#include "imageOperations.h"

VkImageView createCubeMapImageView(VkDevice device, VkImage image, uint32_t mipmap) {
    return createImageView(device, image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipmap, VK_IMAGE_VIEW_TYPE_CUBE, 6);
}

VkImageView createTextureImageView(VkDevice device, VkImage image, uint32_t mipmap) {
    return createImageView(device, image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipmap, VK_IMAGE_VIEW_TYPE_2D, 1);
}
