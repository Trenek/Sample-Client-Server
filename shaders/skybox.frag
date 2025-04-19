#version 450

layout(location = 0) in vec3 fragTexCoords;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform samplerCube skybox[1];

void main() {
    outColor = texture(skybox[0], fragTexCoords);
}

