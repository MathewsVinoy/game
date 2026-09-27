#version 450

layout(location = 0) in vec3 fragNormalWorld;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec2 fragUV;

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

layout(set = 1, binding = 0) uniform sampler2D uTex;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;

    vec4 color;

    int uSkinned;
   
   
} push;

void main() {

    // --------------------------------------------------
    // Base color
    // --------------------------------------------------

    vec3 base =
        pow(push.color.rgb, vec3(2.2));

    // --------------------------------------------------
    // Surface normal
    // --------------------------------------------------

    vec3 surfaceNormal =
        normalize(fragNormalWorld);

    // Correct normal for back-facing triangles
    if (!gl_FrontFacing)
        surfaceNormal = -surfaceNormal;

    // --------------------------------------------------
    // Camera direction
    // --------------------------------------------------

    vec3 cameraPosWorld =
        ubo.invView[3].xyz;

    vec3 viewDirection =
        normalize(
            cameraPosWorld - fragPosWorld
        );

    // --------------------------------------------------
    // Directional light
    // --------------------------------------------------

    vec3 lightDirection =
        normalize(
            -ubo.sunlight.direction
        );

    vec3 lightIntensity =
        ubo.sunlight.color.xyz *
        ubo.sunlight.color.w;

    // --------------------------------------------------
    // Ambient light
    // --------------------------------------------------

    vec3 diffuseLight =
        ubo.ambientLightColor.xyz *
        ubo.ambientLightColor.w;

    // --------------------------------------------------
    // Diffuse lighting
    // --------------------------------------------------

    float cosAngIncidence =
        max(
            dot(
                surfaceNormal,
                lightDirection
            ),
            0.0
        );

    diffuseLight +=
        lightIntensity *
        cosAngIncidence;

    // --------------------------------------------------
    // Blinn-Phong specular
    // --------------------------------------------------

    vec3 halfAngle =
        normalize(
            lightDirection +
            viewDirection
        );

    float blinnTerm =
        max(
            dot(
                surfaceNormal,
                halfAngle
            ),
            0.0
        );

    blinnTerm =
        pow(blinnTerm, 32.0);

    vec3 specularLight =
        lightIntensity *
        blinnTerm *
        0.15;

    // --------------------------------------------------
    // Final color
    // --------------------------------------------------

    vec3 diffuseColor =
        diffuseLight * base;

    vec3 finalColor =
        diffuseColor +
        specularLight;

    outColor =
        vec4(finalColor, 1.0);
}