#pragma ones

namespace engine
{
    struct TerrainConfig
    {
        int gridSize = 128;         // vertices per side
        float vertexSpacing = 1.0f; // world units between grid points
        float heightScale = 20.0f;
        float noiseFrequency = 0.02f;
        int octaves = 4;
        float lacunarity = 2.0f; // frequency multiplier per octave
        float gain = 0.5f;       // amplitude multiplier per octave
        int seed = 1337;         // noise seed
    };
}