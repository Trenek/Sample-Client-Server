#include "VulkanTools.h"

void getCoordinate(vec3 result, struct VulkanTools vulkan) {
    vec4 viewPort;
    mat4 viewProj;
    vec3 center[2];
    double a = 0.0;
    double b = 0.0;

    glfwGetCursorPos(vulkan.window, &a, &b);

    getViewProj(vulkan, viewProj, viewPort);

    glm_unproject((vec3) { a, b, 0.0f }, viewProj, viewPort, center[0]);
    glm_unproject((vec3) { a, b, 1.0f }, viewProj, viewPort, center[1]);

    glm_vec3_lerp(center[0], center[1], fabs(center[0][2]) / fabs(center[1][2] - center[0][2]), result);
}
