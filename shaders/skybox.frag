#version 450

layout(location = 0) in vec3 fragTexCoords;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform samplerCube skybox[1];

void main() {
    vec4 color = texture(skybox[0], fragTexCoords);

    if (color.x == 0 && color.y == 0 && color.z == 0) {
        outColor = vec4(
            int((fragTexCoords.x + 1) * 2) / 4.0,
            int((fragTexCoords.y + 1) * 2) / 4.0,
            int((fragTexCoords.z + 1) * 2) / 4.0,
            1.0
        );
    }
    else {
        outColor = color;
    }
}

