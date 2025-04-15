#include <stdio.h>
#include <string.h>
#include <time.h>

#include "VulkanTools.h"

#include "actualModel.h"
#include "graphicsPipelineObj.h"
#include "uniformBufferObject.h"
#include "instanceBuffer.h"
#include "pushConstantsBuffer.h"

#include "MY_ASSERT.h"

void updateUniformBuffer(void *uniformBuffersMapped, VkExtent2D swapChainExtent, vec3 cameraPos, vec3 center) {
    struct UniformBufferObject ubo;

    glm_look_rh_no(cameraPos, center, (vec3) { 0.0f, 0.0f, 1.0f }, ubo.view);

    glm_perspective(glm_rad(45.0f), swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 10000.0f, ubo.proj);

    ubo.proj[1][1] *= -1;

    memcpy(uniformBuffersMapped, &ubo, sizeof(ubo));
}

static void recordCommandBuffer(VkCommandBuffer commandBuffer, VkFramebuffer swapChainFramebuffer, VkExtent2D swapChainExtent, struct VulkanTools *vulkan, uint32_t currentFrame, uint16_t modelQuantity, struct graphicsPipeline pipe[modelQuantity]) {
    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = 0,
        .pInheritanceInfo = NULL,
        .pNext = NULL
    };

    VkClearValue clearValues[] = {
        [0].color.float32 = {
            0.0f,
            0.0f,
            0.0f,
            1.0f
        },
        [1].depthStencil = {
            .depth = 1.0f,
            .stencil = 0
        }
    };

    VkRenderPassBeginInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = vulkan->graphics.renderPass,
        .framebuffer = swapChainFramebuffer,
        .renderArea = {
            .offset = {
                .x = 0,
                .y = 0
            },
            .extent = swapChainExtent
        },
        .clearValueCount = sizeof(clearValues) / sizeof(VkClearValue),
        .pClearValues = clearValues
    };

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float)swapChainExtent.width,
        .height = (float)swapChainExtent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor = {
        .offset = { 0, 0 },
        .extent = swapChainExtent
    };

    MY_ASSERT(VK_SUCCESS == vkBeginCommandBuffer(commandBuffer, &beginInfo));
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    for (uint32_t i = 0; i < modelQuantity; i += 1) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipe[i].pipeline);
        for (uint32_t j = 0; j < pipe[i].modelQuantity; j += 1) {
            VkDescriptorSet sets[] = {
                pipe[i].model[j].graphics.object.descriptorSets[currentFrame],
                pipe[i].texture->descriptorSets[currentFrame],
                vulkan->graphics.cameraDescriptorSet[currentFrame]
            };
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipe[i].pipelineLayout, 0, 3, sets, 0, NULL);
            for (uint32_t k = 0; k < pipe[i].model[j].meshQuantity; k += 1) {
                vkCmdBindVertexBuffers(commandBuffer, 0, 1, &pipe[i].model[j].mesh[k].vertexBuffer, (VkDeviceSize[]){ 0 });
                vkCmdBindIndexBuffer(commandBuffer, pipe[i].model[j].mesh[k].indexBuffer, 0, VK_INDEX_TYPE_UINT16);

                vkCmdPushConstants(commandBuffer, pipe[i].pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(struct MeshPushConstants), &(struct MeshPushConstants) { .meshID = k });
                vkCmdDrawIndexed(commandBuffer, pipe[i].model[j].mesh[k].indicesQuantity, pipe[i].model[j].instanceCount, 0, 0, 0);
            }
        }
    }
    vkCmdEndRenderPass(commandBuffer);
    MY_ASSERT(VK_SUCCESS == vkEndCommandBuffer(commandBuffer));
}

static void update(struct graphicsPipeline pipe, uint32_t currentFrame) {
    for (uint32_t i = 0; i < pipe.modelQuantity; i += 1) {
        struct Entity *model = &pipe.model[i];
        for (uint32_t j = 0; j < model->instanceCount; j += 1) {
            double time = clock() / (double)CLOCKS_PER_SEC;
            glm_mat4_identity(model->instanceBuffer[j].modelMatrix);

            glm_translate(model->instanceBuffer[j].modelMatrix, model->instance[j].pos);
            glm_rotate(model->instanceBuffer[j].modelMatrix, glm_rad(90) + model->instance[j].fixedRotation[0] + time * model->instance[j].rotation[0], (vec3) { 1, 0, 0 });
            glm_rotate(model->instanceBuffer[j].modelMatrix,               model->instance[j].fixedRotation[1] + time * model->instance[j].rotation[1], (vec3) { 0, 1, 0 });
            glm_rotate(model->instanceBuffer[j].modelMatrix,               model->instance[j].fixedRotation[2] + time * model->instance[j].rotation[2], (vec3) { 0, 0, 1 });
            glm_scale(model->instanceBuffer[j].modelMatrix, model->instance[j].scale);
            model->instanceBuffer[j].textureIndex = model->texturePointer + model->instance[j].textureIndex + model->instance[j].textureInc;
            model->instanceBuffer[j].shadow = model->instance[j].shadow;
        }
        memcpy((uint8_t *)model->graphics.uniformModel.buffersMapped[currentFrame], model->instanceBuffer, sizeof(struct instanceBuffer) * model->instanceCount);
    }
}

