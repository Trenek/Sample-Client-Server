#include <vulkan/vulkan_core.h>

struct graphicsPipelineBuilder;

VkPipelineLayout createPipelineLayout(VkDevice device, size_t qDescriptorLayout, VkDescriptorSetLayout descriptorSetLayout[qDescriptorLayout]);
VkPipeline createGraphicsPipeline(struct graphicsPipelineBuilder builder, VkDevice device, VkRenderPass renderPass, VkPipelineLayout pipelineLayout, VkSampleCountFlagBits msaaSamples);
