#ifndef INSTANCE_H
#define INSTANCE_H

#include <cglm.h>

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
struct Entity;

void updateInstances(struct Entity **model, size_t qModel, float deltaTime);

#endif