static VkResult localDrawFrame(struct VulkanTools *vulkan, uint16_t modelQuantity, struct graphicsPipeline model[modelQuantity]) {
    VkResult result = VK_TRUE;

    uint32_t imageIndex = 0;
    static uint32_t currentFrame = 0;

    VkSemaphore waitSemaphores[] = {
        vulkan->graphics.imageAvailableSemaphore[currentFrame]
    };

    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    VkSemaphore signalSemaphores[] = {
        vulkan->graphics.renderFinishedSemaphore[currentFrame]
    };

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = sizeof(waitSemaphores) / sizeof(VkSemaphore),
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &vulkan->graphics.commandBuffer[currentFrame],
        .signalSemaphoreCount = sizeof(signalSemaphores) / sizeof(VkSemaphore),
        .pSignalSemaphores = signalSemaphores
    };

    VkSwapchainKHR swapChains[] = {
        vulkan->graphics.swapChain.this
    };

    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = sizeof(waitSemaphores) / sizeof(VkSemaphore),
        .pWaitSemaphores = signalSemaphores,
        .swapchainCount = sizeof(swapChains) / sizeof(VkSwapchainKHR),
        .pSwapchains = swapChains,
        .pImageIndices = &imageIndex,
        .pResults = NULL // optional
    };

    vkWaitForFences(vulkan->graphics.device, 1, &vulkan->graphics.inFlightFence[currentFrame], VK_TRUE, UINT64_MAX);

    result = vkAcquireNextImageKHR(vulkan->graphics.device, vulkan->graphics.swapChain.this, UINT64_MAX, vulkan->graphics.imageAvailableSemaphore[currentFrame], VK_NULL_HANDLE, &imageIndex);
    if (VK_SUCCESS == result) {
        updateUniformBuffer(vulkan->graphics.uniformBuffersMapped[currentFrame], vulkan->graphics.swapChain.extent, vulkan->camera.cameraPos, vulkan->camera.center);
        for (uint32_t i = 0; i < modelQuantity; i += 1) {
            update(model[i], currentFrame);
        }

        vkResetFences(vulkan->graphics.device, 1, &vulkan->graphics.inFlightFence[currentFrame]);

        vkResetCommandBuffer(vulkan->graphics.commandBuffer[currentFrame], 0);
        recordCommandBuffer(vulkan->graphics.commandBuffer[currentFrame], vulkan->graphics.swapChainFramebuffers[imageIndex], vulkan->graphics.swapChain.extent, vulkan, currentFrame, modelQuantity, model);

        MY_ASSERT(VK_SUCCESS == vkQueueSubmit(vulkan->graphics.graphicsQueue, 1, &submitInfo, vulkan->graphics.inFlightFence[currentFrame]));
        result = vkQueuePresentKHR(vulkan->graphics.presentQueue, &presentInfo);

        if (VK_SUCCESS == result) {
            currentFrame += 1;
            currentFrame %= MAX_FRAMES_IN_FLIGHT;
        }
    }

    return result;
}

void drawFrame(struct VulkanTools *vulkan, uint16_t modelQuantity, struct graphicsPipeline model[modelQuantity]) {
    updateDeltaTime(&vulkan->deltaTime);

    switch (localDrawFrame(vulkan, modelQuantity, model)) {
        case VK_SUCCESS:
            break;
        case VK_SUBOPTIMAL_KHR:
        case VK_ERROR_OUT_OF_DATE_KHR:
            *vulkan->framebufferResized = true;
            break;
        default:
            fprintf(stderr, "Oh no");
            glfwSetWindowShouldClose(vulkan->window, GLFW_TRUE);
            break;
    }

    if (*vulkan->framebufferResized) {
        *vulkan->framebufferResized = false;

        recreateSwapChain(vulkan);
    }
}
