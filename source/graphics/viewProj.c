#include "VulkanTools.h"

#include "uniformBufferObject.h"

void updateUniformBuffer(void *uniformBuffersMapped, VkExtent2D swapChainExtent, vec3 cameraPos, vec3 result);

void getViewProj(struct VulkanTools vulkan, mat4 resultViewProj, vec4 resultViewPort) {
    vec4 viewPort = { 0, 0, vulkan.graphics.swapChain.extent.width, vulkan.graphics.swapChain.extent.height };

    mat4 viewProj; {
        struct UniformBufferObject ubo = { 0 };

        updateUniformBuffer(&ubo, vulkan.graphics.swapChain.extent, vulkan.camera.cameraPos, vulkan.camera.center);

        glm_mat4_mul(ubo.proj, ubo.view, viewProj);
    }

    glm_vec4_dup(viewPort, resultViewPort);
    glm_mat4_dup(viewProj, resultViewProj);
}
