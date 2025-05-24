#include <cglm.h>
#include <string.h>

#include "engineCore.h"
#include "state.h"

#include "asset.h"

#include "renderPassObj.h"
#include "instanceBuffer.h"
#include "entity.h"

#include "player.h"
#include "button.h"

void pause(struct EngineCore *engine, enum state *state) {
    struct ResourceManager *graphicPipelineData = findResource(&engine->resource, "graphicPipelines");
    struct ResourceManager *entityData = findResource(&engine->resource, "Entity");
    struct ResourceManager *modelData = findResource(&engine->resource, "modelData");
    struct ResourceManager *screenData = findResource(&engine->resource, "ScreenData");
        
    struct graphicsPipeline *pipe[] = {
        findResource(graphicPipelineData, "Text"),
        findResource(graphicPipelineData, "Flat"),
        findResource(graphicPipelineData, "Animated Model")
    };

    struct Entity *entity[] = {
        findResource(entityData, "Pause"),
        findResource(entityData, "Flat"),
        findResource(entityData, "Resume"),
        findResource(entityData, "Main Menu"),
        findResource(entityData, "Exit"),
    };
    size_t qEntity = sizeof(entity) / sizeof(struct Entity *);

    struct ResourceManager *renderPassCoreData = findResource(&engine->resource, "RenderPassCoreData");
    struct renderPassCore *renderPassArr[] = { 
        findResource(renderPassCoreData, "Clean"),
        findResource(renderPassCoreData, "Stay")
    };
    size_t qRenderPassArr = sizeof(renderPassArr) / sizeof(struct renderPassCore *);

    struct renderPassObj *renderPass[] = {
        findResource(screenData, "Left Screen"),
        findResource(screenData, "Right Screen"),
        findResource(screenData, "Background Left 1"),
        findResource(screenData, "Background Right 1"),
        findResource(screenData, "Left Figure"),
        findResource(screenData, "Right Figure"),
        findResource(screenData, "Left Text"),
        findResource(screenData, "Right Text"),
        findResource(screenData, "Background Left 2"),
        findResource(screenData, "Background Left 3"),
        findResource(screenData, "Background Left 4"),
        findResource(screenData, "Background Left 5"),
        findResource(screenData, "Background Right 2"),
        findResource(screenData, "Background Right 3"),
        findResource(screenData, "Background Right 4"),
        findResource(screenData, "Background Right 5"),
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0, 0.0, 1.0, 1.0 },
            .renderPass = renderPassArr[1],
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[0],
                    .entity = (struct Entity* []) {
                        entity[0],
                        entity[2],
                        entity[3],
                        entity[4]
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
    struct instance *buttonText[] = {
        entity[2]->instance,
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

    renderPass[16]->camera = (struct camera) {
        .pos = { 0.0, 0.0, 0.0 },
        .direction = { 0.0, 0.0, 1.0 }
    };

    struct Button button = {
        .joystick = *(char *)findResource(&engine->resource, "playerNumb"),
        .qButton = 3,
        .entity = entity[1],
        .model = findResource(modelData, "flat"),
        .camera = renderPass[16]->cameraBufferMapped[0],
        .newState = (int []) {
            GAME,
            MAIN_MENU,
            EXIT
        },
        .chosen = 0,
    };

    playSound(&engine->soundManager, 3, true, 1.0f);

    while (PAUSE == *state && !shouldWindowClose(engine->window)) {
        glfwPollEvents();

        updateInstances(entity, qEntity, engine->deltaTime.deltaTime);

        drawFrame(engine, qRenderPass, renderPass, qRenderPassArr, renderPassArr);
        shadowButton(engine->graphics, engine->window, &button);
        if (button.isClicked) {
            *state = button.newState[button.chosen];
        }
    }

    switch (*state) {
        case MAIN_MENU:
        case EXIT:
            vkDeviceWaitIdle(engine->graphics.device);
            cleanupResource(&engine->resource, "ScreenData");
            cleanupResource(&engine->resource, "actualPlayerData");
            break;
        default:
    }

    vkDeviceWaitIdle(engine->graphics.device);

    cleanupResource(&engine->resource, "playerNumb");
    destroyRenderPassObjArr(1, renderPass + 16);
}
