#ifndef UNIFORM_BUFFER_OBJECT_H
#define UNIFORM_BUFFER_OBJECT_H

#define GLM_FORCE_RADIANS
#include <cglm.h>
struct CameraBuffer {
    alignas(16) mat4 view;
    alignas(16) mat4 proj;
};

#endif
