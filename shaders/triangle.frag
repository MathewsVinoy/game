
#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;
layout(location = 3) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

// Texture descriptor: set 1, binding 0
layout(set = 1, binding = 0) uniform sampler2D diffuseTexture;

struct DirectionalLight {
    vec4 direction;
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
    int uSkinned;
    int hasTexture;
} push;


void main() {
    vec3 normal = normalize(fragNormalWorld);

    vec3 lightDirection =
        normalize(-ubo.sunlight.direction.xyz);

    vec3 cameraPosWorld = ubo.invView[3].xyz;
    vec3 viewDirection =
        normalize(cameraPosWorld - fragPosWorld);

    // Sample the diffuse texture if one is bound;
    // otherwise fall back to opaque white.
    vec4 texel = (push.hasTexture != 0)
        ? texture(diffuseTexture, fragUV)
        : vec4(1.0);

    // Combine texture and vertex color.
    vec3 baseColor = texel.rgb * fragColor;

    // Ambient lighting with a small minimum contribution.
    vec3 ambient =
        ubo.ambientLightColor.xyz *
        ubo.ambientLightColor.w;

    ambient = max(ambient, vec3(0.12));

    // Directional diffuse lighting.
    vec3 lightColor =
        ubo.sunlight.color.xyz *
        ubo.sunlight.color.w;

    float diffuseFactor =
        max(dot(normal, lightDirection), 0.0);

    vec3 diffuse = lightColor * diffuseFactor;

    // Blinn-Phong specular lighting.
    vec3 halfDirection =
        normalize(lightDirection + viewDirection);

    float specularFactor = 0.0;

    if (diffuseFactor > 0.0) {
        specularFactor = pow(
            max(dot(normal, halfDirection), 0.0),
            32.0
        );
    }

    vec3 specular = lightColor * specularFactor * 0.15;

    vec3 finalColor =
        baseColor * (ambient + diffuse) + specular;

    outColor = vec4(finalColor, texel.a);
}