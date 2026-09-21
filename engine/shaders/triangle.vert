#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;
layout(location = 4) in ivec4 aBoneIds;
layout(location = 5) in vec4 aWeights;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;

struct DirectionalLight {
    vec3 direction;
    vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    mat4 invView;
    vec4 ambientLightColor;
    DirectionalLight sunlight;
    int numLights;
} ubo;

layout(set = 0, binding = 1, std430) readonly buffer BoneBuffer {
    mat4 bones[];
};

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
    int uSkinned;
} push;

void main() {
    vec4 localPosition = vec4(position, 1.0);
    vec3 localNormal = normal;

    if (push.uSkinned == 1) {
        mat4 skin = mat4(0.0);
        float totalWeight = 0.0;

        for (int i = 0; i < 4; ++i) {
            int boneId = aBoneIds[i];
            float weight = aWeights[i];

            if (boneId < 0 || weight <= 0.0)
                continue;

            skin += bones[boneId] * weight;
            totalWeight += weight;
        }

        if (totalWeight > 0.0) {
            skin /= totalWeight;
            localPosition = skin * localPosition;

            mat3 skinNormalMatrix = transpose(inverse(mat3(skin)));
            localNormal = normalize(skinNormalMatrix * localNormal);
        }
    }

    vec4 positionWorld = push.modelMatrix * localPosition;
    gl_Position = ubo.projection * ubo.view * positionWorld;

    fragPosWorld = positionWorld.xyz;
    fragNormalWorld = normalize(mat3(push.normalMatrix) * localNormal);
    fragColor = color;
}