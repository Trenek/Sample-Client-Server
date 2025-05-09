#ifndef CAMERA_H
#define CAMERA_H

#include <cglm.h>

struct camera {
    vec3 cameraPos;
    vec3 center;
    vec2 tilt;
};

struct windowControl;
typedef struct GLFWwindow GLFWwindow;

struct camera initCamera();
void moveCamera(struct windowControl *windowControl, GLFWwindow *window, vec3 direction, vec3 cameraPos, vec2 cameraTilt, float deltaTime);

#endif
