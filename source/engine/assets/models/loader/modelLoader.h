#include "actualModel.h"

struct GraphicsSetup;
struct actualModel;

void objLoadModel(const char *filePath, struct actualModel *model, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
void gltfLoadModel(const char *filePath, struct actualModel *model, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
void ttfLoadModel(const char *objectPath, struct actualModel *model, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
void ttfLoadModelOutline(const char *objectPath, struct actualModel *model, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

struct actualModel *loadModel(const char *filePath, struct GraphicsSetup *vulkan);

void destroyActualModel(void *modelPtr);
