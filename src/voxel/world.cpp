#include "world.hpp"

World::World(float getWidthHeightRatio)
    : m_voxel_handler(&m_chunk_map) {
    m_chunk_shader = ShaderProgram("resources/shaders/chunk.vert", "resources/shaders/chunk.frag");
    m_cubemesh_shader = ShaderProgram("resources/shaders/cubemesh.vert", "resources/shaders/cubemesh.frag");
    m_player.initCamera(getWidthHeightRatio);
    m_player.setPosition(glm::vec3(0, 70, 0));
    m_prev_player_chunk_pos = VoxelMath::getChunkCoordsFromPos(m_player.getPosition());
    loadChunks();
}

World::~World() {
    for (auto &chunk : m_chunk_map) {
        delete chunk.second;
    }

    m_chunk_map.clear();
}

void World::buildChunk(glm::ivec3 chunk_coords) {
    if (1 != m_chunk_map.count(chunk_coords)) return;
    glm::ivec3 north_coord(chunk_coords.x+1, chunk_coords.y, chunk_coords.z);
    glm::ivec3 south_coord(chunk_coords.x-1, chunk_coords.y, chunk_coords.z);
    glm::ivec3 east_coord(chunk_coords.x, chunk_coords.y, chunk_coords.z+1);
    glm::ivec3 west_coord(chunk_coords.x, chunk_coords.y, chunk_coords.z-1);

    unsigned *north_neighbor_pvoxels = getChunkNeighborVoxelsPointer(north_coord);
    unsigned *south_neighbor_pvoxels = getChunkNeighborVoxelsPointer(south_coord);
    unsigned *east_neighbor_pvoxels = getChunkNeighborVoxelsPointer(east_coord);
    unsigned *west_neighbor_pvoxels = getChunkNeighborVoxelsPointer(west_coord);

    m_chunk_map[chunk_coords]->buildMesh(north_neighbor_pvoxels, south_neighbor_pvoxels, east_neighbor_pvoxels, west_neighbor_pvoxels);
}

void World::rebuildChunk(glm::ivec3 chunk_coords) {
    if (1 != m_chunk_map.count(chunk_coords)) return;
    m_chunk_map[chunk_coords]->destroyMesh();
    buildChunk(chunk_coords);
}

void World::update(float delta_time) {
    Camera *player_camera = &m_player.m_camera;

    m_player.move(delta_time);
    player_camera->updateView();
    glm::ivec3 current_player_chunk_pos = VoxelMath::getChunkCoordsFromPos(m_player.getPosition());

    if (m_prev_player_chunk_pos.x != current_player_chunk_pos.x
            || m_prev_player_chunk_pos.z != current_player_chunk_pos.z) {
        removeChunks();
        loadChunks();
        rebuildChunks();

        m_prev_player_chunk_pos = current_player_chunk_pos;
    }

    m_voxel_handler.rayCast(player_camera->getCameraPosition(), player_camera->getCameraFront());
    
    if (0 == m_block_interaction_mode) {
        breakBlock();
    } else if (1 == m_block_interaction_mode) {
        placeBlock();
    }

    render();

    if (m_is_render_cube_mesh) {
        m_cubemesh_shader.activate();
        m_cubemesh_shader.uniformMat4f("view", 1, GL_FALSE, player_camera->getViewMat());
        m_cubemesh_shader.uniformMat4f("projection", 1, GL_FALSE, player_camera->getProjectionMat());
        m_cubemesh_shader.uniformInt("block_interaction_mode", m_block_interaction_mode);

        if (0 == m_block_interaction_mode) {
            glm::vec3 current_voxel = m_voxel_handler.getDetectedVoxelWorldCoords();
            m_cubemesh_shader.uniformMat4f("model", 1, GL_FALSE, m_cubemesh.getModel(glm::vec3(current_voxel)));
        } else if (1 == m_block_interaction_mode) {
            glm::vec3 current_voxel = m_voxel_handler.getVoxelWorldCoordsNextToDetectedVoxel();
            m_cubemesh_shader.uniformMat4f("model", 1, GL_FALSE, m_cubemesh.getModel(glm::vec3(current_voxel)));
        }

        m_cubemesh.render();
    }

    m_is_block_interaction_enabled = false;
}

