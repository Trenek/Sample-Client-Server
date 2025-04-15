#include "VulkanTools.h"
#include "state.h"

#include "asset.h"

#include "entity.h"
#include "entityBuilder.h"
#include "modelBuilder.h"
#include "stringBuilder.h"
#include "instanceBuffer.h"

#include "graphicsPipelineObj.h"

#include "Vertex.h"

void game(struct VulkanTools *vulkan, enum state *state) {
    const char *texturePaths[] = {
        "textures/texture.jpg"
    };
    size_t texturesQuantity = sizeof(texturePaths) / sizeof(const char *);

    struct Textures texture = loadTextures(&vulkan->graphics, texturesQuantity, texturePaths);

    const char *modelPath[] = {
        "models/my_model2d.obj",
        "models/my_floor.obj",
        "models/cylinder.glb",
        "fonts/c.ttf",
    };
    size_t modelQuantity = sizeof(modelPath) / sizeof(const char *);

    struct actualModel actualModel[modelQuantity]; {
        loadModels(modelQuantity, actualModel, modelPath, &vulkan->graphics);
    }

    struct graphicsPipeline pipe[] = { 
        createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
            .vertexShader = "shaders/vert.spv",
            .fragmentShader = "shaders/frag.spv",
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
            .texture = &texture.descriptor,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

            .sizeOfVertex = sizeof(struct Vertex),
            .numOfAttributes = sizeof(vertexAttributeDescriptions) / sizeof(*vertexAttributeDescriptions),
            .attributeDescription = vertexAttributeDescriptions
        }, &vulkan->graphics),
        createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
            .vertexShader = "shaders/text2dV.spv",
            .fragmentShader = "shaders/textF.spv",
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
            .texture = &texture.descriptor,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

            .sizeOfVertex = sizeof(struct FontVertex),
            .numOfAttributes = sizeof(fontVertexAttributeDescriptions) / sizeof(*fontVertexAttributeDescriptions),
            .attributeDescription = fontVertexAttributeDescriptions
        }, &vulkan->graphics),
        createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
            .vertexShader = "shaders/vert2d.spv",
            .fragmentShader = "shaders/frag2d.spv",
            .minDepth = 1.0f,
            .maxDepth = 1.0f,
            .texture = &texture.descriptor,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

            .sizeOfVertex = sizeof(struct Vertex),
            .numOfAttributes = sizeof(vertexAttributeDescriptions) / sizeof(*vertexAttributeDescriptions),
            .attributeDescription = vertexAttributeDescriptions
        }, &vulkan->graphics),
    };

    struct Entity model1[] = {
        /*floor*/ createModel((struct ModelBuilder) {
            .instanceCount = 1,
            .texturesQuantity = 1,
            .texturePointer = 0,
            .modelData = &actualModel[1],
            .objectLayout = pipe[0].objectLayout,
        }, &vulkan->graphics),
        /*players*/ createModel((struct ModelBuilder) {
            .instanceCount = 2,
            .texturesQuantity = 1,
            .texturePointer = 0,
            .modelData = &actualModel[2],
            .objectLayout = pipe[0].objectLayout,
        }, &vulkan->graphics),
    };

    pipe[0].modelQuantity = sizeof(model1) / sizeof(struct Entity);
    pipe[0].model = model1;
    
    struct Entity model2[] = {
        /*background*/ createModel((struct ModelBuilder) {
            .instanceCount = 1,
            .texturesQuantity = 1,
            .texturePointer = 0,
            .modelData = &actualModel[0],
            .objectLayout = pipe[2].objectLayout,
        }, &vulkan->graphics),
    };

    pipe[2].modelQuantity = sizeof(model2) / sizeof(struct Entity);
    pipe[2].model = model2;

    struct Entity model3[] = {
        /*text*/ createString((struct StringBuilder) {
            .instanceCount = 1,
            .string = "Hello World!",
            .modelData = &actualModel[3],
            .objectLayout = pipe[1].objectLayout,
        }, &vulkan->graphics),
    };

    pipe[1].modelQuantity = sizeof(model3) / sizeof(struct Entity);
    pipe[1].model = model3;

    struct instance *floor = pipe[0].model[0].instance;
    struct instance *player = pipe[0].model[1].instance;
    struct instance *background = pipe[2].model[0].instance;

    struct instance *text = pipe[1].model[0].instance;

    *floor = (struct instance){
        .pos = { 0.0f, 0.0f, -50.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { glm_rad(90), 0.0f, 0.0f },
        .scale = { 10000.0f, 100.0f, 10000.0f },
        .textureIndex = 0
    };

    player[0] = (struct instance){
        .pos = { 0.0f, -1.0f, 1.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { glm_rad(90), 0.0f, 0.0f },
        .scale = { 1.0f, 1.0f, 1.0f },
        .textureIndex = 0
    };

    player[1] = (struct instance){
        .pos = { 0.0f, 1.0f, 1.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { glm_rad(90), 0.0f, 0.0f },
        .scale = { 1.0f, 1.0f, 1.0f },
        .textureIndex = 0
    };

    *background = (struct instance){
        .pos = { 0.0f, 0.0f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { glm_rad(90), 0.0f, 0.0f },
        .scale = { 1.0f, 1.0f, 1.0f },
        .textureIndex = 0,
        .shadow = false
    };

    *text = (struct instance){
        .pos = { -0.2f, -0.5f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 0.1f, 0.1f, 0.1f },
        .textureIndex = 0,
        .shadow = false
    };

    vulkan->camera = initCamera();
    while (GAME == *state && !glfwWindowShouldClose(vulkan->window)) {
        glfwPollEvents();

        drawFrame(vulkan, sizeof(pipe) / sizeof(struct graphicsPipeline), pipe);
        moveCamera(vulkan->windowControl, vulkan->window, vulkan->camera.center, vulkan->camera.cameraPos, vulkan->camera.tilt, vulkan->deltaTime.deltaTime / 5.0f);
        
        bool isUClicked = KEY_PRESS & getKeyState(vulkan->windowControl, GLFW_KEY_U);
        bool isHClicked = KEY_PRESS & getKeyState(vulkan->windowControl, GLFW_KEY_H);
        bool isJClicked = KEY_PRESS & getKeyState(vulkan->windowControl, GLFW_KEY_J);
        bool isKClicked = KEY_PRESS & getKeyState(vulkan->windowControl, GLFW_KEY_K);
        bool isMClicked = (KEY_PRESS | KEY_CHANGE) == getKeyState(vulkan->windowControl, GLFW_KEY_M);

        if (isJClicked) player[0].pos[0] += 3e00 * vulkan->deltaTime.deltaTime;
        if (isKClicked) player[0].pos[1] += 3e00 * vulkan->deltaTime.deltaTime;
        if (isUClicked) player[0].pos[0] -= 3e00 * vulkan->deltaTime.deltaTime;
        if (isHClicked) player[0].pos[1] -= 3e00 * vulkan->deltaTime.deltaTime;

        if (isMClicked) text->shadow = !text->shadow;
    }

    for (size_t i = 0; i < sizeof(pipe) / sizeof(struct graphicsPipeline); i += 1) {
        destroyObjGraphicsPipeline(vulkan->graphics.device, pipe[i]);
    }

    destroyEntityArray(sizeof(model1) / sizeof(struct Entity), model1, &vulkan->graphics);
    destroyEntityArray(sizeof(model2) / sizeof(struct Entity), model2, &vulkan->graphics);
    destroyEntityArray(sizeof(model3) / sizeof(struct Entity), model3, &vulkan->graphics);

    destroyActualModels(vulkan->graphics.device, modelQuantity, actualModel);
    unloadTextures(vulkan->graphics.device, texture);
}
