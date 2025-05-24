#include <cglm.h>

#include "engineCore.h"
#include "state.h"

#include "asset.h"
#include "entity.h"
#include "modelBuilder.h"
#include "stringBuilder.h"
#include "instanceBuffer.h"

#include "renderPassCore.h"

#include "graphicsPipelineObj.h"

#include "Vertex.h"

static void addTextures(struct EngineCore *this) {
    struct ResourceManager *textureManager = calloc(1, sizeof(struct ResourceManager));

    addResource(textureManager, "button", loadTextures(&this->graphics, 1, (const char *[]){
        "textures/button.png",
    }), unloadTextures);
    addResource(textureManager, "simpleColors", loadTextures(&this->graphics, 6, (const char *[]){
        "textures/background.png",
        "textures/red.png",
        "textures/red_bg.png",
        "textures/blue.png",
        "textures/blue_bg.png",
        "textures/floor.png",
    }), unloadTextures);
    addResource(textureManager, "cubeMap", loadCubeMaps(&this->graphics, (const char *[]) {
        "textures/CubeMaps/xpos.png",
        "textures/CubeMaps/xneg.png",
        "textures/CubeMaps/ypos.png",
        "textures/CubeMaps/yneg.png",
        "textures/CubeMaps/zpos.png",
        "textures/CubeMaps/zneg.png",
    }), unloadTextures);

    addResource(&this->resource, "textures", textureManager, cleanupResources);
}

static void addModelData(struct EngineCore *this) {
    struct ResourceManager *modelData = calloc(1, sizeof(struct ResourceManager));

    addResource(modelData, "player", loadModel("models/czlowiek.glb", &this->graphics), destroyActualModel);
    addResource(modelData, "font", loadModel("fonts/c.ttf", &this->graphics), destroyActualModel);
    addResource(modelData, "flat", loadModel("models/my_model2d.obj", &this->graphics), destroyActualModel);
    addResource(modelData, "skyBox", loadModel("models/my_skybox.obj", &this->graphics), destroyActualModel);
    addResource(modelData, "floor", loadModel("models/my_floor.obj", &this->graphics), destroyActualModel);
    addResource(modelData, "cube", loadModel("models/my_cube.obj", &this->graphics), destroyActualModel);

    addResource(&this->resource, "modelData", modelData, cleanupResources);
}

static void addRenderPassCoreData(struct EngineCore *this) {
    struct ResourceManager *renderPassCoreData = calloc(1, sizeof(struct ResourceManager));

    addResource(renderPassCoreData, "Clean", createRenderPassCore((struct renderPassCoreBuilder) {
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .initLayout = VK_IMAGE_LAYOUT_UNDEFINED
    }, &this->graphics), freeRenderPassCore);
    addResource(renderPassCoreData, "Stay", createRenderPassCore((struct renderPassCoreBuilder) {
        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
        .initLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    }, &this->graphics), freeRenderPassCore);

    addResource(&this->resource, "RenderPassCoreData", renderPassCoreData, cleanupResources);
}

static void addObjectLayout(struct EngineCore *this) {
    struct ResourceManager *objectLayoutData = calloc(1, sizeof(struct ResourceManager));

    addResource(objectLayoutData, "object", createDescriptorSetLayout(
        createObjectDescriptorSetLayout(this->graphics.device, 2, (VkDescriptorSetLayoutBinding []) {
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
        }), this->graphics.device), 
        destroyDescriptorSetLayout
    );
    addResource(objectLayoutData, "anim", createDescriptorSetLayout(
        createObjectDescriptorSetLayout(this->graphics.device, 3, (VkDescriptorSetLayoutBinding []) {
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
        }), this->graphics.device), 
        destroyDescriptorSetLayout
    );
    addResource(objectLayoutData, "camera", 
        createDescriptorSetLayout(createCameraDescriptorSetLayout(this->graphics.device), this->graphics.device), 
        destroyDescriptorSetLayout
    );

    addResource(&this->resource, "objectLayout", objectLayoutData, cleanupResources);
}

