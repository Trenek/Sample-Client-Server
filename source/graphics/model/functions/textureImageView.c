#include <vulkan/vulkan.h>

#include "imageOperations.h"

VkImageView createTextureImageView(VkDevice device, VkImage image, uint32_t mipmap) {
    return createImageView(device, image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipmap);
}
