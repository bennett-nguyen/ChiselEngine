#include "chunk.hpp"

int height_map(float x, float z) {
    float scale = 0.01f;
    float normalized_range = (glm::simplex(glm::vec2(x, z) * scale) + 1.0f) / 2.0f;
    return (int)(normalized_range * Constant::CHUNK_HEIGHT);
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
            int y_level = height_map(float(x + m_chunk_coord.x * (int)Constant::CHUNK_SIZE), float(z + m_chunk_coord.z * (int)Constant::CHUNK_SIZE));
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

void Chunk::render(ShaderProgram *pchunk_shader, glm::mat4 view, glm::mat4 projection) {
    if (m_is_empty) return;
    m_mesh.render(pchunk_shader, view, projection, m_chunk_coord);
}