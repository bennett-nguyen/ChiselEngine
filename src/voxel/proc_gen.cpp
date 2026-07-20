#include "proc_gen.hpp"

TerrainNoise::TerrainNoise(int seed) : global_seed(seed) {
    node_simplex = FastNoise::New<FastNoise::Simplex>();
    node_fractal = FastNoise::New<FastNoise::FractalFBm>();
    node_multiply = FastNoise::New<FastNoise::Multiply>();
    node_pow_float = FastNoise::New<FastNoise::PowFloat>();
    node_remap = FastNoise::New<FastNoise::Remap>();
    node_generator_cache = FastNoise::New<FastNoise::GeneratorCache>();

    node_fractal->SetSource(node_simplex);
    node_multiply->SetLHS(node_fractal);
    // node_pow_float->SetValue(node_multiply);
    node_remap->SetSource(node_multiply);
    node_generator_cache->SetSource(node_remap);

    node_remap->SetFromMin(-1.0f);
    node_remap->SetFromMax(1.0f);
    node_remap->SetToMin(-1.0f);
    node_remap->SetToMax(1.0f);

    node_remap->SetClampOutput(true);
}

void TerrainNoise::getHeightMap(std::array<float, chisel::ChunkDataConstants::CHUNK_AREA>& height_map, const ChunkPosition chunk, const Biome biome) {
    using chisel::ChunkDataConstants::CHUNK_AREA;
    using chisel::ChunkDataConstants::CHUNK_SIZE;

    WorldPosition world = Conversion::chunkToWorld(chunk);

    node_simplex->SetScale(200.0f);

    node_fractal->SetGain(0.54f);
    node_fractal->SetWeightedStrength(0.52f);
    node_fractal->SetOctaveCount(4);
    node_fractal->SetLacunarity(2.32f);

    node_multiply->SetRHS(0.5f);
    // node_pow_float->SetPow(1.0f);

    auto [min, max] = node_remap->GenUniformGrid2D(
        height_map.data(),
        world.x, world.z,
        CHUNK_SIZE, CHUNK_SIZE,
        1.0f, 1.0f, global_seed);
}
