#include "world.hpp"

World::World(Camera *p_camera, ShaderProgram *p_chunk_shader)
    : m_pcamera(p_camera), m_pchunk_shader(p_chunk_shader) {}

World::~World() {
    for (auto &chunk : m_chunk_map) {
        delete chunk.second;
    }

    m_chunk_map.clear();
}

glm::ivec3 World::get_chunk_coords_from_camera() {
    glm::vec3 player_pos = m_pcamera->get_camera_position();

    return glm::ivec3(
        int(player_pos.x / (float)Constant::CHUNK_SIZE),
        int(player_pos.y / (float)Constant::CHUNK_HEIGHT),
        int(player_pos.z / (float)Constant::CHUNK_SIZE)
    );
}

void World::render() {
    for (auto &chunk : m_chunk_map) {
        chunk.second->render(m_pchunk_shader, m_pcamera->get_view_mat(), m_pcamera->get_projection_mat());
    }
}

void World::load_chunks() {
    glm::ivec3 player_chunk_coords = get_chunk_coords_from_camera();
    std::vector<glm::ivec3> chunks_to_load;

    for (int x = -(int)Constant::LOAD_DISTANCE; x <= (int)Constant::LOAD_DISTANCE; x++) {
        for (int z = -(int)Constant::LOAD_DISTANCE; z <= (int)Constant::LOAD_DISTANCE; z++) {
            glm::ivec3 chunk_coord(x + player_chunk_coords.x, 0, z + player_chunk_coords.z);
            if (1 == m_chunk_map.count(chunk_coord)) continue;
            chunks_to_load.push_back(chunk_coord);
        }
    }

    for (auto const &coord : chunks_to_load) {
        Chunk *pchunk = new Chunk(coord);
        pchunk->load();
        m_chunk_map[coord] = pchunk;
    }
}

void World::remove_chunks() {
    glm::ivec3 player_chunk_coords = get_chunk_coords_from_camera();
    std::vector<glm::ivec3> chunks_to_remove;

    int x_min = -(int)Constant::LOAD_DISTANCE + player_chunk_coords.x;
    int x_max = (int)Constant::LOAD_DISTANCE + player_chunk_coords.x;
    int z_min = -(int)Constant::LOAD_DISTANCE + player_chunk_coords.z;
    int z_max = (int)Constant::LOAD_DISTANCE + player_chunk_coords.z;

    for (auto const &chunk : m_chunk_map) {
        glm::ivec3 chunk_coord = chunk.first;
        if (x_min <= chunk_coord.x && chunk_coord.x <= x_max && 
                z_min <= chunk_coord.z && chunk_coord.z <= z_max) continue;
        
        chunks_to_remove.push_back(chunk_coord);
    }

    for (auto const &chunk_coord : chunks_to_remove) {
        delete m_chunk_map[chunk_coord];
        m_chunk_map.erase(chunk_coord);
    }
}