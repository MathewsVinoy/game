#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;

layout(location = 0) out vec4 outColor;

struct DirectionalLight {
    vec3 direction; // Direction of the light (e.g., sunlight)
    vec4 color;     // xyz = color, w = intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    mat4 invView;

    vec4 ambientLightColor; // xyz = color, w = intensity
    DirectionalLight sunlight; // Sunlight (directional light)
    int numLights;
} ubo;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

void main() {
    vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    vec3 specularLight = vec3(0.0);

    vec3 surfaceNormal = normalize(fragNormalWorld);
    vec3 cameraPosWorld = ubo.invView[3].xyz;
    vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

    // Directional light (sunlight) calculations
    vec3 lightDirection = normalize(-ubo.sunlight.direction); // Negative because light direction points toward the light
    vec3 lightIntensity = ubo.sunlight.color.xyz * ubo.sunlight.color.w;

    // Diffuse component
    float cosAngIncidence = max(dot(surfaceNormal, lightDirection), 0.0);
    diffuseLight += lightIntensity * cosAngIncidence;

    // Specular component (Blinn-Phong)
    vec3 halfAngle = normalize(lightDirection + viewDirection);
    float blinnTerm = max(dot(surfaceNormal, halfAngle), 0.0);
    blinnTerm = pow(blinnTerm, 64.0); // Shininess
    specularLight += lightIntensity * blinnTerm;

    vec3 diffuseColor = diffuseLight * fragColor;
    vec3 finalColor = diffuseColor + specularLight;

    outColor = vec4(finalColor, 1.0);
}