struct camera {
    vec3 cameraPos;
    vec3 center;
    vec2 tilt;
};

struct camera initCamera();
void moveCamera(struct windowControl *windowControl, GLFWwindow *window, vec3 direction, vec3 cameraPos, vec2 cameraTilt, float deltaTime);
