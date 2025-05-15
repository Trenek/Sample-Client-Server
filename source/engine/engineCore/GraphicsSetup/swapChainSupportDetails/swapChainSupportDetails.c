#include <myMalloc.h>
#include <vulkan/vulkan.h>

#include "swapChainSupportDetails.h"

struct SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
    struct SwapChainSupportDetails details = { 0 };

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.formatCount, NULL);
    details.formats = malloc(sizeof(VkSurfaceFormatKHR) * details.formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.formatCount, details.formats);

    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.presentModeCount, NULL);
    details.presentModes = malloc(sizeof(VkPresentModeKHR) * details.presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.presentModeCount, details.presentModes);

    return details;
}

void freeSwapChainSupportDetails(struct SwapChainSupportDetails *this) {
    free(this->formats);
    free(this->presentModes);
}
