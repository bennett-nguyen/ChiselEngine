#ifndef PROC_GEN_HPP
#define PROC_GEN_HPP

#include <unordered_map>
#include <iostream>

#include <FastNoise/FastNoise.h>

#include "gl_constants.hpp"
#include "conversions.hpp"

enum class Biome : unsigned {
    Test,
    Plains
};

struct NoiseParameters {
    int octaves;
    float frequency;
    float initial_gain;
    float delta_gain;
    float fudge_factor;
    float elevation_factor;
    float fractal_lucunarity;
    float fractal_gain;
};

const std::unordered_map<Biome, NoiseParameters> BIOMES_PARAMETERS {
        {
            Biome::Test, {
                .octaves = 4,
                .frequency = 0.5f,
                .initial_gain = 2.0f,
                .delta_gain = 1.0f,
                .fudge_factor = 0.87f,
                .elevation_factor = 3.57f,
                .fractal_lucunarity = 2.3f,
                .fractal_gain = 0.5f
            }
        },

        {
            Biome::Plains, {
                .octaves = 3,
                .frequency = 0.003f,
                .initial_gain = 2.0f,
                .delta_gain = 2.0f,
                .fudge_factor = 0.2f,
                .elevation_factor = 2.57f,
                .fractal_lucunarity = 1.5f,
                .fractal_gain = 0.34f
            }
        }
};

class TerrainNoise {
    FastNoise::SmartNode<FastNoise::Simplex> node_simplex;
    FastNoise::SmartNode<FastNoise::FractalFBm> node_fractal;
    FastNoise::SmartNode<FastNoise::PowFloat> node_pow_float;
    FastNoise::SmartNode<FastNoise::Multiply> node_multiply;
    FastNoise::SmartNode<FastNoise::Remap> node_remap;
    FastNoise::SmartNode<FastNoise::GeneratorCache> node_generator_cache;

    int global_seed;

public:
    explicit TerrainNoise(int seed);

    [[nodiscard]] float getNoise(glm::vec3 any_position, Biome biome);
    void getHeightMap(std::array<float, chisel::ChunkDataConstants::CHUNK_AREA>& height_map, const ChunkPosition chunk, const Biome biome);
};

[[nodiscard]] unsigned moistureMap(glm::vec3 any_position);

inline TerrainNoise terrain_noise_engine(1);

#endif
