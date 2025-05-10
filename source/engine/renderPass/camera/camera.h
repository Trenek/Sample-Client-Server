#ifndef CAMERA_H
#define CAMERA_H

#include <cglm.h>

struct camera {
    vec3 pos;
    vec3 direction;
    vec2 tilt;
};

struct windowControl;
typedef struct GLFWwindow GLFWwindow;
typedef struct VkExtent2D VkExtent2D;

struct camera initCamera();
void moveCamera(struct windowControl *windowControl, GLFWwindow *window, struct camera *camera, float deltaTime);

void updateFirstPersonCameraBuffer(void *uniformBuffersMapped, VkExtent2D swapChainExtent, vec3 cameraPos, vec3 center);
void updateThirdPersonCameraBuffer(void *uniformBuffersMapped, VkExtent2D swapChainExtent, vec3 cameraPos, vec3 center);

#endif
