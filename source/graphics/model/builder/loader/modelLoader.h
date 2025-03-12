#include "vulkan/vulkan.h"

struct ModelBuilder gltfLoader(struct ModelBuilder a);
struct ModelBuilder objLoader(struct ModelBuilder a);

struct actualModel;

void objLoadModel(const char *filePath, struct actualModel *model, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

void gltfLoadModel(const char *filePath, struct actualModel *model, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
