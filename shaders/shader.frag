#version 450

#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in flat uint fragTexIndex;
layout(location = 3) in flat uint shadow;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D texSampler[];

void main() {
    vec2 foo = vec2(int(fragTexCoord.x * 10) / 10.0, int(fragTexCoord.y * 10) / 10.0);
    outColor = vec4(foo.x, 1.0, foo.y, 1.0);
}
