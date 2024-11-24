#include "chunk.hpp"

int height_map(unsigned num_iterations, float x, float z, float persistence, float scale, unsigned low, unsigned high) {
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

void Chunk::load() {
    build_voxels();
    build_mesh();
}

void Chunk::unload() {
    m_is_empty = true;
    delete[] m_pvoxels;
    destroy_mesh();
}

void Chunk::build_voxels() {
    m_pvoxels = new unsigned[Constant::CHUNK_VOLUME];
    for (int i = 0; i < Constant::CHUNK_VOLUME; i++) {
        m_pvoxels[i] = 0;
    }

    for (int x = 0; x < (int)Constant::CHUNK_SIZE; x++) {
        for (int z = 0; z < (int)Constant::CHUNK_SIZE; z++) {
            int y_level = height_map(16, float(x + m_chunk_coord.x * (int)Constant::CHUNK_SIZE),
                float(z + m_chunk_coord.z * (int)Constant::CHUNK_SIZE), 0.5f, 0.007f, 0, Constant::CHUNK_HEIGHT);

            for (int y = 0; y <= y_level; y++) {
                m_pvoxels[get_voxel_idx(x, y, z)] = 1;
                m_is_empty = false;
            }
        }
    }
}

void Chunk::build_mesh() {
    m_mesh.build_chunk_mesh(m_pvoxels);
}

void Chunk::destroy_mesh() {
    m_mesh.destroy_chunk_mesh();
}

void Chunk::render() {
    if (m_is_empty) return;
    m_mesh.render();
}

glm::mat4 Chunk::get_chunk_model() {
    glm::mat4 model(1.0f);

    glm::vec3 chunk_to_voxel_coord(
        m_chunk_coord.x * (int)Constant::CHUNK_SIZE,
        m_chunk_coord.y * (int)Constant::CHUNK_HEIGHT,
        m_chunk_coord.z * (int)Constant::CHUNK_SIZE
    );

    model = glm::translate(model, chunk_to_voxel_coord);
    return model;
}