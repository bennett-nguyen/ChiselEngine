#include "chunk.hpp"
#include <random>

int heightMap(unsigned num_iterations, float x, float z, float persistence, float scale, unsigned low, unsigned high) {
    float max_amp = 0;
    float amp = 1;
    float freq = scale;
    float noise = 0;

    for(unsigned i = 0; i < num_iterations; ++i) {
        noise += glm::simplex(glm::vec2(x, z) * freq) * amp;
        max_amp += amp;
        amp *= persistence;
        freq *= 2;
    }

    noise /= max_amp;
    noise = noise * (high - low) / 2 + (high + low) / 2;
    return (int)noise;
}

Chunk::Chunk(glm::ivec3 chunk_coord) {
    m_chunk_coord = chunk_coord;
}

Chunk::~Chunk() {
    unload();
}

bool Chunk::isAirAt(unsigned voxel_idx) {
    return VoxelID::Air == m_pvoxels[voxel_idx];
}

void Chunk::setVoxelID(unsigned voxel_idx, unsigned value) {
    m_pvoxels[voxel_idx] = value;
}

void Chunk::unload() {
    m_is_empty = true;
    delete[] m_pvoxels;
    destroyMesh();
}

void Chunk::buildVoxels() {
    m_pvoxels = new unsigned[Constant::CHUNK_VOLUME];

    for (unsigned i = 0; i < Constant::CHUNK_VOLUME; i++) {
        m_pvoxels[i] = 0;
    }

    int min = 1;
    int max = 1000;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);
    unsigned voxel_id = distrib(gen);

    for (unsigned x = 0; x < Constant::CHUNK_SIZE; x++) {
        for (unsigned z = 0; z < Constant::CHUNK_SIZE; z++) {
            unsigned y_level = heightMap(16, float((int)x + m_chunk_coord.x * (int)Constant::CHUNK_SIZE),
                float((int)z + m_chunk_coord.z * (int)Constant::CHUNK_SIZE), 0.6f, 0.01f, 0, Constant::CHUNK_HEIGHT);

            for (unsigned y = 0; y <= y_level; y++) {
                m_pvoxels[VoxelMath::getVoxelIndex(x, y, z)] = voxel_id;
                m_is_empty = false;
            }
        }
    }
}

void Chunk::buildMesh(unsigned *north_neighbor, unsigned *south_neighbor,
        unsigned *east_neighbor, unsigned *west_neighbor) {

    m_mesh.buildChunkMesh(m_pvoxels, north_neighbor, south_neighbor,
        east_neighbor, west_neighbor);
}

void Chunk::destroyMesh() {
    m_mesh.destroyChunkMesh();
}

void Chunk::render() {
    if (m_is_empty) return;
    m_mesh.render();
}

glm::mat4 Chunk::getChunkModel() {
    glm::mat4 model(1.0f);
    glm::vec3 chunk_to_voxel_coord = VoxelMath::getVoxelWorldCoordsFromChunkCoords(m_chunk_coord);
    model = glm::translate(model, glm::vec3(chunk_to_voxel_coord));
    return model;
}

unsigned *Chunk::getVoxelsPointer() {
    return m_pvoxels;
}
