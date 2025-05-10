#include <asset.h>

#include <GLFW/glfw3.h>
#include <cglm.h>
#include <string.h>

#include <miniaudio.h>

#include "VulkanTools.h"
#include "state.h"

#include "entity.h"
#include "entityBuilder.h"
#include "modelBuilder.h"
#include "stringBuilder.h"
#include "instanceBuffer.h"

#include "graphicsPipelineObj.h"
#include "renderPass.h"

#include "Vertex.h"

#include "player.h"
#include "vec2.h"

void game(struct VulkanTools *vulkan, enum state *state) {
    ma_engine engine;
    ma_sound sound; {
        assert(MA_SUCCESS == ma_engine_init(NULL, &engine));
        assert(MA_SUCCESS == ma_sound_init_from_file(&engine, "music/music.mp3", 0, NULL, NULL, &sound));

        ma_sound_start(&sound);
        ma_sound_set_looping(&sound, true);
    }

    const char *texturePaths[] = {
        "textures/texture.jpg",
    };
    size_t texturesQuantity = sizeof(texturePaths) / sizeof(const char *);

    struct Textures texture = loadTextures(&vulkan->graphics, texturesQuantity, texturePaths);
    struct Textures cubeMap = loadCubeMaps(&vulkan->graphics, (const char *[]) {
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
        loadModels(modelQuantity, actualModel, modelPath, &vulkan->graphics);
    }

    VkDescriptorSetLayout objectLayout = createObjectDescriptorSetLayout(vulkan->graphics.device, 2, (VkDescriptorSetLayoutBinding []) {
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

    VkDescriptorSetLayout animLayout = createObjectDescriptorSetLayout(vulkan->graphics.device, 3, (VkDescriptorSetLayoutBinding []) {
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

    VkDescriptorSetLayout cameraLayout = createCameraDescriptorSetLayout(vulkan->graphics.device);

    struct graphicsPipeline pipe[] = { 
        /* No Texture Model */ createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
            .vertexShader = "shaders/vert.spv",
            .fragmentShader = "shaders/debugFrag.spv",
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

            .texture = &texture.descriptor,
            .objectLayout = objectLayout,

            .sizeOfVertex = sizeof(struct AnimVertex),
            .numOfAttributes = sizeof(animVertexAttributeDescriptions) / sizeof(*animVertexAttributeDescriptions),
            .attributeDescription = animVertexAttributeDescriptions,
            .operation = VK_COMPARE_OP_LESS,
            .cullFlags = VK_CULL_MODE_BACK_BIT,

            .cameraLayout = cameraLayout
        }, &vulkan->graphics),
        /* Texture Model */ createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
            .vertexShader = "shaders/playerAnimation.spv",
            .fragmentShader = "shaders/playerFrag.spv",
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
            .texture = &texture.descriptor,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

            .objectLayout = animLayout,

            .sizeOfVertex = sizeof(struct AnimVertex),
            .numOfAttributes = sizeof(animVertexAttributeDescriptions) / sizeof(*animVertexAttributeDescriptions),
            .attributeDescription = animVertexAttributeDescriptions,
            .operation = VK_COMPARE_OP_LESS,
            .cullFlags = VK_CULL_MODE_NONE,

            .cameraLayout = cameraLayout
        }, &vulkan->graphics),
        /* Text */ createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
            .vertexShader = "shaders/text2dV.spv",
            .fragmentShader = "shaders/textF.spv",
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
            .texture = &texture.descriptor,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

            .objectLayout = objectLayout,

            .sizeOfVertex = sizeof(struct FontVertex),
            .numOfAttributes = sizeof(fontVertexAttributeDescriptions) / sizeof(*fontVertexAttributeDescriptions),
            .attributeDescription = fontVertexAttributeDescriptions,
            .operation = VK_COMPARE_OP_LESS,
            .cullFlags = VK_CULL_MODE_BACK_BIT,

            .cameraLayout = cameraLayout
        }, &vulkan->graphics),
        /* Skybox */ createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
            .vertexShader = "shaders/skyboxV.spv",
            .fragmentShader = "shaders/skyboxF.spv",
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
            .texture = &cubeMap.descriptor,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

            .objectLayout = objectLayout,

            .sizeOfVertex = sizeof(struct AnimVertex),
            .numOfAttributes = sizeof(animVertexAttributeDescriptions) / sizeof(*animVertexAttributeDescriptions),
            .attributeDescription = animVertexAttributeDescriptions,
            .operation = VK_COMPARE_OP_LESS_OR_EQUAL,
            .cullFlags = VK_CULL_MODE_BACK_BIT,

            .cameraLayout = cameraLayout
        }, &vulkan->graphics),
    };
    size_t qPipe = sizeof(pipe) / sizeof(struct graphicsPipeline);

    struct Entity *entity[] = {
        /*floor*/ createModel((struct ModelBuilder) {
            .instanceCount = 1,
            .modelData = &actualModel[1],
            .objectLayout = objectLayout,

            .instanceSize = sizeof(struct instance),
            .instanceBufferSize = sizeof(struct instanceBuffer),
            .instanceUpdater = updateInstance
        }, &vulkan->graphics),
        /*player*/ createModel((struct ModelBuilder) {
            .instanceCount = 1,
            .modelData = &actualModel[5],
            .objectLayout = animLayout,

            .instanceSize = sizeof(struct playerInstance),
            .instanceBufferSize = sizeof(struct playerInstanceBuffer),
            .instanceUpdater = updatePlayerInstance,
        }, &vulkan->graphics),
        /*enemy*/ createModel((struct ModelBuilder) {
            .instanceCount = 1,
            .modelData = &actualModel[5],
            .objectLayout = animLayout,

            .instanceSize = sizeof(struct playerInstance),
            .instanceBufferSize = sizeof(struct playerInstanceBuffer),
            .instanceUpdater = updatePlayerInstance,
        }, &vulkan->graphics),
        /*text*/ createString((struct StringBuilder) {
            .instanceCount = 1,
            .string = "Hello Animation World!",
            .modelData = &actualModel[3],
            .objectLayout = objectLayout,

            .instanceSize = sizeof(struct instance),
            .instanceBufferSize = sizeof(struct instanceBuffer),
            .instanceUpdater = updateInstance,
        }, &vulkan->graphics),
        /*background*/ createModel((struct ModelBuilder) {
            .instanceCount = 1,
            .modelData = &actualModel[4],
            .objectLayout = objectLayout,

            .instanceSize = sizeof(struct instance),
            .instanceBufferSize = sizeof(struct instanceBuffer),
            .instanceUpdater = updateInstance,
        }, &vulkan->graphics),
    };
    size_t qEntity = sizeof(entity) / sizeof(struct Entity *);

    struct renderPass renderPass[] = {
        createRenderPassObj((struct renderPassBuilder){
            .p = { 0.0, 0.0, 0.5, 1.0 },
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
        }, &vulkan->graphics),
        createRenderPassObj((struct renderPassBuilder){
            .p = { 0.5, 0.0, 0.5, 1.0 },
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
        }, &vulkan->graphics),
        createRenderPassObj((struct renderPassBuilder){
            .p = { 1.0 / 8, 1.0 / 8, 1.0 / 8, 1.0 / 8 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = &pipe[1],
                    .entity = entity + 1,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &vulkan->graphics),
        createRenderPassObj((struct renderPassBuilder){
            .p = { 6.0 / 8, 1.0 / 8, 1.0 / 8, 1.0 / 8 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = &pipe[1],
                    .entity = entity + 2,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateThirdPersonCameraBuffer
        }, &vulkan->graphics),
        createRenderPassObj((struct renderPassBuilder){
            .p = { 0.0, 0.0, 1.0, 1.0 },
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = &pipe[2],
                    .entity = entity + 3,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .updateCameraBuffer = updateFirstPersonCameraBuffer
        }, &vulkan->graphics),
    };
    size_t qRenderPass = sizeof(renderPass) / sizeof(struct renderPass);

    struct instance *floor = entity[0]->instance;
    struct playerInstance *player = entity[1]->instance;
    struct playerInstance *enemy = entity[2]->instance;
    struct instance *text = entity[3]->instance;
    struct instance *background = entity[4]->instance;

    struct player playerData[2] = {
        {
            .model = entity[1],
            .actualModel = &actualModel[5],
            .enemy = &playerData[1],
            .playerKeys = {
                GLFW_KEY_W,
                GLFW_KEY_A,
                GLFW_KEY_S,
                GLFW_KEY_D
            },
            .playerJoystick = GLFW_JOYSTICK_1
        },
        {
            .model = entity[2],
            .actualModel = &actualModel[5],
            .enemy = &playerData[0],
            .playerKeys = {
                GLFW_KEY_UP,
                GLFW_KEY_LEFT,
                GLFW_KEY_DOWN,
                GLFW_KEY_RIGHT
            },
            .playerJoystick = GLFW_JOYSTICK_2
        },
    };

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

    background[0] = (struct instance){
        .pos = { 0.0f, 0.0f, 0.0f }, 
        .rotation = { 0.0f, glm_rad(0.3), 0.0f },
        .fixedRotation = { glm_rad(90), 0.0f, 0.0f },
        .scale = { 1.0f, 1.0f, 1.0f },
        .textureIndex = 0,
        .shadow = false
    };

    while (GAME == *state && !glfwWindowShouldClose(vulkan->window)) {
        if (GLFW_PRESS == glfwGetKey(vulkan->window, GLFW_KEY_END)) {
            glfwSetWindowShouldClose(vulkan->window, GLFW_TRUE);
        }
        glfwPollEvents();

        updateInstances(entity, qEntity, vulkan->deltaTime.deltaTime);

        movePlayer(&playerData[0], vulkan->windowControl, vulkan->deltaTime.deltaTime);
        movePlayer(&playerData[1], vulkan->windowControl, vulkan->deltaTime.deltaTime);

        vec2 diff;
        glm_vec2_sub(player->pos, enemy->pos, diff);
        glm_vec2_normalize(diff);
        renderPass[0].camera = (struct camera) {
            .pos = {
                [0] = player->pos[0] + 2 * diff[0] - 0.5 * diff[1],
                [1] = player->pos[1] + 2 * diff[1] + 0.5 * diff[0],
                [2] = player->pos[2] + 2.5,
            },

            .direction = {
                [0] = enemy->pos[0],
                [1] = enemy->pos[1],
                [2] = enemy->pos[2] + 1
            }
        };

        glm_vec2_sub(enemy->pos, player->pos, diff);
        glm_vec2_normalize(diff);
        renderPass[1].camera = (struct camera) {
            .pos = {
                [0] = enemy->pos[0] + 2 * diff[0] + 0.5 * diff[1],
                [1] = enemy->pos[1] + 2 * diff[1] - 0.5 * diff[0],
                [2] = enemy->pos[2] + 2.5,
            },

            .direction = {
                [0] = player->pos[0],
                [1] = player->pos[1],
                [2] = player->pos[2] + 1
            }
        };

        renderPass[2].camera = (struct camera) {
            .pos = {
                [0] = player->pos[0] - 1,
                [1] = player->pos[1] + 0.4,
                [2] = player->pos[2] + 2,
            },

            .direction = {
                [0] = player->pos[0],
                [1] = player->pos[1],
                [2] = player->pos[2] + 2.3
            }
        };

        renderPass[3].camera = (struct camera) {
            .pos = {
                [0] = enemy->pos[0] + 1,
                [1] = enemy->pos[1] + 0.4,
                [2] = enemy->pos[2] + 2,
            },

            .direction = {
                [0] = enemy->pos[0],
                [1] = enemy->pos[1],
                [2] = enemy->pos[2] + 2.3
            }
        };
        
        drawFrame(vulkan, qRenderPass, renderPass);

        bool isMClicked = (KEY_PRESS | KEY_CHANGE) == getKeyState(vulkan->windowControl, GLFW_KEY_M);

        if (isMClicked) text->shadow = !text->shadow;
    }

    for (size_t i = 0; i < qPipe; i += 1) {
        destroyObjGraphicsPipeline(vulkan->graphics.device, pipe[i]);
    }

    destroyEntityArray(qEntity, entity, &vulkan->graphics);
    destroyRenderPassObj(qRenderPass, renderPass, &vulkan->graphics);

    vkDestroyDescriptorSetLayout(vulkan->graphics.device, objectLayout, NULL);
    vkDestroyDescriptorSetLayout(vulkan->graphics.device, animLayout, NULL);
    vkDestroyDescriptorSetLayout(vulkan->graphics.device, cameraLayout, NULL);

    destroyActualModels(vulkan->graphics.device, modelQuantity, actualModel);
    unloadTextures(vulkan->graphics.device, texture);
    unloadTextures(vulkan->graphics.device, cubeMap);

    ma_sound_uninit(&sound);
    ma_engine_uninit(&engine);
}
