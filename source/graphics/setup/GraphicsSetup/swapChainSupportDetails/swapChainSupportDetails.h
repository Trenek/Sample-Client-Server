#include <vulkan/vulkan.h>

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR *formats;
    VkPresentModeKHR *presentModes;

    uint32_t formatCount;
    uint32_t presentModeCount;
};

struct SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
void freeSwapChainSupportDetails(struct SwapChainSupportDetails *this);
