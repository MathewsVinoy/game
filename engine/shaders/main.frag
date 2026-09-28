#version 450

layout(location = 0) in vec3 fragNormalWorld;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outColor;


// ======================================================
// Directional Light
// ======================================================

struct DirectionalLight {
    vec3 direction;
    vec4 color;
};


// ======================================================
// Global Uniform Buffer
// ======================================================

layout(set = 0, binding = 0) uniform GlobalUbo {

    mat4 projection;
    mat4 view;
    mat4 invView;

    vec4 ambientLightColor;

    DirectionalLight sunlight;

    int numLights;

} ubo;


// ======================================================
// Push Constants
// ======================================================

layout(push_constant) uniform Push {

    mat4 modelMatrix;
    mat4 normalMatrix;

    // RGB = object color
    // A = 0 -> checkerboard ground
    // A = 1 -> normal object
    vec4 color;

    int uSkinned;

} push;


void main()
{
    // ==================================================
    // BASE COLOR
    // ==================================================

    vec3 base;


    // --------------------------------------------------
    // CHECKERBOARD GROUND
    // --------------------------------------------------

    if (push.color.a < 0.5)
    {
        // The reference image uses a larger checker pattern.
        float tileSize = 8.0;


        // Determine which tile we are inside
        vec2 tile =
            floor(
                fragPosWorld.xz / tileSize
            );


        // Alternate between 0 and 1
        float check =
            mod(
                tile.x + tile.y,
                2.0
            );


        // Reference floor colors: cool charcoal dark squares and
        // soft light-gray tiles with a slightly blue undertone.
        vec3 darkTile =
            vec3(
                0.43,
                0.45,
                0.49
            );


        vec3 lightTile =
            vec3(
                0.72,
                0.74,
                0.77
            );


        // Select tile color
        base =
            mix(
                darkTile,
                lightTile,
                check
            );
    }


    // --------------------------------------------------
    // NORMAL OBJECT COLOR
    // --------------------------------------------------

    else
    {
        base =
            pow(
                push.color.rgb,
                vec3(2.2)
            );
    }


    // ==================================================
    // SURFACE NORMAL
    // ==================================================

    vec3 surfaceNormal =
        normalize(
            fragNormalWorld
        );


    // Correct normal for back-facing triangles
    if (!gl_FrontFacing)
    {
        surfaceNormal =
            -surfaceNormal;
    }


    // ==================================================
    // CAMERA POSITION
    // ==================================================

    vec3 cameraPosWorld =
        ubo.invView[3].xyz;


    // Direction from fragment to camera
    vec3 viewDirection =
        normalize(
            cameraPosWorld -
            fragPosWorld
        );


    // ==================================================
    // DIRECTIONAL LIGHT
    // ==================================================

    vec3 lightDirection =
        normalize(
            -ubo.sunlight.direction
        );


    vec3 lightIntensity =
        ubo.sunlight.color.xyz *
        ubo.sunlight.color.w;


    // ==================================================
    // AMBIENT LIGHT
    // ==================================================

    vec3 ambientLight =
        ubo.ambientLightColor.xyz *
        ubo.ambientLightColor.w;


    // ==================================================
    // DIFFUSE LIGHT
    // ==================================================

    float cosAngIncidence =
        max(
            dot(
                surfaceNormal,
                lightDirection
            ),
            0.0
        );


    vec3 diffuseLight =
        ambientLight +
        lightIntensity *
        cosAngIncidence;


    // ==================================================
    // BLINN-PHONG SPECULAR
    // ==================================================

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
        pow(
            blinnTerm,
            32.0
        );


    vec3 specularLight =
        lightIntensity *
        blinnTerm *
        0.15;


    // ==================================================
    // FINAL COLOR
    // ==================================================

    vec3 finalColor;


    // --------------------------------------------------
    // GROUND
    // --------------------------------------------------

    if (push.color.a < 0.5)
    {
        // IMPORTANT:
        //
        // Do NOT multiply the checkerboard by
        // directional lighting.
        //
        // Otherwise the horizontal ground can
        // become completely black depending on
        // the sunlight direction.

        finalColor = base;
    }


    // --------------------------------------------------
    // NORMAL OBJECTS
    // --------------------------------------------------

    else
    {
        vec3 diffuseColor =
            diffuseLight *
            base;


        finalColor =
            diffuseColor +
            specularLight;
    }


    // ==================================================
    // OUTPUT
    // ==================================================

    outColor =
        vec4(
            finalColor,
            1.0
        );
}