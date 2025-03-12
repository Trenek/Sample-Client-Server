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
    vec3 scale;
    bool shadow;
};

#endif
