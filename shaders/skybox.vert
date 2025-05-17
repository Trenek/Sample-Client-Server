#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in ivec4 inBoneIDs;
layout(location = 4) in  vec4 inWeights;

layout(location = 0) out vec3 fragTexCoords;

layout(set = 2, binding = 0) readonly uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

struct ObjectData {
    uint index;
    mat4 model;
    bool shadow;
};

layout(std140, set = 0, binding = 0) readonly buffer ObjectBuffer{
	ObjectData objects[];
} instance;

void main() {
    fragTexCoords = inPosition;

    mat4 view = ubo.view;
    view[3][0] = 0;
    view[3][1] = 0;
    view[3][2] = 0;

    gl_Position = (ubo.proj * view * instance.objects[gl_InstanceIndex].model * vec4(inPosition, 1.0)).xyww;
}  

