#include <cglm.h>
#include <string.h>

#include "camera.h"

#include "engineCore.h"
#include "state.h"

#include "asset.h"
#include "entity.h"
#include "instanceBuffer.h"

#include "graphicsPipelineObj.h"
#include "renderPassObj.h"

#include "player.h"

#include "button.h"

void menu(struct EngineCore *engine, enum state *state) {
    struct ResourceManager *graphicsPipelineData = findResource(&engine->resource, "graphicPipelines");
    struct ResourceManager *entityData = findResource(&engine->resource, "Entity");
    struct ResourceManager *modelData = findResource(&engine->resource, "modelData");

    struct graphicsPipeline *pipe[] = {
        findResource(graphicsPipelineData, "Text"),
        findResource(graphicsPipelineData, "Flat"),
        findResource(graphicsPipelineData, "Skybox"),
    };

    struct Entity *entity[] = {
        findResource(entityData, "Main Menu"),
        findResource(entityData, "Flat"),
        findResource(entityData, "Background"),
        findResource(entityData, "Play"),
        findResource(entityData, "Exit"),
    };
    size_t qEntity = sizeof(entity) / sizeof(struct Entity *);

    struct ResourceManager *renderPassCoreData = findResource(&engine->resource, "RenderPassCoreData");
    struct renderPassCore *clean = findResource(renderPassCoreData, "Clean");

    struct renderPassObj *renderPass[] = {
        createRenderPassObj((struct renderPassBuilder){
            .renderPass = clean,
            .coordinates = { 0.0, 0.0, 1.0, 1.0 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[0],
                    .entity = (struct Entity* []) {
                        entity[0],
                        entity[3],
                        entity[4],
                    },
                    .qEntity = 3
                },
                {
                    .pipe = pipe[1],
                    .entity = &entity[1],
                    .qEntity = 1
                },
                {
                    .pipe = pipe[2],
                    .entity = &entity[2],
                    .qEntity = 1
                }
            },
            .qData = 3,
            .updateCameraBuffer = updateFirstPersonCameraBuffer
        }, &engine->graphics),
    };
    size_t qRenderPass = sizeof(renderPass) / sizeof(struct renderPassObj *);

    struct Button button = {
        .joystick = GLFW_JOYSTICK_1,
        .isClicked = false,
        .qButton = 2,
        .entity = entity[1],
        .model = findResource(modelData, "flat"),
        .camera = renderPass[0]->cameraBufferMapped[0],
        .newState = (int []) {
            LOAD_GAME,
            EXIT
        },
        .chosen = 0,
    };

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

    flat[2] = (struct instance){};

    background[0] = (struct instance){
        .pos = { 0.0f, 0.0f, 0.0f }, 
        .rotation = { 0.0f, glm_rad(0.3), 0.0f },
        .fixedRotation = { glm_rad(90), 0.0f, 0.0f },
        .scale = { 1.0f, 1.0f, 1.0f },
        .textureIndex = 0,
        .shadow = false
    };

    renderPass[0]->camera = (struct camera) {
        .pos = { 0.0, 0.0, 0.0 },
        .direction = { 0.0, 1.0, 0.0 }
    };

    playSound(&engine->soundManager, 0, true, 1.0f);

    while (MAIN_MENU == *state && !shouldWindowClose(engine->window)) {
        glfwPollEvents();

        updateInstances(entity, qEntity, engine->deltaTime.deltaTime);

        drawFrame(engine, qRenderPass, renderPass, 1, &clean);
        shadowButton(engine->graphics, engine->window, &button);
        if (button.isClicked) {
            *state = button.newState[button.chosen];
        }
    }

    vkDeviceWaitIdle(engine->graphics.device);
    destroyRenderPassObjArr(qRenderPass, renderPass);
}
