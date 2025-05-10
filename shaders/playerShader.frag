#version 450

#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 dressColor;
layout(location = 3) in vec4 skinColor;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D texSampler[];

void main() {
    if (fragColor[0] > 0.7 && fragColor[1] < 0.2)
        outColor = vec4(dressColor.xyz, 1.0);
    else if (fragColor[0] > 0.7 && fragColor[1] > 0.4 && fragColor[1] < 0.5)
        outColor = vec4(skinColor.xyz, 1.0);
    else 
        outColor = vec4(fragColor, 1.0);
}
