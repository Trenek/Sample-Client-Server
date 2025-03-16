#include "VulkanTools.h"
#include "state.h"

#include "asset.h"

#include "model.h"
#include "modelBuilder.h"
#include "instanceBuffer.h"

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

    struct Model model[] = {
        /*floor*/ createModels((struct ModelBuilder) {
            .instanceCount = 1,
            .texturesQuantity = 1,
            .texturePointer = 0,
            .texture = &texture.descriptor,
            .modelPath = &actualModel[1],
            .vertexShader = "shaders/vert.spv",
            .fragmentShader = "shaders/frag.spv",
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        }, &vulkan->graphics),
        /*players*/ createModels((struct ModelBuilder) {
            .instanceCount = 2,
            .texturesQuantity = 1,
            .texturePointer = 0,
            .texture = &texture.descriptor,
            .modelPath = &actualModel[2],
            .vertexShader = "shaders/vert.spv",
            .fragmentShader = "shaders/frag.spv",
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        }, &vulkan->graphics),
        /*background*/ createModels((struct ModelBuilder) {
            .instanceCount = 1,
            .texturesQuantity = 1,
            .texturePointer = 0,
            .texture = &texture.descriptor,
            .modelPath = &actualModel[0],
            .vertexShader = "shaders/vert2d.spv",
            .fragmentShader = "shaders/frag2d.spv",
            .minDepth = 1.0f,
            .maxDepth = 1.0f
        }, &vulkan->graphics),
    };

    struct instance *player = model[1].instance;

    *model[0].instance = (struct instance){
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

    *model[2].instance = (struct instance){
        .pos = { 0.0f, 0.0f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 1.0f, 1.0f, 1.0f },
        .textureIndex = 0,
        .shadow = false
    };

    vulkan->camera = initCamera();
    while (GAME == *state && !glfwWindowShouldClose(vulkan->window)) {
        glfwPollEvents();

        drawFrame(vulkan, sizeof(model) / sizeof(struct Model), model);
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

    destroyModelArray(sizeof(model) / sizeof(struct Model), model, &vulkan->graphics);

    destroyActualModels(vulkan->graphics.device, modelQuantity, actualModel);
    unloadTextures(vulkan->graphics.device, texture);
}
