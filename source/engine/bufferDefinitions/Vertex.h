#ifndef STRUCT_VERTEX
#define STRUCT_VERTEX

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <cglm.h>

struct Vertex {
    vec3 pos;
    vec3 color;
    vec2 texCoord;
};

#endif