void World::breakBlock() {
    m_is_render_cube_mesh = false;

    if (m_voxel_handler.IsDetectedVoxel()) {
        m_is_render_cube_mesh = true;

        if (!m_is_block_interaction_enabled) return;

        m_is_render_cube_mesh = false;
        glm::ivec3 detected_voxel_coords = m_voxel_handler.getDetectedVoxelWorldCoords();
        glm::ivec3 chunk_coords = VoxelMath::getChunkCoordsFromVoxel(detected_voxel_coords);
        glm::uvec3 voxel_local_coords = VoxelMath::getLocalVoxelCoords(detected_voxel_coords, chunk_coords);
        unsigned voxel_idx = VoxelMath::getVoxelIndex(voxel_local_coords);

        m_chunk_map[chunk_coords]->setVoxelID(voxel_idx, 0);
        rebuildChunk(chunk_coords);

        if (0 == voxel_local_coords.x) {
            rebuildChunk(glm::ivec3(chunk_coords.x-1, chunk_coords.y, chunk_coords.z));
        } else if (Constant::CHUNK_SIZE-1 == voxel_local_coords.x) {
            rebuildChunk(glm::ivec3(chunk_coords.x+1, chunk_coords.y, chunk_coords.z));
        }

        if (0 == voxel_local_coords.y) {
            rebuildChunk(glm::ivec3(chunk_coords.x, chunk_coords.y-1, chunk_coords.z));
        } else if (Constant::CHUNK_HEIGHT-1 == voxel_local_coords.y) {
            rebuildChunk(glm::ivec3(chunk_coords.x, chunk_coords.y+1, chunk_coords.z));
        }

        if (0 == voxel_local_coords.z) {
            rebuildChunk(glm::ivec3(chunk_coords.x, chunk_coords.y, chunk_coords.z-1));
        } else if (Constant::CHUNK_SIZE-1 == voxel_local_coords.z) {
            rebuildChunk(glm::ivec3(chunk_coords.x, chunk_coords.y, chunk_coords.z+1));
        }
    }
}

void World::placeBlock() {
    m_is_render_cube_mesh = false;

    if (m_voxel_handler.IsDetectedVoxel()) {
        m_is_render_cube_mesh = true;

        if (!m_is_block_interaction_enabled) return;
        glm::ivec3 adjacent_voxel_world_coords = m_voxel_handler.getVoxelWorldCoordsNextToDetectedVoxel();
        glm::ivec3 adjacent_voxel_chunk_coords = VoxelMath::getChunkCoordsFromVoxel(adjacent_voxel_world_coords);
        glm::uvec3 adjacent_voxel_local_coords = VoxelMath::getLocalVoxelCoords(adjacent_voxel_world_coords, adjacent_voxel_chunk_coords);
        unsigned adjacent_voxel_index = VoxelMath::getVoxelIndex(adjacent_voxel_local_coords);

        if (adjacent_voxel_chunk_coords.y >= Constant::CHUNK_HEIGHT || 0 < adjacent_voxel_chunk_coords.y) return;
        m_chunk_map[adjacent_voxel_chunk_coords]->setVoxelID(adjacent_voxel_index, VoxelID::CobbleStone);
        rebuildChunk(adjacent_voxel_chunk_coords);

        if (0 == adjacent_voxel_local_coords.x) {
            rebuildChunk(glm::ivec3(adjacent_voxel_chunk_coords.x-1, adjacent_voxel_chunk_coords.y, adjacent_voxel_chunk_coords.z));
        } else if (Constant::CHUNK_SIZE-1 == adjacent_voxel_local_coords.x) {
            rebuildChunk(glm::ivec3(adjacent_voxel_chunk_coords.x+1, adjacent_voxel_chunk_coords.y, adjacent_voxel_chunk_coords.z));
        }

        if (0 == adjacent_voxel_local_coords.y) {
            rebuildChunk(glm::ivec3(adjacent_voxel_chunk_coords.x, adjacent_voxel_chunk_coords.y-1, adjacent_voxel_chunk_coords.z));
        } else if (Constant::CHUNK_HEIGHT-1 == adjacent_voxel_local_coords.y) {
            rebuildChunk(glm::ivec3(adjacent_voxel_chunk_coords.x, adjacent_voxel_chunk_coords.y+1, adjacent_voxel_chunk_coords.z));
        }

        if (0 == adjacent_voxel_local_coords.z) {
            rebuildChunk(glm::ivec3(adjacent_voxel_chunk_coords.x, adjacent_voxel_chunk_coords.y, adjacent_voxel_chunk_coords.z-1));
        } else if (Constant::CHUNK_SIZE-1 == adjacent_voxel_local_coords.z) {
            rebuildChunk(glm::ivec3(adjacent_voxel_chunk_coords.x, adjacent_voxel_chunk_coords.y, adjacent_voxel_chunk_coords.z+1));
        }
    }
}

