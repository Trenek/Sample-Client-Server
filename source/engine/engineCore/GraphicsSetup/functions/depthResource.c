#include <vulkan/vulkan.h>

#include "imageOperations.h"
#include "depthHelpers.h"

#include "MY_ASSERT.h"

void createDepthResources(VkImage *depthImage, VkDeviceMemory *depthImageMemory, VkImageView *depthImageView, VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D swapChainExtent, VkSampleCountFlagBits msaaSamples, VkCommandPool commandPool, VkQueue queue) {
    VkFormat depthFormat = findDepthFormat(physicalDevice);

    *depthImage = createImage(device, swapChainExtent.width, swapChainExtent.height, 1, msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 1, 0);
    *depthImageMemory = createImageMemory(device, physicalDevice, *depthImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    *depthImageView = createImageView(device, *depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, VK_IMAGE_VIEW_TYPE_2D, 1);

    transitionImageLayout(*depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, device, commandPool, queue, 1);
}
