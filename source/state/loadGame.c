#include <cglm.h>

#include "GLFW/glfw3.h"
#include "engineCore.h"
#include "state.h"

#include "asset.h"
#include "entity.h"
#include "instanceBuffer.h"

#include "graphicsPipelineObj.h"
#include "renderPass.h"

#include "player.h"

static void createScreens(struct EngineCore *engine) {
    struct ResourceManager *entityData = findResource(&engine->resource, "Entity");
    struct ResourceManager *graphicPipelineData = findResource(&engine->resource, "graphicPipelines");

    struct graphicsPipeline *pipe[] = { 
        findResource(graphicPipelineData, "Floor"),
        findResource(graphicPipelineData, "Animated Model"),
        findResource(graphicPipelineData, "Text"),
        findResource(graphicPipelineData, "Skybox"),
        findResource(graphicPipelineData, "FlatColor"),
    };

    struct Entity *entity[] = {
        findResource(entityData, "Floor"),
        findResource(entityData, "Player 1"),
        findResource(entityData, "Player 2"),
        findResource(entityData, "Fight!"),
        findResource(entityData, "Background"),
        findResource(entityData, "Player 1 Text"),
        findResource(entityData, "Player 2 Text"),
        findResource(entityData, "Blue Back"),
        findResource(entityData, "Health"),
        findResource(entityData, "Health Background"),
        findResource(entityData, "Rest"),
        findResource(entityData, "Rest Background"),
        findResource(entityData, "Cube")
    };
    
    struct ResourceManager *screenData = calloc(1, sizeof(struct ResourceManager));

    addResource(screenData, "Left Screen", createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0, 0.0, 0.5, 1.0 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[0],
                    .entity = (struct Entity *[]) {
                        entity[0],
                        entity[12]
                    },
                    .qEntity = 2
                },
                {
                    .pipe = pipe[1],
                    .entity = entity + 1,
                    .qEntity = 2
                },
                {
                    .pipe = pipe[3],
                    .entity = entity + 4,
                    .qEntity = 1
                },
            },
            .qData = 3,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, "Right Screen",
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.5, 0.0, 0.5, 1.0 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[0],
                    .entity = (struct Entity *[]) {
                        entity[0],
                        entity[12]
                    },
                    .qEntity = 2
                },
                {
                    .pipe = pipe[1],
                    .entity = entity + 1,
                    .qEntity = 2
                },
                {
                    .pipe = pipe[3],
                    .entity = entity + 4,
                    .qEntity = 1
                },
            },
            .qData = 3,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, "Background Left 1",
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0 / 8, 0.0 / 8, 2.0 / 8, 1.0 / 8 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[4],
                    .entity = entity + 7,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, "Background Right 1",
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 6.0 / 8, 0.0 / 8, 2.0 / 8, 1.0 / 8 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[4],
                    .entity = entity + 7,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, "Middle Text",
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0, 0.0, 1.0, 1.0 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[2],
                    .entity = (struct Entity* []) {
                        entity[3],
                    },
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateFirstPersonCameraBuffer
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, "Left Figure",
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0 / 8, 0.0 / 8, 1.0 / 8, 1.0 / 8 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[1],
                    .entity = entity + 1,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, "Right Figure",
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 7.0 / 8, 0.0 / 8, 1.0 / 8, 1.0 / 8 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[1],
                    .entity = entity + 2,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, "Left Text",
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0 / 8, 0.0 / 8, 1.0 / 8, 1.0 / 8 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[2],
                    .entity = (struct Entity* []) {
                        entity[5],
                    },
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateFirstPersonCameraBuffer
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, "Right Text",
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 7.0 / 8, 0.0 / 8, 1.0 / 8, 1.0 / 8 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[2],
                    .entity = (struct Entity* []) {
                        entity[6]
                    },
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateFirstPersonCameraBuffer
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, "Background Left 2",
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 1.0 / 8, 2.0 / 80, 1.0 / 8, 3.0 / 80 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[4],
                    .entity = entity + 9,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, "Background Left 3",
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 1.0 / 8, 6.0 / 90, 1.0 / 8, 3.0 / 80 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[4],
                    .entity = entity + 11,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, "Background Left 4",
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 1.0 / 8, 2.0 / 80, 1.0 / 8, 3.0 / 80 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[4],
                    .entity = entity + 8,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, "Background Left 5",
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 1.0 / 8, 6.0 / 90, 1.0 / 8, 3.0 / 80 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[4],
                    .entity = entity + 10,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, "Background Right 2",
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 6.0 / 8, 2.0 / 80, 1.0 / 8, 3.0 / 80 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[4],
                    .entity = entity + 9,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, "Background Right 3",
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 6.0 / 8, 6.0 / 90, 1.0 / 8, 3.0 / 80 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[4],
                    .entity = entity + 11,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, "Background Right 4",
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 6.0 / 8, 2.0 / 80, 1.0 / 8, 3.0 / 80 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[4],
                    .entity = entity + 8,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, "Background Right 5",
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 6.0 / 8, 6.0 / 90, 1.0 / 8, 3.0 / 80 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[4],
                    .entity = entity + 10,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        destroyRenderPassObj
    );

    struct instance *floor = entity[0]->instance;
    struct playerInstance *player = entity[1]->instance;
    struct playerInstance *enemy = entity[2]->instance;
    struct instance *text = entity[3]->instance;
    struct instance *player1Text = entity[5]->instance;
    struct instance *player2Text = entity[6]->instance;
    struct instance *background = entity[4]->instance;
    struct instance *flat = entity[7]->instance;
    struct instance *red = entity[8]->instance;
    struct instance *redBg = entity[9]->instance;
    struct instance *blue = entity[10]->instance;
    struct instance *blueBg = entity[11]->instance;
    struct instance *ring = entity[12]->instance;

    floor[0] = (struct instance){
        .pos = { 0.5f, 0.5f, -5.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { glm_rad(90), 0.0f, 0.0f },
        .scale = { 100.0f, 10.0f, 100.0f },
        .textureIndex = 5
    };

    player[0] = (struct playerInstance){
        .pos = { 2.5f, 0.0f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { glm_rad(90), glm_rad(-90), 0.0f },
        .scale = { 1.5 * 10e-2, 1.5 * 10e-2, 1.5 * 10e-2 },
        .skinColor = { (float)0xff / 0x100, (float)0xad / 0x100, (float)0x5c / 0x100 },
        .dressColor = { 1, 0, 0 }
    };

    enemy[0] = (struct playerInstance){
        .pos = { -2.5f, 0.0f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { glm_rad(90), glm_rad(90), 0.0f },
        .scale = { 1.5 * 10e-2, 1.5 * 10e-2, 1.5 * 10e-2 },
        .skinColor = { (float)0x5c / 0x100, (float)0x2e / 0x100, (float)0x00 / 0x100 },
        .dressColor = { 0, 1, 0 }
    };

    text[0] = (struct instance){
        .pos = { 0.0f, 0.3f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 4 * 10e-3, 4 * 10e-3, 4 * 10e-3 },
        .textureIndex = 0,
        .shadow = false
    };

    player1Text[0] = (struct instance){
        .pos = { 0.0f, -0.35f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 2 * 10e-2, 2 * 10e-2, 2 * 10e-2 },
        .textureIndex = 0,
        .shadow = false
    };

    player2Text[0] = (struct instance){
        .pos = { 0.0f, -0.35f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 2 * 10e-2, 2 * 10e-2, 2 * 10e-2 },
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

    flat[0] = (struct instance){
        .pos = { 0.0f, 0.0f, 0.0f }, 
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 10.0f, 10.0f, 10.0f },
        .textureIndex = 0,
        .shadow = false
    };

    red[0] = (struct instance){
        .pos = { 0.0f, 0.0f, 0.0f }, 
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 10.0f, 10.0f, 10.0f },
        .textureIndex = 1,
        .shadow = false
    };

    redBg[0] = (struct instance){
        .pos = { 0.0f, 0.0f, 0.0f }, 
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 10.0f, 10.0f, 10.0f },
        .textureIndex = 2,
        .shadow = false
    };

    blue[0] = (struct instance){
        .pos = { 0.0f, 0.0f, 0.0f }, 
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 10.0f, 10.0f, 10.0f },
        .textureIndex = 3,
        .shadow = false
    };

    blueBg[0] = (struct instance){
        .pos = { 0.0f, 0.0f, 0.0f }, 
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 10.0f, 10.0f, 10.0f },
        .textureIndex = 4,
        .shadow = false
    };

    ring[0] = (struct instance) {
        .pos = { -11.0f, 0.0f, 0.5f }, 
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 1.0f, 22.0f, 3.0f },
        .textureIndex = 0,
        .shadow = false
    };

    ring[1] = (struct instance) {
        .pos = { 11.0f, 0.0f, 0.5f }, 
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 1.0f, 22.0f, 3.0f },
        .textureIndex = 0,
        .shadow = false
    };

    ring[2] = (struct instance) {
        .pos = { 0.0f, 11.0f, 0.5f }, 
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, glm_rad(90) },
        .scale = { 1.0f, 22.0f, 3.0f },
        .textureIndex = 0,
        .shadow = false
    };

    ring[3] = (struct instance) {
        .pos = { 0.0f, -11.0f, 0.5f }, 
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, glm_rad(90) },
        .scale = { 1.0f, 22.0f, 3.0f },
        .textureIndex = 0,
        .shadow = false
    };

    addResource(&engine->resource, "ScreenData", screenData, cleanupResources);
}