static void createGraphicPipelines(struct EngineCore *this) {
    struct ResourceManager *graphicPipelinesData = calloc(1, sizeof(struct ResourceManager));
    struct ResourceManager *renderPassCoreData = findResource(&this->resource, "RenderPassCoreData");

    struct Textures *texture = findResource(findResource(&this->resource, "textures"), "button");
    struct Textures *colorTexture = findResource(findResource(&this->resource, "textures"), "simpleColors");
    struct Textures *cubeMap = findResource(findResource(&this->resource, "textures"), "cubeMap");
    struct descriptorSetLayout *objectLayout = findResource(findResource(&this->resource, "objectLayout"), "object");
    struct descriptorSetLayout *animLayout = findResource(findResource(&this->resource, "objectLayout"), "anim");
    struct descriptorSetLayout *cameraLayout = findResource(findResource(&this->resource, "objectLayout"), "camera");

    struct renderPassCore *renderPass[] = {
        findResource(renderPassCoreData, "Clean"),
        findResource(renderPassCoreData, "Stay")
    };
    size_t qRenderPass = sizeof(renderPass) / sizeof(struct renderPassCore *);

    addResource(graphicPipelinesData, "Text", createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
        .qRenderPassCore = qRenderPass,
        .renderPassCore = renderPass,
        .vertexShader = "shaders/text2dV.spv",
        .fragmentShader = "shaders/textF.spv",
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .texture = &texture->descriptor,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

        .objectLayout = objectLayout->descriptorSetLayout,

        Vert(FontVertex),
        .operation = VK_COMPARE_OP_LESS,
        .cullFlags = VK_CULL_MODE_BACK_BIT,

        .cameraLayout = cameraLayout->descriptorSetLayout
    }, &this->graphics), destroyObjGraphicsPipeline);
    addResource(graphicPipelinesData, "Flat", createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
        .qRenderPassCore = qRenderPass,
        .renderPassCore = renderPass,
        .vertexShader = "shaders/vert2d.spv",
        .fragmentShader = "shaders/frag2d.spv",
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .texture = &texture->descriptor,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

        .objectLayout = objectLayout->descriptorSetLayout,

        Vert(AnimVertex),
        .operation = VK_COMPARE_OP_LESS,
        .cullFlags = VK_CULL_MODE_NONE,

        .cameraLayout = cameraLayout->descriptorSetLayout
    }, &this->graphics), destroyObjGraphicsPipeline);
    addResource(graphicPipelinesData, "FlatColor", createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
        .qRenderPassCore = qRenderPass,
        .renderPassCore = renderPass,
        .vertexShader = "shaders/vert2d.spv",
        .fragmentShader = "shaders/frag2d.spv",
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .texture = &colorTexture->descriptor,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

        .objectLayout = objectLayout->descriptorSetLayout,

        Vert(AnimVertex),
        .operation = VK_COMPARE_OP_LESS,
        .cullFlags = VK_CULL_MODE_NONE,

        .cameraLayout = cameraLayout->descriptorSetLayout
    }, &this->graphics), destroyObjGraphicsPipeline);
    addResource(graphicPipelinesData, "Skybox", createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
        .qRenderPassCore = qRenderPass,
        .renderPassCore = renderPass,
        .vertexShader = "shaders/skyboxV.spv",
        .fragmentShader = "shaders/skyboxF.spv",
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .texture = &cubeMap->descriptor,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

        .objectLayout = objectLayout->descriptorSetLayout,

        Vert(AnimVertex),
        .operation = VK_COMPARE_OP_LESS_OR_EQUAL,
        .cullFlags = VK_CULL_MODE_BACK_BIT,

        .cameraLayout = cameraLayout->descriptorSetLayout
    }, &this->graphics), destroyObjGraphicsPipeline);
    addResource(graphicPipelinesData, "Animated Model", createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
        .qRenderPassCore = qRenderPass,
        .renderPassCore = renderPass,
        .vertexShader = "shaders/playerAnimation.spv",
        .fragmentShader = "shaders/playerFrag.spv",
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .texture = &texture->descriptor,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

        .objectLayout = animLayout->descriptorSetLayout,

        Vert(AnimVertex),
        .operation = VK_COMPARE_OP_LESS,
        .cullFlags = VK_CULL_MODE_NONE,

        .cameraLayout = cameraLayout->descriptorSetLayout
    }, &this->graphics), destroyObjGraphicsPipeline);
    addResource(graphicPipelinesData, "Floor", createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
        .qRenderPassCore = qRenderPass,
        .renderPassCore = renderPass,
        .vertexShader = "shaders/vert.spv",
        .fragmentShader = "shaders/frag.spv",
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

        .texture = &colorTexture->descriptor,
        .objectLayout = objectLayout->descriptorSetLayout,

        Vert(AnimVertex),
        .operation = VK_COMPARE_OP_LESS,
        .cullFlags = VK_CULL_MODE_BACK_BIT,

        .cameraLayout = cameraLayout->descriptorSetLayout
    }, &this->graphics), destroyObjGraphicsPipeline);

    addResource(&this->resource, "graphicPipelines", graphicPipelinesData, cleanupResources);
}

void addString(
    struct ResourceManager *entityData,
    struct ResourceManager *modelData,

    struct descriptorSetLayout *objectLayout,
    struct EngineCore *this,
    const char *buffer
) {
    addResource(entityData, buffer, createString((struct StringBuilder) {
        .instanceCount = 1,
        .string = buffer,
        .modelData = findResource(modelData, "font"),
        .objectLayout = objectLayout->descriptorSetLayout,

        INS(instance, instanceBuffer),
        .center = 0
    }, &this->graphics), destroyEntity);
}

