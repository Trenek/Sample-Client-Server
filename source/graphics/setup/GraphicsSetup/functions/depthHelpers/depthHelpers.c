#include <vulkan/vulkan.h>

#include "MY_ASSERT.h"

static VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, size_t candidatesQuantity, VkFormat candidates[candidatesQuantity], VkImageTiling tiling, VkFormatFeatureFlags features) {
    VkFormat format = 0;
    size_t i = 0;

    while (format == 0 && i < candidatesQuantity) {
        VkFormatProperties props;

        vkGetPhysicalDeviceFormatProperties(physicalDevice, candidates[i], &props);
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            format = candidates[i];
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            format = candidates[i];
        }

        i += 1;
    }

    MY_ASSERT(0 != format);

    return format;
}

VkFormat findDepthFormat(VkPhysicalDevice physicalDevice) {
    VkFormat candidates[] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    return findSupportedFormat(
        physicalDevice,
        sizeof(candidates) / sizeof(VkFormat),
        candidates,
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

bool hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
           format == VK_FORMAT_D24_UNORM_S8_UINT;
}
