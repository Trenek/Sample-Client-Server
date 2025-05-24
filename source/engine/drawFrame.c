#include <string.h>

#include "engineCore.h"

#include "renderPassCore.h"
#include "renderPassObj.h"
#include "graphicsPipelineObj.h"
#include "entity.h"
#include "actualModel.h"
#include "pushConstantsBuffer.h"

#include "MY_ASSERT.h"

static void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, VkExtent2D swapChainExtent, uint32_t currentFrame, uint16_t qRenderPass, struct renderPassObj *renderPass[qRenderPass]) {
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

    VkRect2D renderArena[qRenderPass];
    VkRenderPassBeginInfo renderPassInfo[qRenderPass];
    VkViewport viewport[qRenderPass];

    for (size_t i = 0; i < qRenderPass; i += 1) {
        renderArena[i] = (VkRect2D) {
            .offset = {
                .x = (int32_t)(renderPass[i]->coordinates[0] * swapChainExtent.width),
                .y = (int32_t)(renderPass[i]->coordinates[1] * swapChainExtent.height)
            },
            .extent = {
                .width = (uint32_t)(renderPass[i]->coordinates[2] * swapChainExtent.width),
                .height = (uint32_t)(renderPass[i]->coordinates[3] * swapChainExtent.height)
            }
        };
        renderPassInfo[i] = (VkRenderPassBeginInfo){
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = renderPass[i]->renderPass->renderPass,
            .framebuffer = renderPass[i]->renderPass->swapChainFramebuffers[imageIndex],
            .renderArea = renderArena[i],
            .clearValueCount = sizeof(clearValues) / sizeof(VkClearValue),
            .pClearValues = clearValues
        };
        viewport[i] = (VkViewport){
            .x = renderArena[i].offset.x,
            .y = renderArena[i].offset.y,
            .width = renderArena[i].extent.width,
            .height = renderArena[i].extent.height,
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };
    }

    MY_ASSERT(VK_SUCCESS == vkBeginCommandBuffer(commandBuffer, &beginInfo));

    for (size_t i = 0; i < qRenderPass; i += 1) {
        if (renderPassInfo[i].renderArea.extent.width > 0 && renderPassInfo[i].renderArea.extent.height > 0) {
            vkCmdBeginRenderPass(commandBuffer, &renderPassInfo[i], VK_SUBPASS_CONTENTS_INLINE);
            vkCmdSetViewport(commandBuffer, 0, 1, &viewport[i]);
            vkCmdSetScissor(commandBuffer, 0, 1, &renderArena[i]);

            for (uint32_t j = 0; j < renderPass[i]->qData; j += 1) {
                vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderPass[i]->data[j].pipe->pipeline[renderPass[i]->data[j].pipeNum].pipeline);
                for (uint32_t k = 0; k < renderPass[i]->data[j].qEntity; k += 1) {
                    VkDescriptorSet sets[] = {
                        renderPass[i]->data[j].entity[k]->object.descriptorSets[currentFrame],
                        renderPass[i]->data[j].pipe->texture->descriptorSets[currentFrame],
                        renderPass[i]->cameraDescriptorSet[currentFrame],
                    };
                    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderPass[i]->data[j].pipe->pipelineLayout, 0, 3, sets, 0, NULL);
                    for (uint32_t l = 0; l < renderPass[i]->data[j].entity[k]->meshQuantity; l += 1) {
                        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &renderPass[i]->data[j].entity[k]->mesh[l].vertexBuffer, (VkDeviceSize[]){ 0 });
                        vkCmdBindIndexBuffer(commandBuffer, renderPass[i]->data[j].entity[k]->mesh[l].indexBuffer, 0, VK_INDEX_TYPE_UINT16);

                        vkCmdPushConstants(commandBuffer, renderPass[i]->data[j].pipe->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(struct MeshPushConstants), &(struct MeshPushConstants) { .meshID = l });
                        vkCmdDrawIndexed(commandBuffer, renderPass[i]->data[j].entity[k]->mesh[l].indicesQuantity, renderPass[i]->data[j].entity[k]->instanceCount, 0, 0, 0);
                    }
                }
            }
            vkCmdEndRenderPass(commandBuffer);
        }
    }
    MY_ASSERT(VK_SUCCESS == vkEndCommandBuffer(commandBuffer));
}

static void updateModelBuffer(size_t currentFrame, struct Entity *model) {
    for (uint32_t k = 0; k < model->qBuff; k += 1) {
        if (model->buffer[k]) {
            memcpy((*model->mapp[k])[currentFrame], model->buffer[k], model->range[k]);
        }
    }
}

static void updateBuffers(size_t currentFrame, size_t qRenderPass, struct renderPassObj *renderPass[qRenderPass], VkExtent2D swapChainExtent) {
    for (uint32_t i = 0; i < qRenderPass; i += 1) {
        renderPass[i]->updateCameraBuffer(renderPass[i]->cameraBufferMapped[currentFrame], (VkExtent2D) { 
            .width = renderPass[i]->coordinates[2] * swapChainExtent.width,
            .height = renderPass[i]->coordinates[3] * swapChainExtent.height,
        }, renderPass[i]->camera.pos, renderPass[i]->camera.direction);
        for (uint32_t j = 0; j < renderPass[i]->qData; j += 1) {
            for (uint32_t k = 0; k < renderPass[i]->data[j].qEntity; k += 1) {
                updateModelBuffer(currentFrame, renderPass[i]->data[j].entity[k]);
            }
        }
    }
}

static VkResult localDrawFrame(struct EngineCore *vulkan, uint16_t qRenderPass, struct renderPassObj *renderPass[qRenderPass]) {
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
        updateBuffers(currentFrame, qRenderPass, renderPass, vulkan->graphics.swapChain.extent);

        vkResetFences(vulkan->graphics.device, 1, &vulkan->graphics.inFlightFence[currentFrame]);

        vkResetCommandBuffer(vulkan->graphics.commandBuffer[currentFrame], 0);
        recordCommandBuffer(vulkan->graphics.commandBuffer[currentFrame], imageIndex, vulkan->graphics.swapChain.extent, currentFrame, qRenderPass, renderPass);

        MY_ASSERT(VK_SUCCESS == vkQueueSubmit(vulkan->graphics.graphicsQueue, 1, &submitInfo, vulkan->graphics.inFlightFence[currentFrame]));
        result = vkQueuePresentKHR(vulkan->graphics.presentQueue, &presentInfo);

        if (VK_SUCCESS == result) {
            currentFrame += 1;
            currentFrame %= MAX_FRAMES_IN_FLIGHT;
        }
    }

    return result;
}

void drawFrame(struct EngineCore *vulkan, uint16_t qRenderPass, struct renderPassObj **renderPass, uint16_t qRenderPassCore, struct renderPassCore **renderPassCore) {
    updateDeltaTime(&vulkan->deltaTime);

    switch (localDrawFrame(vulkan, qRenderPass, renderPass)) {
        case VK_SUCCESS:
            break;
        case VK_SUBOPTIMAL_KHR:
        case VK_ERROR_OUT_OF_DATE_KHR:
            vulkan->window.data->framebufferResized = true;
            break;
        default:
            fprintf(stderr, "Oh no");
            glfwSetWindowShouldClose(vulkan->window.window, GLFW_TRUE);
            break;
    }

    if (vulkan->window.data->framebufferResized) {
        vulkan->window.data->framebufferResized = false;

        recreateSwapChain(vulkan, qRenderPassCore, renderPassCore);
    }
}
