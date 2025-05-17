#include <cglm.h>
#include <string.h>

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

void game(struct EngineCore *engine, enum state *state) {
    const char *texturePaths[] = {
        "textures/background.png",
        "textures/red.png",
        "textures/red_bg.png",
        "textures/blue.png",
        "textures/blue_bg.png",
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
        "models/my_model2d.obj",
        "models/my_floor.obj",
        "models/cylinder.glb",
        "fonts/c.ttf",
        "models/my_skybox.obj",
        "models/czlowiek.glb",
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

    VkDescriptorSetLayout animLayout = createObjectDescriptorSetLayout(engine->graphics.device, 3, (VkDescriptorSetLayoutBinding []) {
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
        },
        {
            .binding = 2,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = NULL
        }
    });

    VkDescriptorSetLayout cameraLayout = createCameraDescriptorSetLayout(engine->graphics.device);

    struct graphicsPipeline pipe[] = { 
        /* No Texture Model */ createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
            .vertexShader = "shaders/vert.spv",
            .fragmentShader = "shaders/frag.spv",
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

            .texture = &texture.descriptor,
            .objectLayout = objectLayout,

            Vert(AnimVertex),
            .operation = VK_COMPARE_OP_LESS,
            .cullFlags = VK_CULL_MODE_BACK_BIT,

            .cameraLayout = cameraLayout
        }, &engine->graphics),
        /* Texture Model */ createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
            .vertexShader = "shaders/playerAnimation.spv",
            .fragmentShader = "shaders/playerFrag.spv",
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
            .texture = &texture.descriptor,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

            .objectLayout = animLayout,

            Vert(AnimVertex),
            .operation = VK_COMPARE_OP_LESS,
            .cullFlags = VK_CULL_MODE_NONE,

            .cameraLayout = cameraLayout
        }, &engine->graphics),
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
    };
    size_t qPipe = sizeof(pipe) / sizeof(struct graphicsPipeline);

    struct Entity *entity[] = {
        /*floor*/ createModel((struct ModelBuilder) {
            .instanceCount = 1,
            .modelData = &actualModel[1],
            .objectLayout = objectLayout,

            INS(instance, instanceBuffer),
        }, &engine->graphics),
        /*player*/ createModel((struct ModelBuilder) {
            .instanceCount = 1,
            .modelData = &actualModel[5],
            .objectLayout = animLayout,

            INS(playerInstance, playerInstanceBuffer),
        }, &engine->graphics),
        /*enemy*/ createModel((struct ModelBuilder) {
            .instanceCount = 1,
            .modelData = &actualModel[5],
            .objectLayout = animLayout,

            INS(playerInstance, playerInstanceBuffer),
        }, &engine->graphics),
        /*text*/ createString((struct StringBuilder) {
            .instanceCount = 1,
            .string = "Fight!",
            .modelData = &actualModel[3],
            .objectLayout = objectLayout,

            INS(instance, instanceBuffer),
            .center = 0
        }, &engine->graphics),
        /*background*/ createModel((struct ModelBuilder) {
            .instanceCount = 1,
            .modelData = &actualModel[4],
            .objectLayout = objectLayout,

            INS(instance, instanceBuffer),
        }, &engine->graphics),
        /*text*/ createString((struct StringBuilder) {
            .instanceCount = 1,
            .string = "Player 1",
            .modelData = &actualModel[3],
            .objectLayout = objectLayout,

            INS(instance, instanceBuffer),
            .center = 0
        }, &engine->graphics),
        /*text*/ createString((struct StringBuilder) {
            .instanceCount = 1,
            .string = "Player 2",
            .modelData = &actualModel[3],
            .objectLayout = objectLayout,

            INS(instance, instanceBuffer),
            .center = 0
        }, &engine->graphics),
        /*flat*/ createModel((struct ModelBuilder) {
            .instanceCount = 1,
            .modelData = &actualModel[0],
            .objectLayout = objectLayout,

            INS(instance, instanceBuffer),
        }, &engine->graphics),
        /*red flat*/ createModel((struct ModelBuilder) {
            .instanceCount = 1,
            .modelData = &actualModel[0],
            .objectLayout = objectLayout,

            INS(instance, instanceBuffer),
        }, &engine->graphics),
        /*red background flat*/ createModel((struct ModelBuilder) {
            .instanceCount = 1,
            .modelData = &actualModel[0],
            .objectLayout = objectLayout,

            INS(instance, instanceBuffer),
        }, &engine->graphics),
        /*blue flat*/ createModel((struct ModelBuilder) {
            .instanceCount = 1,
            .modelData = &actualModel[0],
            .objectLayout = objectLayout,

            INS(instance, instanceBuffer),
        }, &engine->graphics),
        /*blue background flat*/ createModel((struct ModelBuilder) {
            .instanceCount = 1,
            .modelData = &actualModel[0],
            .objectLayout = objectLayout,

            INS(instance, instanceBuffer),
        }, &engine->graphics),
    };
    size_t qEntity = sizeof(entity) / sizeof(struct Entity *);

    struct renderPassObj renderPass[] = {
        /* left screen */createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0, 0.0, 0.5, 1.0 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = &pipe[0],
                    .entity = entity,
                    .qEntity = 1
                },
                {
                    .pipe = &pipe[1],
                    .entity = entity + 1,
                    .qEntity = 2
                },
                {
                    .pipe = &pipe[3],
                    .entity = entity + 4,
                    .qEntity = 1
                },
            },
            .qData = 3,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        /* right screen */createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.5, 0.0, 0.5, 1.0 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = &pipe[0],
                    .entity = entity,
                    .qEntity = 1
                },
                {
                    .pipe = &pipe[1],
                    .entity = entity + 1,
                    .qEntity = 2
                },
                {
                    .pipe = &pipe[3],
                    .entity = entity + 4,
                    .qEntity = 1
                },
            },
            .qData = 3,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        /* background left */createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0 / 8, 0.0 / 8, 2.0 / 8, 1.0 / 8 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = &pipe[4],
                    .entity = entity + 7,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        /* background right */createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 6.0 / 8, 0.0 / 8, 2.0 / 8, 1.0 / 8 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = &pipe[4],
                    .entity = entity + 7,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        /* middle text */createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0, 0.0, 1.0, 1.0 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = &pipe[2],
                    .entity = (struct Entity* []) {
                        entity[3],
                    },
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateFirstPersonCameraBuffer
        }, &engine->graphics),
        /* left figure */createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0 / 8, 0.0 / 8, 1.0 / 8, 1.0 / 8 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = &pipe[1],
                    .entity = entity + 1,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        /* right figure */createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 7.0 / 8, 0.0 / 8, 1.0 / 8, 1.0 / 8 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = &pipe[1],
                    .entity = entity + 2,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        /* left text */createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0 / 8, 0.0 / 8, 1.0 / 8, 1.0 / 8 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = &pipe[2],
                    .entity = (struct Entity* []) {
                        entity[5],
                    },
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateFirstPersonCameraBuffer
        }, &engine->graphics),
        /* right text */createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 7.0 / 8, 0.0 / 8, 1.0 / 8, 1.0 / 8 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = &pipe[2],
                    .entity = (struct Entity* []) {
                        entity[6]
                    },
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateFirstPersonCameraBuffer
        }, &engine->graphics),
        /* background left */createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 1.0 / 8, 2.0 / 80, 1.0 / 8, 3.0 / 80 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = &pipe[4],
                    .entity = entity + 9,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        /* background left */createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 1.0 / 8, 6.0 / 90, 1.0 / 8, 3.0 / 80 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = &pipe[4],
                    .entity = entity + 11,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        /* background left */createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 1.0 / 8, 2.0 / 80, 1.0 / 8, 3.0 / 80 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = &pipe[4],
                    .entity = entity + 8,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        /* background left */createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 1.0 / 8, 6.0 / 90, 1.0 / 8, 3.0 / 80 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = &pipe[4],
                    .entity = entity + 10,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        /* background left */createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 6.0 / 8, 2.0 / 80, 1.0 / 8, 3.0 / 80 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = &pipe[4],
                    .entity = entity + 9,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        /* background left */createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 6.0 / 8, 6.0 / 90, 1.0 / 8, 3.0 / 80 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = &pipe[4],
                    .entity = entity + 11,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        /* background left */createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 6.0 / 8, 2.0 / 80, 1.0 / 8, 3.0 / 80 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = &pipe[4],
                    .entity = entity + 8,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
        /* background left */createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 6.0 / 8, 6.0 / 90, 1.0 / 8, 3.0 / 80 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = &pipe[4],
                    .entity = entity + 10,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &engine->graphics),
    };

    size_t qRenderPass = sizeof(renderPass) / sizeof(struct renderPassObj);

    struct player playerData[2] = {
        {
            .model = entity[1],
            .actualModel = &actualModel[5],
            .enemy = &playerData[1],
            .playerKeys = {
                GLFW_KEY_W,
                GLFW_KEY_A,
                GLFW_KEY_S,
                GLFW_KEY_D,
                GLFW_KEY_1,
                GLFW_KEY_2,
                GLFW_KEY_3,
                GLFW_KEY_4,
                GLFW_KEY_5,
                GLFW_KEY_6,
            },
            .playerJoystick = GLFW_JOYSTICK_1,

            .maxHealth = 100,
            .currentHealth = 50,
            .maxRest = 100,
            .currentRest = 50,
            .splitScreen = &renderPass[0].camera,
            .face = &renderPass[5].camera,
            .relativeFaceCameraPos = {
                -1,
                0.4,
                2,
            }
        },
        {
            .model = entity[2],
            .actualModel = &actualModel[5],
            .enemy = &playerData[0],
            .playerKeys = {
                GLFW_KEY_UP,
                GLFW_KEY_LEFT,
                GLFW_KEY_DOWN,
                GLFW_KEY_RIGHT,
                GLFW_KEY_X,
                GLFW_KEY_C,
                GLFW_KEY_V,
                GLFW_KEY_B,
                GLFW_KEY_N,
                GLFW_KEY_M
            },
            .playerJoystick = GLFW_JOYSTICK_2,
            .maxHealth = 100,
            .currentHealth = 80,
            .maxRest = 100,
            .currentRest = 10,

            .splitScreen = &renderPass[1].camera,
            .face = &renderPass[6].camera,
            .relativeFaceCameraPos = {
                1,
                0.4,
                2,
            }
        },
    };

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

    floor[0] = (struct instance){
        .pos = { 0.0f, 0.0f, -5.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { glm_rad(90), 0.0f, 0.0f },
        .scale = { 10000.0f, 10.0f, 10000.0f },
        .textureIndex = 0
    };

    player[0] = (struct playerInstance){
        .pos = { 0.0f, 0.0f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { glm_rad(90), glm_rad(-90), 0.0f },
        .scale = { 1.5 * 10e-2, 1.5 * 10e-2, 1.5 * 10e-2 },
        .skinColor = { (float)0xff / 0x100, (float)0xad / 0x100, (float)0x5c / 0x100 },
        .dressColor = { 1, 0, 0 }
    };

    enemy[0] = (struct playerInstance){
        .pos = { -5.0f, 0.0f, 0.0f },
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

    while (GAME == *state && !shouldWindowClose(engine->window)) {
        glfwPollEvents();

        updateInstances(entity, qEntity, engine->deltaTime.deltaTime);

        movePlayer(&playerData[0], &engine->window, engine->deltaTime.deltaTime);
        renderPass[11].coordinates[2] = (float)playerData[0].currentHealth / (8.0 * playerData[0].maxHealth);
        renderPass[12].coordinates[2] = (float)playerData[0].currentRest / (8.0 * playerData[0].maxRest);

        movePlayer(&playerData[1], &engine->window, engine->deltaTime.deltaTime);
        renderPass[15].coordinates[2] = (float)playerData[1].currentHealth / (8.0 * playerData[1].maxHealth);
        renderPass[16].coordinates[2] = (float)playerData[1].currentRest / (8.0 * playerData[1].maxRest);

        drawFrame(engine, qRenderPass, renderPass);

        bool isMClicked = (KEY_PRESS | KEY_CHANGE) == getKeyState(&engine->window, GLFW_KEY_M);

        if (isMClicked) text->shadow = !text->shadow;
    }

    for (size_t i = 0; i < qPipe; i += 1) {
        destroyObjGraphicsPipeline(engine->graphics.device, pipe[i]);
    }

    destroyEntityArray(qEntity, entity, &engine->graphics);
    destroyRenderPassObj(qRenderPass, renderPass, &engine->graphics);

    vkDestroyDescriptorSetLayout(engine->graphics.device, objectLayout, NULL);
    vkDestroyDescriptorSetLayout(engine->graphics.device, animLayout, NULL);
    vkDestroyDescriptorSetLayout(engine->graphics.device, cameraLayout, NULL);

    destroyActualModels(engine->graphics.device, modelQuantity, actualModel);
    unloadTextures(engine->graphics.device, texture);
    unloadTextures(engine->graphics.device, cubeMap);
}
