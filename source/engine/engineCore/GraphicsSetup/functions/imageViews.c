#include <vulkan/vulkan.h>
#include <myMalloc.h>

#include "swapChain.h"
#include "imageOperations.h"
#include "swapChainSupportDetails.h"

VkImageView *createImageViews(VkDevice device, struct swapChain swapChain) {
    VkImageView *swapChainImageViews = malloc(sizeof(VkImageView) * swapChain.imagesCount);
    uint32_t i = 0;

    while (i < swapChain.imagesCount) {
        swapChainImageViews[i] = createImageView(device, swapChain.images[i], swapChain.imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, VK_IMAGE_VIEW_TYPE_2D, 1);

        i += 1;
    }

    return swapChainImageViews;
}

void destroyImageViews(VkImageView *swapChainImageViews, uint32_t swapChainImagesCount, VkDevice device) {
    uint32_t i = 0;

    while (i < swapChainImagesCount) {
        vkDestroyImageView(device, swapChainImageViews[i], NULL);

        i += 1;
    }

    free(swapChainImageViews);
}
