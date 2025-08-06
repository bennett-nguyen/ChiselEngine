#ifndef PROC_GEN_HPP
#define PROC_GEN_HPP

#include <unordered_map>

#include <glm/gtc/noise.hpp>

#include "constant.hpp"
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
};

const std::unordered_map<Biome, NoiseParameters> BIOMES_PARAMETERS {
    {
        Biome::Test, {
            .octaves = 4,
            .frequency = 0.5f,
            .initial_gain = 2.0f,
            .delta_gain = 1.0f,
            .fudge_factor = 0.87f,
            .elevation_factor = 3.57f
        }
    },

    {
        Biome::Plains, {
            .octaves = 4,
            .frequency = 0.2f,
            .initial_gain = 2.0f,
            .delta_gain = 2.0f,
            .fudge_factor = 0.8f,
            .elevation_factor = 2.57f
        }
    }
};

[[nodiscard]] float noise(glm::vec3 any_position, Biome biome);
[[nodiscard]] unsigned heightMap(glm::vec3 any_position);
[[nodiscard]] unsigned moistureMap(glm::vec3 any_position);

#endif