static void addEntities(struct EngineCore *this) {
    struct ResourceManager *entityData = calloc(1, sizeof(struct ResourceManager));
    struct ResourceManager *modelData = findResource(&this->resource, "modelData");

    struct descriptorSetLayout *objectLayout = findResource(findResource(&this->resource, "objectLayout"), "object");
    struct descriptorSetLayout *animLayout = findResource(findResource(&this->resource, "objectLayout"), "anim");

    addResource(entityData, "Flat", createModel((struct ModelBuilder) {
        .instanceCount = 3,
        .modelData = findResource(modelData, "flat"),
        .objectLayout = objectLayout->descriptorSetLayout,

        INS(instance, instanceBuffer),
    }, &this->graphics), destroyEntity);
    addResource(entityData, "Floor", createModel((struct ModelBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, "floor"),
        .objectLayout = objectLayout->descriptorSetLayout,

        INS(instance, instanceBuffer),
    }, &this->graphics), destroyEntity);
    addResource(entityData, "Cube", createModel((struct ModelBuilder) {
        .instanceCount = 4,
        .modelData = findResource(modelData, "cube"),
        .objectLayout = objectLayout->descriptorSetLayout,

        INS(instance, instanceBuffer),
    }, &this->graphics), destroyEntity);
    addResource(entityData, "Background", createModel((struct ModelBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, "skyBox"),
        .objectLayout = objectLayout->descriptorSetLayout,

        INS(instance, instanceBuffer),
    }, &this->graphics), destroyEntity);
    addString(entityData, modelData, objectLayout, this, "Main Menu");
    addString(entityData, modelData, objectLayout, this, "Restart");
    addString(entityData, modelData, objectLayout, this, "Play");
    addString(entityData, modelData, objectLayout, this, "Exit");
    addString(entityData, modelData, objectLayout, this, "Fight!");
    addString(entityData, modelData, objectLayout, this, "Pause");
    addString(entityData, modelData, objectLayout, this, "Resume");
    addResource(entityData, "Player 1 Text", createString((struct StringBuilder) {
        .instanceCount = 1,
        .string = "Player 1",
        .modelData = findResource(modelData, "font"),
        .objectLayout = objectLayout->descriptorSetLayout,

        INS(instance, instanceBuffer),
        .center = 0
    }, &this->graphics), destroyEntity);
    addResource(entityData, "Player 2 Text", createString((struct StringBuilder) {
        .instanceCount = 1,
        .string = "Player 2",
        .modelData = findResource(modelData, "font"),
        .objectLayout = objectLayout->descriptorSetLayout,

        INS(instance, instanceBuffer),
        .center = 0
    }, &this->graphics), destroyEntity);
    addResource(entityData, "Player 1", createModel((struct ModelBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, "player"),
        .objectLayout = animLayout->descriptorSetLayout,

        INS(playerInstance, playerInstanceBuffer),
    }, &this->graphics), destroyEntity);
    addResource(entityData, "Player 2", createModel((struct ModelBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, "player"),
        .objectLayout = animLayout->descriptorSetLayout,

        INS(playerInstance, playerInstanceBuffer),
    }, &this->graphics), destroyEntity);
    addResource(entityData, "Blue Back", createModel((struct ModelBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, "flat"),
        .objectLayout = objectLayout->descriptorSetLayout,

        INS(instance, instanceBuffer),
    }, &this->graphics), destroyEntity);
    addResource(entityData, "Health", createModel((struct ModelBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, "flat"),
        .objectLayout = objectLayout->descriptorSetLayout,

        INS(instance, instanceBuffer),
    }, &this->graphics), destroyEntity);
    addResource(entityData, "Health Background", createModel((struct ModelBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, "flat"),
        .objectLayout = objectLayout->descriptorSetLayout,

        INS(instance, instanceBuffer),
    }, &this->graphics), destroyEntity);
    addResource(entityData, "Rest", createModel((struct ModelBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, "flat"),
        .objectLayout = objectLayout->descriptorSetLayout,

        INS(instance, instanceBuffer),
    }, &this->graphics), destroyEntity);
    addResource(entityData, "Rest Background", createModel((struct ModelBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, "flat"),
        .objectLayout = objectLayout->descriptorSetLayout,

        INS(instance, instanceBuffer),
    }, &this->graphics), destroyEntity);

    addResource(&this->resource, "Entity", entityData, cleanupResources);
}

static void loadSounds(struct EngineCore *this) {
    loadSound(&this->soundManager, 3, "music/Elevator Music.mp3");
    loadSound(&this->soundManager, 2, "music/Victory Music.mp3");
    loadSound(&this->soundManager, 1, "music/Music Bustin Loose.mp3");
    loadSound(&this->soundManager, 0, "music/Synthwave Music - Hackers by Karl Casey.mp3");
}

void loadResources(struct EngineCore *engine, enum state *state) {
    addTextures(engine);
    addModelData(engine);

    addRenderPassCoreData(engine);
    addObjectLayout(engine);

    createGraphicPipelines(engine);
    addEntities(engine);

    loadSounds(engine);

    *state = MAIN_MENU;
}
