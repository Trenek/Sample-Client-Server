#include "VulkanTools.h"
#include "state.h"

#include "asset.h"

#include "model.h"
#include "modelBuilder.h"
#include "instanceBuffer.h"

#include "graphicsPipelineObj.h"

void game(struct VulkanTools *vulkan, enum state *state) {
    const char *texturePaths[] = {
        "textures/texture.jpg"
    };
    size_t texturesQuantity = sizeof(texturePaths) / sizeof(const char *);

    struct Textures texture = loadTextures(&vulkan->graphics, texturesQuantity, texturePaths);

    const char *modelPath[] = {
        "models/my_model2d.obj",
        "models/my_floor.obj",
        "models/cylinder.glb"
    };
    size_t modelQuantity = sizeof(modelPath) / sizeof(const char *);

    struct actualModel actualModel[sizeof(modelPath) / sizeof(const char *)]; {
        loadModels(modelQuantity, actualModel, modelPath, &vulkan->graphics);
    }

    struct graphicsPipeline pipe[] = { 
        createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
            .vertexShader = "shaders/vert.spv",
            .fragmentShader = "shaders/frag.spv",
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
            .texture = &texture.descriptor,
        }, &vulkan->graphics),
        createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
            .vertexShader = "shaders/vert2d.spv",
            .fragmentShader = "shaders/frag2d.spv",
            .minDepth = 1.0f,
            .maxDepth = 1.0f,
            .texture = &texture.descriptor,
        }, &vulkan->graphics),
    };

    struct Model model1[] = {
        /*floor*/ createModels((struct ModelBuilder) {
            .instanceCount = 1,
            .texturesQuantity = 1,
            .texturePointer = 0,
            .modelPath = &actualModel[1],
            .objectLayout = pipe[0].objectLayout,
        }, &vulkan->graphics),
        /*players*/ createModels((struct ModelBuilder) {
            .instanceCount = 2,
            .texturesQuantity = 1,
            .texturePointer = 0,
            .modelPath = &actualModel[2],
            .objectLayout = pipe[0].objectLayout,
        }, &vulkan->graphics),
    };

    pipe[0].modelQuantity = sizeof(model1) / sizeof(struct Model);
    pipe[0].model = model1;
    
    struct Model model2[] = {
        /*background*/ createModels((struct ModelBuilder) {
            .instanceCount = 1,
            .texturesQuantity = 1,
            .texturePointer = 0,
            .modelPath = &actualModel[0],
            .objectLayout = pipe[1].objectLayout,
        }, &vulkan->graphics),
    };

    pipe[1].modelQuantity = sizeof(model2) / sizeof(struct Model);
    pipe[1].model = model2;

    struct instance *floor = pipe[0].model[0].instance;
    struct instance *player = pipe[0].model[1].instance;
    struct instance *background = pipe[1].model[0].instance;

    *floor = (struct instance){
        .pos = { 0.0f, 0.0f, -50.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 10000.0f, 100.0f, 10000.0f },
        .textureIndex = 0
    };

    player[0] = (struct instance){
        .pos = { 0.0f, -1.0f, 1.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 1.0f, 1.0f, 1.0f },
        .textureIndex = 0
    };

    player[1] = (struct instance){
        .pos = { 0.0f, 1.0f, 1.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 1.0f, 1.0f, 1.0f },
        .textureIndex = 0
    };

    *background = (struct instance){
        .pos = { 0.0f, 0.0f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 1.0f, 1.0f, 1.0f },
        .textureIndex = 0,
        .shadow = false
    };

    vulkan->camera = initCamera();
    while (GAME == *state && !glfwWindowShouldClose(vulkan->window)) {
        glfwPollEvents();

        drawFrame(vulkan, sizeof(pipe) / sizeof(struct graphicsPipeline), pipe);
        moveCamera(vulkan->windowControl, vulkan->window, vulkan->camera.center, vulkan->camera.cameraPos, vulkan->camera.tilt, vulkan->deltaTime.deltaTime);
        
        bool isUClicked = KEY_PRESS & getKeyState(vulkan->windowControl, GLFW_KEY_U);
        bool isHClicked = KEY_PRESS & getKeyState(vulkan->windowControl, GLFW_KEY_H);
        bool isJClicked = KEY_PRESS & getKeyState(vulkan->windowControl, GLFW_KEY_J);
        bool isKClicked = KEY_PRESS & getKeyState(vulkan->windowControl, GLFW_KEY_K);

        if (isJClicked) player[0].pos[0] += 3e00 * vulkan->deltaTime.deltaTime;
        if (isUClicked) player[0].pos[0] -= 3e00 * vulkan->deltaTime.deltaTime;
        if (isKClicked) player[0].pos[1] += 3e00 * vulkan->deltaTime.deltaTime;
        if (isHClicked) player[0].pos[1] -= 3e00 * vulkan->deltaTime.deltaTime;
    }

    destroyObjGraphicsPipeline(vulkan->graphics.device, pipe[0]);
    destroyObjGraphicsPipeline(vulkan->graphics.device, pipe[1]);

    destroyModelArray(sizeof(model1) / sizeof(struct Model), model1, &vulkan->graphics);
    destroyModelArray(sizeof(model2) / sizeof(struct Model), model2, &vulkan->graphics);

    destroyActualModels(vulkan->graphics.device, modelQuantity, actualModel);
    unloadTextures(vulkan->graphics.device, texture);
}
