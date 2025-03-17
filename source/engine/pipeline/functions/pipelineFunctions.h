#include <vulkan/vulkan_core.h>

VkPipelineLayout createPipelineLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSetLayout descriptorSetLayout2, VkDescriptorSetLayout descriptorSetLayout3);
VkPipeline createGraphicsPipeline(const char *vertexShader, const char *fragmentShader, float minDepth, float maxDepth, VkDevice device, VkRenderPass renderPass, VkPipelineLayout pipelineLayout, VkSampleCountFlagBits msaaSamples, VkPrimitiveTopology topology);
