#include <vulkan/vulkan.h>

#include "imageOperations.h"
#include "depthHelpers.h"

#include "MY_ASSERT.h"

void createColorResources(VkImage *colorImage, VkDeviceMemory *colorImageMemory, VkImageView *colorImageView, VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D swapChainExtent, VkFormat swapChainImageFormat, VkSampleCountFlagBits msaaSamples) {
    VkFormat colorFormat = swapChainImageFormat;

    *colorImage = createImage(
        device,
        swapChainExtent.width, swapChainExtent.height, 1,
        msaaSamples,
        colorFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        1,
        0
    );
    *colorImageMemory = createImageMemory(device, physicalDevice, *colorImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    *colorImageView = createImageView(device, *colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, VK_IMAGE_VIEW_TYPE_2D, 1);
}
