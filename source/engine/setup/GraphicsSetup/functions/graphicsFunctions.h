#include "bufferOperations.h"

#include "Vertex.h"
#include "definitions.h"

// setup
VkInstance createInstance(VkDebugUtilsMessengerEXT *debugMessenger);
VkSurfaceKHR createSurface(GLFWwindow *window, VkInstance instance);
VkPhysicalDevice pickPhysicalDevice(VkSampleCountFlagBits *msaaSamples, VkInstance instance, VkSurfaceKHR surface);
VkDevice createLogicalDevice(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice, VkQueue *graphicsQueue, VkQueue *presentQueue, VkQueue *transferQueue);
struct swapChain createSwapChain(GLFWwindow *window, VkSurfaceKHR surface, VkPhysicalDevice physicalDevice, VkDevice device);
VkImageView *createImageViews(VkDevice device, struct swapChain swapChain);

VkRenderPass createRenderPass(VkDevice device, VkPhysicalDevice physicalDevice, VkFormat swapChainImageFormat, VkSampleCountFlagBits msaaSamples);

VkCommandPool createCommandPool(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
void createCommandBuffer(VkCommandBuffer commandBuffer[MAX_FRAMES_IN_FLIGHT], VkDevice device, VkCommandPool commandPool);

VkCommandPool createTransferCommandPool(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

void createColorResources(VkImage *colorImage, VkDeviceMemory *colorImageMemory, VkImageView *colorImageView, VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D swapChainExtent, VkFormat swapChainImageFormat, VkSampleCountFlagBits msaaSamples);
void createDepthResources(VkImage *depthImage, VkDeviceMemory *depthImageMemory, VkImageView *depthImageView, VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D swapChainExtent, VkSampleCountFlagBits msaaSamples, VkCommandPool commandPool, VkQueue queue);
VkFramebuffer *createFramebuffers(VkDevice device, VkImageView *swapChainImageViews, uint32_t swapChainImagesCount, VkExtent2D swapChainExtent, VkRenderPass renderPass, VkImageView depthImageView, VkImageView colorImageView);

void createUniformBuffers(VkBuffer uniformBuffers[], VkDeviceMemory uniformBuffersMemory[], void *uniformBuffersMapped[], VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

VkSemaphore createSemaphore(VkDevice device);
VkFence createFence(VkDevice device);

// cleanup
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);
void destroyImageViews(VkImageView *swapChainImageViews, uint32_t swapChainImagesCount, VkDevice device);
void destroyFramebuffers(VkDevice device, VkFramebuffer *swapChainFramebuffers, uint32_t swapChainImagesCount);
void destroyUniformBuffers(VkDevice device, VkBuffer uniformBuffers[], VkDeviceMemory uniformBuffersMemory[]);

