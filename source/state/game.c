#include <string.h>

#include "VulkanTools.h"
#include "state.h"

#include "model.h"
#include "modelBuilder.h"
#include "instanceBuffer.h"

#include "modelFunctions.h"

void loadModel(struct actualModel *model, const char *modelPath, struct GraphicsSetup *vulkan) {
    (NULL != strstr(modelPath, ".obj") ? objLoadModel : gltfLoadModel)(
        modelPath, 
        model,
        vulkan->device,
        vulkan->physicalDevice,
        vulkan->surface
    );

    for (uint32_t i = 0; i < model->meshQuantity; i += 1) {
        model->mesh[i].vertexBuffer = createVertexBuffer(&model->mesh[i].vertexBufferMemory, vulkan->device, vulkan->physicalDevice, vulkan->surface, vulkan->commandPool, vulkan->transferQueue, model->mesh[i].verticesQuantity, model->mesh[i].vertices);
        model->mesh[i].indexBuffer = createIndexBuffer(&model->mesh[i].indexBufferMemory, vulkan->device, vulkan->physicalDevice, vulkan->surface, vulkan->commandPool, vulkan->transferQueue, model->mesh[i].verticesQuantity, model->mesh[i].indicesQuantity, model->mesh[i].indices);
    }
}

void loadModels(size_t quantity, struct actualModel model[quantity], const char *modelPath[quantity], struct GraphicsSetup *vulkan) {
    for (size_t i = 0; i < quantity; i += 1) {
        loadModel(&model[i], modelPath[i], vulkan);
    }
}

void game(struct VulkanTools *vulkan, enum state *state) {
    const char *texturePaths[] = {
        "textures/pause.png",
        "textures/grass.jpg",
        "textures/texture.jpg",
    };
    size_t texturesQuantity = sizeof(texturePaths) / sizeof(const char *);

    struct descriptor textureDesc;
    struct Textures *textureData = loadTextures(&textureDesc, &vulkan->graphics, texturesQuantity, texturePaths);

    const char *modelPath[] = {
        "models/my_model2d.obj",
        "models/my_floor.obj",
        "models/cylinder.glb"
    };
    size_t modelQuantity = sizeof(modelPath) / sizeof(const char *);

    struct actualModel actualModel[sizeof(modelPath) / sizeof(const char *)];

    loadModels(modelQuantity, actualModel, modelPath, &vulkan->graphics); 

    struct Model model[] = {
        /*floor*/ createModels((struct ModelBuilder) {
            .instanceCount = 1,
            .texturesQuantity = 1,
            .texturePointer = 1,
            .texture = &textureDesc,
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
            .texture = &textureDesc,
            .modelPath = &actualModel[2],
            .vertexShader = "shaders/vert.spv",
            .fragmentShader = "shaders/frag.spv",
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        }, &vulkan->graphics),
        /*background*/ createModels((struct ModelBuilder) {
            .instanceCount = 1,
            .texturesQuantity = 1,
            .texturePointer = 2,
            .texture = &textureDesc,
            .modelPath = &actualModel[0],
            .vertexShader = "shaders/vert2d.spv",
            .fragmentShader = "shaders/frag2d.spv",
            .minDepth = 1.0f,
            .maxDepth = 1.0f
        }, &vulkan->graphics),
    };

    struct instance (*player) = model[1].instance;

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
    destroyActualModels(vulkan->graphics.device, sizeof(actualModel) / sizeof(struct actualModel), actualModel);
    unloadTextures(vulkan->graphics.device, texturesQuantity, textureData, textureDesc);
}
