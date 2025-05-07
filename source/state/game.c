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

#include "Vertex.h"

#include "player.h"

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
        }, &vulkan->graphics),
        /* Texture Model */ createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
            .vertexShader = "shaders/animation.spv",
            .fragmentShader = "shaders/frag.spv",
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
        }, &vulkan->graphics),
    };
    size_t qPipe = sizeof(pipe) / sizeof(struct graphicsPipeline);

    struct Entity *model[] = {
        /*floor*/ createModel((struct ModelBuilder) {
            .instanceCount = 1,
            .modelData = &actualModel[1],
            .objectLayout = objectLayout,

            .instanceSize = sizeof(struct instance),
            .instanceBufferSize = sizeof(struct instanceBuffer)
        }, &vulkan->graphics),
        /*player*/ createModel((struct ModelBuilder) {
            .instanceCount = 1,
            .modelData = &actualModel[5],
            .objectLayout = animLayout,

            .instanceSize = sizeof(struct instance),
            .instanceBufferSize = sizeof(struct instanceBuffer)
        }, &vulkan->graphics),
        /*enemy*/ createModel((struct ModelBuilder) {
            .instanceCount = 1,
            .modelData = &actualModel[5],
            .objectLayout = animLayout,

            .instanceSize = sizeof(struct instance),
            .instanceBufferSize = sizeof(struct instanceBuffer)
        }, &vulkan->graphics),
        /*text*/ createString((struct StringBuilder) {
            .instanceCount = 1,
            .string = "Hello Animation World!",
            .modelData = &actualModel[3],
            .objectLayout = objectLayout,

            .instanceSize = sizeof(struct instance),
            .instanceBufferSize = sizeof(struct instanceBuffer)
        }, &vulkan->graphics),
        /*background*/ createModel((struct ModelBuilder) {
            .instanceCount = 1,
            .modelData = &actualModel[4],
            .objectLayout = objectLayout,

            .instanceSize = sizeof(struct instance),
            .instanceBufferSize = sizeof(struct instanceBuffer)
        }, &vulkan->graphics),
    };

    pipe[0].modelQuantity = 1;
    pipe[1].modelQuantity = 2;
    pipe[2].modelQuantity = 1;
    pipe[3].modelQuantity = 1;

    pipe[0].model = model;
    for (size_t i = 1; i < qPipe; i += 1) {
        pipe[i].model = pipe[i - 1].model + pipe[i - 1].modelQuantity;
    }

    struct instance *floor = pipe[0].model[0]->instance;
    struct instance *player = pipe[1].model[0]->instance;
    struct instance *enemy = pipe[1].model[1]->instance;
    struct instance *text = pipe[2].model[0]->instance;
    struct instance *background = pipe[3].model[0]->instance;

    struct player playerData[2] = {
        {
            .model = pipe[1].model[0],
            .actualModel = &actualModel[5]
        },
        {
            .model = pipe[1].model[1],
            .actualModel = &actualModel[5]
        },
    };

    floor[0] = (struct instance){
        .pos = { 0.0f, 0.0f, -5.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { glm_rad(90), 0.0f, 0.0f },
        .scale = { 10000.0f, 10.0f, 10000.0f },
        .textureIndex = 0
    };

    player[0] = (struct instance){
        .pos = { 0.0f, 0.0f, 1.5f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { glm_rad(90), glm_rad(-90), 0.0f },
        .scale = { 1.5 * 10e-2, 1.5 * 10e-2, 1.5 * 10e-2 },
        .textureIndex = 0
    };

    enemy[0] = (struct instance){
        .pos = { -5.0f, 0.0f, 1.5f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { glm_rad(90), glm_rad(90), 0.0f },
        .scale = { 1.5 * 10e-2, 1.5 * 10e-2, 1.5 * 10e-2 },
        .textureIndex = 0
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

    vulkan->camera = initCamera();

    while (GAME == *state && !glfwWindowShouldClose(vulkan->window)) {
        glfwPollEvents();

        updateInstances(model, sizeof(model) / sizeof(struct Entity *), vulkan->deltaTime.deltaTime);

        movePlayer(&playerData[0], vulkan->windowControl, vulkan->deltaTime.deltaTime);
        moveEnemy(&playerData[1], vulkan->windowControl, vulkan->deltaTime.deltaTime);

        drawFrame(vulkan, qPipe, pipe);
        moveCamera(vulkan->windowControl, vulkan->window, vulkan->camera.center, vulkan->camera.cameraPos, vulkan->camera.tilt, vulkan->deltaTime.deltaTime / 5.0f);

        memcpy(background->pos, vulkan->camera.cameraPos, sizeof(vec3));
        
        bool isMClicked = (KEY_PRESS | KEY_CHANGE) == getKeyState(vulkan->windowControl, GLFW_KEY_M);

        if (isMClicked) text->shadow = !text->shadow;
    }

    for (size_t i = 0; i < qPipe; i += 1) {
        destroyEntityArray(pipe[i].modelQuantity, pipe[i].model, &vulkan->graphics);
        destroyObjGraphicsPipeline(vulkan->graphics.device, pipe[i]);
    }

    vkDestroyDescriptorSetLayout(vulkan->graphics.device, objectLayout, NULL);
    vkDestroyDescriptorSetLayout(vulkan->graphics.device, animLayout, NULL);

    destroyActualModels(vulkan->graphics.device, modelQuantity, actualModel);
    unloadTextures(vulkan->graphics.device, texture);
    unloadTextures(vulkan->graphics.device, cubeMap);

    ma_sound_uninit(&sound);
    ma_engine_uninit(&engine);
}