void World::render() {
    m_chunk_shader.activate();
    m_chunk_shader.uniformMat4f("view", 1, GL_FALSE, m_player.m_camera.getViewMat());
    m_chunk_shader.uniformMat4f("projection", 1, GL_FALSE, m_player.m_camera.getProjectionMat());

    for (auto &chunk : m_chunk_map) {
        m_chunk_shader.uniformMat4f("model", 1, GL_FALSE, chunk.second->getChunkModel());
        chunk.second->render();
    }
}

void World::loadChunks() {
    glm::ivec3 player_chunk_coords = VoxelMath::getChunkCoordsFromPos(m_player.getPosition());
    std::vector<glm::ivec3> chunks_to_load;

    int x_min = -(int)Constant::LOAD_DISTANCE + player_chunk_coords.x;
    int x_max =  (int)Constant::LOAD_DISTANCE + player_chunk_coords.x;
    int z_min = -(int)Constant::LOAD_DISTANCE + player_chunk_coords.z;
    int z_max =  (int)Constant::LOAD_DISTANCE + player_chunk_coords.z;

    for (int x = x_min; x <= x_max; x++) {
        for (int z = z_min; z <= z_max; z++) {
            glm::ivec3 chunk_coord(x, 0, z);
            if (1 == m_chunk_map.count(chunk_coord)) continue;
            chunks_to_load.push_back(chunk_coord);
            Chunk *pchunk = new Chunk(chunk_coord);
            pchunk->buildVoxels();
            m_chunk_map[chunk_coord] = pchunk;
        }
    }

    for (auto const &coords : chunks_to_load) {
        buildChunk(coords);
    }
}

Player* World::getPlayerPointer() {
    return &m_player;
}

unsigned* World::getChunkNeighborVoxelsPointer(glm::ivec3 coord) {
    unsigned *neighbor_pvoxels = nullptr;

    if (1 == m_chunk_map.count(coord)) {
        neighbor_pvoxels = m_chunk_map[coord]->getVoxelsPointer();
    }

    return neighbor_pvoxels;
}

void World::removeChunks() {
    glm::ivec3 player_chunk_coords = VoxelMath::getChunkCoordsFromPos(m_player.getPosition());
    std::vector<glm::ivec3> chunks_to_remove;

    int x_min = -(int)Constant::LOAD_DISTANCE + player_chunk_coords.x;
    int x_max =  (int)Constant::LOAD_DISTANCE + player_chunk_coords.x;
    int z_min = -(int)Constant::LOAD_DISTANCE + player_chunk_coords.z;
    int z_max =  (int)Constant::LOAD_DISTANCE + player_chunk_coords.z;

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

void World::rebuildChunks() {
    glm::ivec3 current_player_chunk_pos = VoxelMath::getChunkCoordsFromPos(m_player.getPosition());
    std::unordered_set<glm::ivec3> chunks_to_rebuild;

    int old_x_min = m_prev_player_chunk_pos.x - (int)Constant::LOAD_DISTANCE;
    int old_x_max = m_prev_player_chunk_pos.x + (int)Constant::LOAD_DISTANCE;
    int old_z_min = m_prev_player_chunk_pos.z - (int)Constant::LOAD_DISTANCE;
    int old_z_max = m_prev_player_chunk_pos.z + (int)Constant::LOAD_DISTANCE;

    int new_x_min = current_player_chunk_pos.x - (int)Constant::LOAD_DISTANCE;
    int new_x_max = current_player_chunk_pos.x + (int)Constant::LOAD_DISTANCE;
    int new_z_min = current_player_chunk_pos.z - (int)Constant::LOAD_DISTANCE;
    int new_z_max = current_player_chunk_pos.z + (int)Constant::LOAD_DISTANCE;

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

    for (auto const &coords : chunks_to_rebuild) {
        rebuildChunk(coords);
    }
}

void World::pollEvent(const SDL_Event &event, float delta_time) {
    m_player.m_camera.pan(event, delta_time);

    if (SDL_KEYDOWN == event.type) {
        if (SDL_SCANCODE_B == event.key.keysym.scancode) {
            m_is_block_interaction_enabled = true;
        }
    }

    if (SDL_MOUSEBUTTONDOWN == event.type) {
        if (SDL_BUTTON_LEFT == event.button.button) {
            m_is_block_interaction_enabled = true;
        } else if (SDL_BUTTON_RIGHT == event.button.button) {
            m_block_interaction_mode++;
            m_block_interaction_mode %= 2;
        }
    }
}
