#include <GLFW/glfw3.h>

#include "swapChain.h"

#include "myMalloc.h"
#include "MY_ASSERT.h"
#include "swapChainSupportDetails.h"
#include "queueFamilyIndices.h"

#define BETWEEN(x, MIN, MAX) (x) >= (MIN) && (x) <= (MAX)
#define CLOSEST_A(x, MIN, MAX) ((MAX) - (x) > (x) - (MIN)) ? (MIN) : (MAX)
#define CLOSEST(x, MIN, MAX) BETWEEN(x, MIN, MAX) ? x : CLOSEST_A(x, MIN, MAX)

static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const VkSurfaceFormatKHR *availableFormats, const uint32_t availableFormatsQuantity) {
    uint32_t i = 0;

    while (i < availableFormatsQuantity &&
        !(availableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)) {
        i += 1;
    }

    return availableFormats[i == availableFormatsQuantity ? 0 : i];
}

static VkPresentModeKHR chooseSwapPresentMode(const VkPresentModeKHR *availablePresentModes, const uint32_t availablePresentModesQuantity) {
    uint32_t i = 0;

    while (i < availablePresentModesQuantity && availablePresentModes[i] != VK_PRESENT_MODE_MAILBOX_KHR) {
        i += 1;
    }

    return i != availablePresentModesQuantity ? availablePresentModes[i] : VK_PRESENT_MODE_FIFO_KHR; // second value guaranteed to be available
}

static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR *capabilities, GLFWwindow *window) {
    VkExtent2D result = capabilities->currentExtent;
    int width = 0;
    int height = 0;

    if (result.width == UINT32_MAX) {
        glfwGetFramebufferSize(window, &width, &height);

        result = (VkExtent2D){
            .width = (uint32_t)width,
            .height = (uint32_t)height
        };

        result = (VkExtent2D){
            .width = CLOSEST(result.width, capabilities->minImageExtent.width, capabilities->maxImageExtent.width),
            .height = CLOSEST(result.height, capabilities->minImageExtent.height, capabilities->maxImageExtent.height)
        };
    }

    return result;
}

struct swapChain createSwapChain(GLFWwindow *window, VkSurfaceKHR surface, VkPhysicalDevice physicalDevice, VkDevice device) {
    struct swapChain swapChain = { 0 };

    struct SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats, swapChainSupport.formatCount);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes, swapChainSupport.presentModeCount);
    VkExtent2D extent = chooseSwapExtent(&swapChainSupport.capabilities, window);

    uint32_t imageCount = (swapChainSupport.capabilities.maxImageCount > 0 && swapChainSupport.capabilities.minImageCount + 1 > swapChainSupport.capabilities.maxImageCount) ?
        swapChainSupport.capabilities.maxImageCount :
        swapChainSupport.capabilities.minImageCount + 1;

    struct QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value, indices.presentFamily.value };

    VkSwapchainCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1, // 1 unless stereoscopic 3D application
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, // VK_IMAGE_USAGE_TRANSFER_DST_BIT for postprocessing
        .imageSharingMode = familyEqual(indices.graphicsFamily, indices.presentFamily) ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount = familyEqual(indices.graphicsFamily, indices.presentFamily) ? 0 : 2, // 0 optional
        .pQueueFamilyIndices = familyEqual(indices.graphicsFamily, indices.presentFamily) ? NULL : queueFamilyIndices, // NULL optional
        .preTransform = swapChainSupport.capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };

    MY_ASSERT(VK_SUCCESS == vkCreateSwapchainKHR(device, &createInfo, NULL, &swapChain.this));

    vkGetSwapchainImagesKHR(device, swapChain.this, &swapChain.imagesCount, NULL);
    swapChain.images = malloc(sizeof(VkImage) * imageCount);
    vkGetSwapchainImagesKHR(device, swapChain.this, &swapChain.imagesCount, swapChain.images);
    freeSwapChainSupportDetails(&swapChainSupport);

    swapChain.imageFormat = surfaceFormat.format;
    swapChain.extent = extent;

    return swapChain;
}

void freeSwapChain(VkDevice device, struct swapChain *swapChain) {
    vkDestroySwapchainKHR(device, swapChain->this, NULL);

    free(swapChain->images);
}
