#include <cglm.h>
#include <string.h>

#include "camera.h"

#include "engineCore.h"
#include "state.h"

#include "asset.h"
#include "entity.h"
#include "stringBuilder.h"
#include "instanceBuffer.h"

#include "graphicsPipelineObj.h"
#include "renderPassObj.h"

#include "player.h"

#include "button.h"

void win(struct EngineCore *engine, enum state *state) {
    struct ResourceManager *graphicPipelineData = findResource(&engine->resource, "graphicPipelines");
    struct ResourceManager *entityData = findResource(&engine->resource, "Entity");
    struct ResourceManager *modelData = findResource(&engine->resource, "modelData");
    struct descriptorSetLayout *objectLayout = findResource(findResource(&engine->resource, "objectLayout"), "object");
        
    struct graphicsPipeline *pipe[] = {
        findResource(graphicPipelineData, "Text"),
        findResource(graphicPipelineData, "Flat"),
        findResource(graphicPipelineData, "Skybox"),
        findResource(graphicPipelineData, "Animated Model")
    };

    char buffer[100]; {
        char *name = findResource(&engine->resource, "playerName");

        sprintf(buffer, "%s Won!", name);
    }

    struct playerInstance *winnerInfo = findResource(&engine->resource, "playerInfo");

    struct Entity *entity[] = {
        /*text*/ createString((struct StringBuilder) {
            .instanceCount = 1,
            .string = buffer,
            .modelData = findResource(modelData, "font"),
            .objectLayout = objectLayout->descriptorSetLayout,

            INS(instance, instanceBuffer),
            .center = 0
        }, &engine->graphics),
        findResource(entityData, "Flat"),
        findResource(entityData, "Background"),
        findResource(entityData, "Restart"),
        findResource(entityData, "Main Menu"),
        findResource(entityData, "Exit"),
        findResource(entityData, "Player 1")
    };
    size_t qEntity = sizeof(entity) / sizeof(struct Entity *);

    struct ResourceManager *renderPassCoreData = findResource(&engine->resource, "RenderPassCoreData");
    struct renderPassCore *renderPassArr[] = { 
        findResource(renderPassCoreData, "Clean"),
        findResource(renderPassCoreData, "Stay")
    };

    size_t qRenderPassArr = sizeof(renderPassArr) / sizeof(struct renderPassCore *);
    struct renderPassObj *renderPass[] = {
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0, 0.0, 1.0, 1.0 },
            .renderPass = renderPassArr[0],
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[2],
                    .entity = &entity[2],
                    .qEntity = 1
                },
                {
                    .pipe = pipe[3],
                    .entity = &entity[6],
                    .qEntity = 1
                }
            },
            .qData = 2,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0, 0.0, 1.0, 1.0 },
            .renderPass = renderPassArr[1],
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[0],
                    .entity = (struct Entity* []) {
                        entity[0],
                        entity[3],
                        entity[4],
                        entity[5]
                    },
                    .qEntity = 4
                },
                {
                    .pipe = pipe[1],
                    .entity = &entity[1],
                    .qEntity = 1
                },
            },
            .qData = 2,
            .updateCameraBuffer = updateFirstPersonCameraBuffer
        }, &engine->graphics),
    };
    size_t qRenderPass = sizeof(renderPass) / sizeof(struct renderPassObj *);

    struct instance *text = entity[0]->instance;
    struct instance *flat = entity[1]->instance;
    struct instance *background = entity[2]->instance;
    struct instance *buttonText[] = {
        entity[3]->instance,
        entity[4]->instance,
        entity[5]->instance
    };
    struct playerInstance *player = entity[6]->instance;

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

    buttonText[2][0] = (struct instance){
        .pos = { 0.0f, -0.22f -0.014f, 0.0f },
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

    flat[2] = (struct instance){
        .pos = { 0.0f, -0.22f, 0.0f },
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

    player[0] = (struct playerInstance){
        .pos = { 0.0f, 0.0f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { glm_rad(90), glm_rad(-90), 0.0f },
        .scale = { 10e-2, 10e-2, 10e-2 },
        .skinColor = { 
            winnerInfo->skinColor[0], 
            winnerInfo->skinColor[1], 
            winnerInfo->skinColor[2]
        },
        .dressColor = {
            winnerInfo->dressColor[0],
            winnerInfo->dressColor[1],
            winnerInfo->dressColor[2],
        }
    };

    renderPass[0]->camera = (struct camera) {
        .pos = { -1.0, 0.0, 1.7 },
        .direction = { 0.0, 0.0, 1.1 }
    };
    renderPass[1]->camera = (struct camera) {
        .pos = { 0.0, 0.0, 0.0 },
        .direction = { 0.0, 0.0, 1.0 }
    };

    struct Button button = {
        .joystick = *(char *)findResource(&engine->resource, "playerNumb"),
        .qButton = 3,
        .entity = entity[1],
        .model = findResource(modelData, "flat"),
        .camera = renderPass[1]->cameraBufferMapped[0],
        .newState = (int []) {
            LOAD_GAME,
            MAIN_MENU,
            EXIT
        },
        .chosen = 0,
    };

    struct player playerData[1] = {
        {
            .model = entity[6],
            .actualModel = findResource(modelData, "player"),
            .state = STANDING
        },
    };

    playSound(&engine->soundManager, 2, true, 1.0f);

    while (WIN_SCREEN == *state && !shouldWindowClose(engine->window)) {
        glfwPollEvents();

        posePlayer(playerData, engine->deltaTime.deltaTime);
        updateInstances(entity, qEntity, engine->deltaTime.deltaTime);

        drawFrame(engine, qRenderPass, renderPass, qRenderPassArr, renderPassArr);
        shadowButton(engine->graphics, engine->window, &button);
        if (button.isClicked) {
            *state = button.newState[button.chosen];
        }
    }

    vkDeviceWaitIdle(engine->graphics.device);

    cleanupResource(&engine->resource, "playerName");
    cleanupResource(&engine->resource, "playerInfo");
    cleanupResource(&engine->resource, "playerNumb");

    destroyEntity(entity[0]);
    destroyRenderPassObjArr(qRenderPass, renderPass);
}
