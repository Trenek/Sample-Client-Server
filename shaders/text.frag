#version 450

#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragBezzier;
layout(location = 2) in flat uint fragInOut;
layout(location = 3) in flat uint shadow;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D texSampler[];

void main() {
    float x = fragBezzier.x;
    float y = fragBezzier.y;
    bool fill = fragInOut == 2 || ((fragInOut == 0) ? y >= x * x : y <= x * x);

    if (shadow == 0 && false == fill) discard;

    outColor = vec4(fragColor, 1);
}
