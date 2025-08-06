#include "proc_gen.hpp"

float noise(const glm::vec3 any_position, const Biome biome) {
    auto const PARAMETERS = BIOMES_PARAMETERS.at(biome);

    const float OCTAVES          = PARAMETERS.octaves;
    const float FREQUENCY        = PARAMETERS.frequency;
    const float INITIAL_GAIN     = PARAMETERS.initial_gain;
    const float DELTA_GAIN       = PARAMETERS.delta_gain;
    const float FUDGE_FACTOR     = PARAMETERS.fudge_factor;
    const float ELEVATION_FACTOR = PARAMETERS.elevation_factor;

    float noise = 0.0f;
    float amplitude = 1.0f;
    float amplitude_sum = 0.0f;
    float gain = 0.0f;
    float frequency = FREQUENCY;

    for (int i = 0; i < OCTAVES; i++) {
        noise += amplitude * ((glm::simplex(frequency * any_position) + 1.0f) * 0.5f);

        amplitude_sum += amplitude;
        gain = INITIAL_GAIN + i * DELTA_GAIN;
        frequency *= 2.0f;
        amplitude = 1.0f / gain;
    }

    noise /= amplitude_sum;
    noise = std::pow(noise * FUDGE_FACTOR, ELEVATION_FACTOR);

    return noise;
}

unsigned heightMap(const glm::vec3 any_position) {
    constexpr float CHUNK_HEIGHT_RANGE = static_cast<float>(Constant::CHUNK_HEIGHT);
    const unsigned height = static_cast<unsigned>(noise(any_position, Biome::Test) * CHUNK_HEIGHT_RANGE);
    return height;
}