void createPlayerStructs(struct EngineCore *engine) {
    struct ResourceManager *modelData = findResource(&engine->resource, "modelData");
    struct ResourceManager *entityData = findResource(&engine->resource, "Entity");
    struct ResourceManager *screenData = findResource(&engine->resource, "ScreenData");

    struct renderPassObj *renderPass[] = {
        findResource(screenData, "Left Screen"),
        findResource(screenData, "Right Screen"),
        findResource(screenData, "Background Left 1"),
        findResource(screenData, "Background Right 1"),
        findResource(screenData, "Middle Text"),
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
    };

    addResource(&engine->resource, "actualPlayerData", malloc(sizeof(struct player[2])), free);
    struct player *playerData = findResource(&engine->resource, "actualPlayerData");
    playerData[0] = (struct player){
        .model = findResource(entityData, "Player 1"),
        .actualModel = findResource(modelData, "player"),
        .enemy = &playerData[1],
        .playerKeys = {
            GLFW_KEY_W,
            GLFW_KEY_A,
            GLFW_KEY_S,
            GLFW_KEY_D,
            GLFW_KEY_F,
            GLFW_KEY_T,
            GLFW_KEY_G,
            GLFW_KEY_H,
            GLFW_KEY_LEFT_CONTROL,
            GLFW_KEY_LEFT_SHIFT,
        },
        .playerJoystick = GLFW_JOYSTICK_1,

        .maxHealth = 100,
        .currentHealth = 100,
        .healthPercentage = &renderPass[11]->coordinates[2],

        .maxRest = 10000,
        .currentRest = 10000,
        .restPercentage = &renderPass[12]->coordinates[2],

        .splitScreen = &renderPass[0]->camera,
        .face = &renderPass[5]->camera,
        .relativeFaceCameraPos = {
            -1,
            0.4,
            2,
        }
    };
    playerData[1] = (struct player){
        .model = findResource(entityData, "Player 2"),
        .actualModel = findResource(modelData, "player"),
        .enemy = &playerData[0],
        .playerKeys = {
            GLFW_KEY_UP,
            GLFW_KEY_LEFT,
            GLFW_KEY_DOWN,
            GLFW_KEY_RIGHT,
            GLFW_KEY_J,
            GLFW_KEY_I,
            GLFW_KEY_K,
            GLFW_KEY_L,
            GLFW_KEY_RIGHT_CONTROL,
            GLFW_KEY_RIGHT_SHIFT,
        },
        .playerJoystick = GLFW_JOYSTICK_2,
        .maxHealth = 100,
        .currentHealth = 100,
        .healthPercentage = &renderPass[15]->coordinates[2],

        .maxRest = 10000,
        .currentRest = 10000,
        .restPercentage = &renderPass[16]->coordinates[2],

        .splitScreen = &renderPass[1]->camera,
        .face = &renderPass[6]->camera,
        .relativeFaceCameraPos = {
            1,
            0.4,
            2,
        }
    };
}

void loadGame(struct EngineCore *engine, enum state *state) {
    createScreens(engine);
    createPlayerStructs(engine);

    *state = GAME;
}
