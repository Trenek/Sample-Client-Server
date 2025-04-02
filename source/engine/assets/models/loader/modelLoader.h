#include "actualModel.h"

struct GraphicsSetup;
struct actualModel;

void objLoadModel(const char *filePath, struct actualModel *model, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
void gltfLoadModel(const char *filePath, struct actualModel *model, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
void ttfLoadModel(const char *objectPath, struct actualModel *model, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
void ttfLoadModelOutline(const char *objectPath, struct actualModel *model, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

void loadModel(const char *filePath, struct actualModel *model, struct GraphicsSetup *vulkan);
void loadModels(size_t quantity, struct actualModel model[quantity], const char *modelPath[quantity], struct GraphicsSetup *vulkan);

void destroyActualModels(VkDevice device, uint32_t modelQuantity, struct actualModel *model);
