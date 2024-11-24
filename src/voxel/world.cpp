#include "world.hpp"

World::World(Camera *p_camera, ShaderProgram *p_chunk_shader)
        : m_pcamera(p_camera), m_pchunk_shader(p_chunk_shader) {
    load_chunks();
    m_prev_player_chunk_pos = get_chunk_coords_from_camera();
}

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

void World::update() {
    glm::ivec3 current_player_chunk_pos = get_chunk_coords_from_camera();

    if (m_prev_player_chunk_pos.x != current_player_chunk_pos.x
            || m_prev_player_chunk_pos.z != current_player_chunk_pos.z) {
        remove_chunks();
        load_chunks();
        rebuild_chunks();

        m_prev_player_chunk_pos = current_player_chunk_pos;
    }

    render();
}

void World::render() {
    m_pchunk_shader->activate();
    m_pchunk_shader->uniform_mat4f("view", 1, GL_FALSE, m_pcamera->get_view_mat());
    m_pchunk_shader->uniform_mat4f("projection", 1, GL_FALSE, m_pcamera->get_projection_mat());

    for (auto &chunk : m_chunk_map) {
        m_pchunk_shader->uniform_mat4f("model", 1, GL_FALSE, chunk.second->get_chunk_model());
        chunk.second->render();
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
            Chunk *pchunk = new Chunk(chunk_coord);
            pchunk->build_voxels();
            m_chunk_map[chunk_coord] = pchunk;
        }
    }

    for (auto const &coord : chunks_to_load) {
        glm::ivec3 north_coord(coord.x+1, coord.y, coord.z);
        glm::ivec3 south_coord(coord.x-1, coord.y, coord.z);
        glm::ivec3 east_coord(coord.x, coord.y, coord.z+1);
        glm::ivec3 west_coord(coord.x, coord.y, coord.z-1);

        unsigned *north_neighbor_pvoxels = get_chunk_neighbor_pvoxels(north_coord);
        unsigned *south_neighbor_pvoxels = get_chunk_neighbor_pvoxels(south_coord);
        unsigned *east_neighbor_pvoxels = get_chunk_neighbor_pvoxels(east_coord);
        unsigned *west_neighbor_pvoxels = get_chunk_neighbor_pvoxels(west_coord);

        m_chunk_map[coord]->build_mesh(north_neighbor_pvoxels, south_neighbor_pvoxels, east_neighbor_pvoxels, west_neighbor_pvoxels);
    }
}

unsigned* World::get_chunk_neighbor_pvoxels(glm::ivec3 coord) {
    unsigned *neighbor_pvoxels = nullptr;
    
    if (1 == m_chunk_map.count(coord)) {
        neighbor_pvoxels = m_chunk_map[coord]->get_pvoxels();
    }

    return neighbor_pvoxels;
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

void World::rebuild_chunks() {
    glm::ivec3 current_player_chunk_pos = get_chunk_coords_from_camera();
    std::unordered_set<glm::ivec3> chunks_to_rebuild;

    int old_x_min = m_prev_player_chunk_pos.x - Constant::LOAD_DISTANCE;
    int old_x_max = m_prev_player_chunk_pos.x + Constant::LOAD_DISTANCE;
    int old_z_min = m_prev_player_chunk_pos.z - Constant::LOAD_DISTANCE;
    int old_z_max = m_prev_player_chunk_pos.z + Constant::LOAD_DISTANCE;

    int new_x_min = current_player_chunk_pos.x - Constant::LOAD_DISTANCE;
    int new_x_max = current_player_chunk_pos.x + Constant::LOAD_DISTANCE;
    int new_z_min = current_player_chunk_pos.z - Constant::LOAD_DISTANCE;
    int new_z_max = current_player_chunk_pos.z + Constant::LOAD_DISTANCE;

    for (int x = new_x_min; x <= new_x_max; x++) {
        for (int z = new_z_min; z <= new_z_max; z++) {
            if (new_x_min < x && x < new_x_max && new_z_min < z && z < new_z_max) continue;
            
            if (old_x_min <= x && x <= old_x_max && old_z_min <= z && z <= old_z_max) {
                chunks_to_rebuild.insert(glm::ivec3(x, 0, z));
            }
        }
    }

    for (int x = old_x_min; x <= old_x_max; x++) {
        for (int z = old_z_min; z <= old_z_max; z++) {
            if (old_x_min < x && x < old_x_max && old_z_min < z && z < old_z_max) continue;
            
            if (new_x_min <= x && x <= new_x_max && new_z_min <= z && z <= new_z_max) {
                chunks_to_rebuild.insert(glm::ivec3(x, 0, z));
            }
        }
    }

    for (auto const &coord : chunks_to_rebuild) {
        glm::ivec3 north_coord(coord.x+1, coord.y, coord.z);
        glm::ivec3 south_coord(coord.x-1, coord.y, coord.z);
        glm::ivec3 east_coord(coord.x, coord.y, coord.z+1);
        glm::ivec3 west_coord(coord.x, coord.y, coord.z-1);

        unsigned *north_neighbor_pvoxels = get_chunk_neighbor_pvoxels(north_coord);
        unsigned *south_neighbor_pvoxels = get_chunk_neighbor_pvoxels(south_coord);
        unsigned *east_neighbor_pvoxels = get_chunk_neighbor_pvoxels(east_coord);
        unsigned *west_neighbor_pvoxels = get_chunk_neighbor_pvoxels(west_coord);

        m_chunk_map[coord]->destroy_mesh();
        m_chunk_map[coord]->build_mesh(north_neighbor_pvoxels, south_neighbor_pvoxels, east_neighbor_pvoxels, west_neighbor_pvoxels);
    }
}