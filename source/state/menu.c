#include <cglm.h>
#include <string.h>

#include "camera.h"

#include "engineCore.h"
#include "state.h"

#include "asset.h"
#include "entity.h"
#include "entityBuilder.h"
#include "modelBuilder.h"
#include "stringBuilder.h"
#include "instanceBuffer.h"

#include "graphicsPipelineObj.h"
#include "renderPass.h"

#include "Vertex.h"

#include "player.h"

#include "button.h"

void menu(struct EngineCore *engine, enum state *state) {
    const char *texturePaths[] = {
        "textures/button.png",
    };
    size_t texturesQuantity = sizeof(texturePaths) / sizeof(const char *);

    struct Textures texture = loadTextures(&engine->graphics, texturesQuantity, texturePaths);
    struct Textures cubeMap = loadCubeMaps(&engine->graphics, (const char *[]) {
        "textures/CubeMaps/xpos.png",
        "textures/CubeMaps/xneg.png",
        "textures/CubeMaps/ypos.png",
        "textures/CubeMaps/yneg.png",
        "textures/CubeMaps/zpos.png",
        "textures/CubeMaps/zneg.png",
    });

    const char *modelPath[] = {
        "fonts/c.ttf",
        "models/my_model2d.obj",
        "models/my_skybox.obj"
    };
    size_t modelQuantity = sizeof(modelPath) / sizeof(const char *);

    struct actualModel actualModel[modelQuantity] = {}; {
        loadModels(modelQuantity, actualModel, modelPath, &engine->graphics);
    }

    VkDescriptorSetLayout objectLayout = createObjectDescriptorSetLayout(engine->graphics.device, 2, (VkDescriptorSetLayoutBinding []) {
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = NULL
        },
        {
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = NULL
        }
    });

    VkDescriptorSetLayout cameraLayout = createCameraDescriptorSetLayout(engine->graphics.device);

    struct graphicsPipeline pipe[] = { 
        /* Text */ createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
            .vertexShader = "shaders/text2dV.spv",
            .fragmentShader = "shaders/textF.spv",
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
            .texture = &texture.descriptor,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

            .objectLayout = objectLayout,

            Vert(FontVertex),
            .operation = VK_COMPARE_OP_LESS,
            .cullFlags = VK_CULL_MODE_BACK_BIT,

            .cameraLayout = cameraLayout
        }, &engine->graphics),
        /* Flat */ createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
            .vertexShader = "shaders/vert2d.spv",
            .fragmentShader = "shaders/frag2d.spv",
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
            .texture = &texture.descriptor,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

            .objectLayout = objectLayout,

            Vert(AnimVertex),
            .operation = VK_COMPARE_OP_LESS,
            .cullFlags = VK_CULL_MODE_NONE,

            .cameraLayout = cameraLayout
        }, &engine->graphics),
        /* Skybox */ createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
            .vertexShader = "shaders/skyboxV.spv",
            .fragmentShader = "shaders/skyboxF.spv",
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
            .texture = &cubeMap.descriptor,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

            .objectLayout = objectLayout,

            Vert(AnimVertex),
            .operation = VK_COMPARE_OP_LESS_OR_EQUAL,
            .cullFlags = VK_CULL_MODE_BACK_BIT,

            .cameraLayout = cameraLayout
        }, &engine->graphics),
    };
    size_t qPipe = sizeof(pipe) / sizeof(struct graphicsPipeline);

    struct Entity *entity[] = {
        /*text*/ createString((struct StringBuilder) {
            .instanceCount = 1,
            .string = "Main Menu",
            .modelData = &actualModel[0],
            .objectLayout = objectLayout,

            INS(instance, instanceBuffer),
            .center = 0
        }, &engine->graphics),
        /*flat*/ createModel((struct ModelBuilder) {
            .instanceCount = 2,
            .modelData = &actualModel[1],
            .objectLayout = objectLayout,

            INS(instance, instanceBuffer),
        }, &engine->graphics),
        /*background*/ createModel((struct ModelBuilder) {
            .instanceCount = 1,
            .modelData = &actualModel[2],
            .objectLayout = objectLayout,

            INS(instance, instanceBuffer),
        }, &engine->graphics),
        /*text2*/ createString((struct StringBuilder) {
            .instanceCount = 1,
            .string = "Play",
            .modelData = &actualModel[0],
            .objectLayout = objectLayout,

            INS(instance, instanceBuffer),
            .center = 0
        }, &engine->graphics),
        /*text3*/ createString((struct StringBuilder) {
            .instanceCount = 1,
            .string = "Exit",
            .modelData = &actualModel[0],
            .objectLayout = objectLayout,

            INS(instance, instanceBuffer),
            .center = 0
        }, &engine->graphics),
    };
    size_t qEntity = sizeof(entity) / sizeof(struct Entity *);

    struct renderPassObj renderPass[] = {
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0, 0.0, 1.0, 1.0 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = &pipe[0],
                    .entity = (struct Entity* []) {
                        entity[0],
                        entity[3],
                        entity[4],
                    },
                    .qEntity = 3
                },
                {
                    .pipe = &pipe[1],
                    .entity = &entity[1],
                    .qEntity = 1
                },
                {
                    .pipe = &pipe[2],
                    .entity = &entity[2],
                    .qEntity = 1
                }
            },
            .qData = 3,
            .updateCameraBuffer = updateFirstPersonCameraBuffer
        }, &engine->graphics),
    };
    size_t qRenderPass = sizeof(renderPass) / sizeof(struct renderPassObj);

    struct instance *text = entity[0]->instance;
    struct instance *flat = entity[1]->instance;
    struct instance *background = entity[2]->instance;
    struct instance *buttonText[] = {
        entity[3]->instance,
        entity[4]->instance
    };

    text[0] = (struct instance){
        .pos = { 0.0f, 0.3f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 4 * 10e-3, 4 * 10e-3, 4 * 10e-3 },
        .textureIndex = 0,
        .shadow = false
    };

    buttonText[0][0] = (struct instance){
        .pos = { 0.0f, -0.014f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 3 * 10e-3, 3 * 10e-3, 3 * 10e-3 },
        .textureIndex = 0,
        .shadow = false
    };

    buttonText[1][0] = (struct instance){
        .pos = { 0.0f, -0.11f -0.014f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 3 * 10e-3, 3 * 10e-3, 3 * 10e-3 },
        .textureIndex = 0,
        .shadow = false
    };

    flat[0] = (struct instance){
        .pos = { 0.0f, 0.0f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 0.1f, 0.05f, 0.1f },
        .textureIndex = 0,
        .shadow = false
    };

    flat[1] = (struct instance){
        .pos = { 0.0f, -0.11f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 0.1f, 0.05f, 0.1f },
        .textureIndex = 0,
        .shadow = false
    };

    background[0] = (struct instance){
        .pos = { 0.0f, 0.0f, 0.0f }, 
        .rotation = { 0.0f, glm_rad(0.3), 0.0f },
        .fixedRotation = { glm_rad(90), 0.0f, 0.0f },
        .scale = { 1.0f, 1.0f, 1.0f },
        .textureIndex = 0,
        .shadow = false
    };

    renderPass[0].camera = (struct camera) {
        .pos = { 0.0, 0.0, 0.0 },
        .direction = { 0.0, 1.0, 0.0 }
    };

    struct Button button = {
        .entity = entity[1],
        .model = &actualModel[1],
        .camera = renderPass[0].cameraBufferMapped[0],
        .newState = (int []) {
            GAME,
            EXIT
        },
        .chosen = -1,
    };
    while (MAIN_MENU == *state && !shouldWindowClose(engine->window)) {
        glfwPollEvents();

        updateInstances(entity, qEntity, engine->deltaTime.deltaTime);

        drawFrame(engine, qRenderPass, renderPass);
        shadowButton(engine->graphics, engine->window, &button);
        if ((KEY_PRESS | KEY_CHANGE) == getMouseState(&engine->window, GLFW_MOUSE_BUTTON_LEFT)) {
            if (button.chosen >= 0) {
                *state = button.newState[button.chosen];
            }
        }
    }

    for (size_t i = 0; i < qPipe; i += 1) {
        destroyObjGraphicsPipeline(engine->graphics.device, pipe[i]);
    }

    destroyEntityArray(qEntity, entity, &engine->graphics);
    destroyRenderPassObj(qRenderPass, renderPass, &engine->graphics);

    vkDestroyDescriptorSetLayout(engine->graphics.device, objectLayout, NULL);
    vkDestroyDescriptorSetLayout(engine->graphics.device, cameraLayout, NULL);

    destroyActualModels(engine->graphics.device, modelQuantity, actualModel);
    unloadTextures(engine->graphics.device, texture);
    unloadTextures(engine->graphics.device, cubeMap);
}
