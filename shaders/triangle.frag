
#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;

layout(location = 0) out vec4 outColor;

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

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

void main() {
    vec3 normal = normalize(fragNormalWorld);

    vec3 lightDirection =
        normalize(-ubo.sunlight.direction);

    vec3 cameraPosWorld = ubo.invView[3].xyz;
    vec3 viewDirection =
        normalize(cameraPosWorld - fragPosWorld);

    // Ambient lighting
    vec3 ambient =
        ubo.ambientLightColor.xyz *
        ubo.ambientLightColor.w;

    // Diffuse lighting
    vec3 lightColor =
        ubo.sunlight.color.xyz *
        ubo.sunlight.color.w;

    float diffuseFactor =
        max(dot(normal, lightDirection), 0.0);

    vec3 diffuse = lightColor * diffuseFactor;

    // Blinn-Phong specular lighting
    vec3 halfDirection =
        normalize(lightDirection + viewDirection);

    float specularFactor = 0.0;

    if (diffuseFactor > 0.0) {
        specularFactor = pow(
            max(dot(normal, halfDirection), 0.0),
            32.0
        );
    }

    // Keep highlights subtle
    vec3 specular = lightColor * specularFactor * 0.15;

    vec3 finalColor =
        fragColor * (ambient + diffuse) + specular;

    outColor = vec4(finalColor, 1.0);
}
