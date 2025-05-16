#ifndef INSTANCE_H
#define INSTANCE_H

#include <cglm.h>

#define INS(x, y) \
    .instanceSize = sizeof(struct x), \
    .instanceBufferSize = sizeof(struct y), \
    .instanceUpdater = x##Updater

struct instanceBuffer {
    uint32_t textureIndex;
    mat4 modelMatrix;
    bool shadow;
};

struct instance {
    uint32_t textureIndex;
    uint32_t textureInc;
    vec3 pos;
    vec3 rotation;
    vec3 fixedRotation;
    vec3 scale;
    bool shadow;
};

struct playerInstanceBuffer {
    mat4 modelMatrix;
    vec4 dressColor;
    vec4 skinColor;
};

struct playerInstance {
    vec3 pos;
    vec3 rotation;
    vec3 fixedRotation;
    vec3 scale;
    vec3 dressColor;
    vec3 skinColor;
};

struct Entity;
void updateInstances(struct Entity **model, size_t qModel, float deltaTime);
void playerInstanceUpdater(void *instancePtr, void *instanceBufferPtr, uint32_t instanceCount, float deltaTime);
void instanceUpdater(void *instancePtr, void *instanceBufferPtr, uint32_t instanceCount, float deltaTime);
#endif
