#include "chunk.hpp"

unsigned heightMap(unsigned num_iterations, float x, float z, float persistence, float scale, unsigned low, unsigned high) {
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
    noise = noise * ((float)high - (float)low) / 2 + ((float)high + (float)low) / 2;
    return (unsigned)noise;
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

    unsigned sand_level = 10; 
    unsigned dirt_level = 20; 
    unsigned grass_level = 30; 
    unsigned cobblestone_level = 40; 
    unsigned stone_level = 50; 

    // std::random_device rd;
    // std::mt19937 gen(rd());
    // std::uniform_int_distribution<> distrib(min, max);
    // unsigned voxel_id = (unsigned)distrib(gen);
    
    unsigned voxel_id;
    for (unsigned x = 0; x < Constant::CHUNK_SIZE; x++) {
        for (unsigned z = 0; z < Constant::CHUNK_SIZE; z++) {
            unsigned y_level = heightMap(6, float((int)x + m_chunk_coord.x * (int)Constant::CHUNK_SIZE),
                float((int)z + m_chunk_coord.z * (int)Constant::CHUNK_SIZE), 0.6f, 0.007f, 0, Constant::CHUNK_HEIGHT);

            for (unsigned y = 0; y <= y_level; y++) {
                if (0 <= y && y <= sand_level) {
                    voxel_id = VoxelID::Sand;
                } else if (sand_level < y && y <= dirt_level) {
                    voxel_id = VoxelID::Dirt;
                } else if (dirt_level < y && y <= grass_level) {
                    voxel_id = VoxelID::Grass;
                } else if (grass_level < y && y <= cobblestone_level) {
                    voxel_id = VoxelID::CobbleStone;
                } else if (stone_level < y && y <= stone_level) {
                    voxel_id = VoxelID::Stone;
                }
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
