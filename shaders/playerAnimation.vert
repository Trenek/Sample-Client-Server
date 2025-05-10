#version 450

layout(location = 0) in  vec3 inPosition;
layout(location = 1) in  vec3 inColor;
layout(location = 2) in  vec2 inTexCoord;
layout(location = 3) in ivec4 inBoneIDs;
layout(location = 4) in  vec4 inWeights;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec4 dressColor;
layout(location = 3) out vec4 skinColor;

layout(set = 2, binding = 0) readonly uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

struct ObjectData {
    mat4 model;
    vec4 dressColor;
    vec4 skinColor;
};

layout(std140, set = 0, binding = 0) readonly buffer ObjectBuffer{
	ObjectData objects[];
} instance;

layout(std140, set = 0, binding = 1) readonly buffer MeshBuffer{
	mat4 localModel[];
} mesh;

layout(std140, set = 0, binding = 2) readonly buffer AnimBuffer{
	mat4 transform[];
} joint;

layout(push_constant) uniform constants {
	int meshID;
} PushConstants;

vec3 applyBoneTransform(vec4 p) {
    vec4 result = vec4(0.0);

    for (int i = 0; i < 4; i += 1) {
        result += joint.transform[inBoneIDs[i]] * (inWeights[i] * p);
    }

    return result.xyz;
}

void main() {
    vec3 position = applyBoneTransform(vec4(inPosition, 1.0));

    gl_Position = 
        ubo.proj * 
        ubo.view * 
        instance.objects[gl_InstanceIndex].model * 
        mesh.localModel[PushConstants.meshID] * 
        vec4(position, 1.0);

    fragColor = inColor;
    fragTexCoord = inTexCoord;
    dressColor = instance.objects[gl_InstanceIndex].dressColor;
    skinColor = instance.objects[gl_InstanceIndex].skinColor;
}
