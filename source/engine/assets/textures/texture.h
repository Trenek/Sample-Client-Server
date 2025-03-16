#include "descriptor.h"
struct GraphicsSetup;

struct Textures {
    struct descriptor descriptor;

    size_t quantity;
    struct Data {
        VkImage image;
        VkDeviceMemory imageMemory;
        uint32_t mipLevels;
        VkImageView imageView;
        VkSampler sampler;
    } *data;
};

struct Textures loadTextures(struct GraphicsSetup *vulkan, uint32_t texturesQuantity, const char *texturePath[static texturesQuantity]);
void unloadTextures(VkDevice device, struct Textures texture);
