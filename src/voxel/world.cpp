#include "world.hpp"

// void initWorld(World &world) {
//     glCreateVertexArrays(1, &world.empty_VAO);
// }

void initChunkShader(World &world, std::string vshader_path, std::string fshader_path) {
    const ShaderID vshader = makeShader(vshader_path.c_str(), GL_VERTEX_SHADER);
    const ShaderID fshader = makeShader(fshader_path.c_str(), GL_FRAGMENT_SHADER);
    world.chunk_shader_program = glCreateProgram();
    glAttachShader(world.chunk_shader_program, vshader);
    glAttachShader(world.chunk_shader_program, fshader);
    glLinkProgram(world.chunk_shader_program);
    programLinkingCheck(world.chunk_shader_program);
    glDeleteShader(vshader);
    glDeleteShader(fshader);
}

Chunk* getChunkPointer(World &world, const glm::ivec3 chunk_position) {
    if (!isChunkExist(world, chunk_position)) {
        return nullptr;
    }

    return world.chunk_map[chunk_position];
}

bool isChunkExist(const World &world, const glm::ivec3 chunk_position) {
    return world.chunk_map.count(chunk_position) != 0;
}

void buildChunk(World &world, const glm::ivec3 chunk_position) {
    if (!isChunkExist(world, chunk_position)) return;
    const glm::ivec3 north_position = chunk_position + glm::ivec3(1, 0, 0);
    const glm::ivec3 south_position = chunk_position - glm::ivec3(1, 0, 0);
    const glm::ivec3 east_position  = chunk_position + glm::ivec3(0, 0, 1);
    const glm::ivec3 west_position  = chunk_position - glm::ivec3(0, 0, 1);

    Chunk* current_chunk  = getChunkPointer(world, chunk_position);
    Chunk* north_neighbor = getChunkPointer(world, north_position);
    Chunk* south_neighbor = getChunkPointer(world, south_position);
    Chunk* east_neighbor  = getChunkPointer(world, east_position);
    Chunk* west_neighbor  = getChunkPointer(world, west_position);

    buildMesh(current_chunk, north_neighbor, south_neighbor, east_neighbor, west_neighbor);
}

void rebuildChunk(World &world, const glm::ivec3 chunk_position) {
    if (!isChunkExist(world, chunk_position)) return;
    Chunk* ptr_chunk = getChunkPointer(world, chunk_position);
    destroyMesh(ptr_chunk);
    buildChunk(world, chunk_position);
}

void loadChunks(World &world) {
    const glm::ivec3 player_chunk_coords = Conversion::toChunk(world.camera.position);
    std::vector<glm::ivec3> chunks_to_load;

    const int X_MIN = -static_cast<int>(Constant::LOAD_DISTANCE) + player_chunk_coords.x;
    const int X_MAX =  static_cast<int>(Constant::LOAD_DISTANCE) + player_chunk_coords.x;
    const int Z_MIN = -static_cast<int>(Constant::LOAD_DISTANCE) + player_chunk_coords.z;
    const int Z_MAX =  static_cast<int>(Constant::LOAD_DISTANCE) + player_chunk_coords.z;

    for (int x = X_MIN; x <= X_MAX; x++) {
        for (int z = Z_MIN; z <= Z_MAX; z++) {
            glm::ivec3 chunk_position(x, 0, z);
            if (isChunkExist(world, chunk_position)) continue;

            chunks_to_load.push_back(chunk_position);
            auto* ptr_chunk = new Chunk(chunk_position);
            buildVoxels(ptr_chunk);
            world.chunk_map[chunk_position] = ptr_chunk;
        }
    }

    for (auto const &position : chunks_to_load) {
        buildChunk(world, position);
    }
}

void removeChunks(World &world) {
    const auto player_chunk_coords = Conversion::toChunk(world.camera.position);
    std::vector<glm::ivec3> chunks_to_remove;

    const int X_MIN = -static_cast<int>(Constant::LOAD_DISTANCE) + player_chunk_coords.x;
    const int X_MAX =  static_cast<int>(Constant::LOAD_DISTANCE) + player_chunk_coords.x;
    const int Z_MIN = -static_cast<int>(Constant::LOAD_DISTANCE) + player_chunk_coords.z;
    const int Z_MAX =  static_cast<int>(Constant::LOAD_DISTANCE) + player_chunk_coords.z;

    for (const auto &[chunk_position, ptr_chunk] : world.chunk_map) {
        if (X_MIN <= chunk_position.x && chunk_position.x <= X_MAX &&
                Z_MIN <= chunk_position.z && chunk_position.z <= Z_MAX) continue;

        chunks_to_remove.push_back(chunk_position);
    }

    for (auto const &chunk_position : chunks_to_remove) {
        Chunk* ptr_chunk = getChunkPointer(world, chunk_position);
        destroyChunk(ptr_chunk);
        delete ptr_chunk;
        world.chunk_map.erase(chunk_position);
    }
}

void rebuildChunks(World &world) {
    const auto current_player_chunk_pos = Conversion::toChunk(world.camera.position);
    std::unordered_set<glm::ivec3> chunks_to_rebuild;

    const int old_x_min = world.prev_player_chunk_pos.x - static_cast<int>(Constant::LOAD_DISTANCE);
    const int old_x_max = world.prev_player_chunk_pos.x + static_cast<int>(Constant::LOAD_DISTANCE);
    const int old_z_min = world.prev_player_chunk_pos.z - static_cast<int>(Constant::LOAD_DISTANCE);
    const int old_z_max = world.prev_player_chunk_pos.z + static_cast<int>(Constant::LOAD_DISTANCE);

    const int new_x_min = current_player_chunk_pos.x - static_cast<int>(Constant::LOAD_DISTANCE);
    const int new_x_max = current_player_chunk_pos.x + static_cast<int>(Constant::LOAD_DISTANCE);
    const int new_z_min = current_player_chunk_pos.z - static_cast<int>(Constant::LOAD_DISTANCE);
    const int new_z_max = current_player_chunk_pos.z + static_cast<int>(Constant::LOAD_DISTANCE);

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

    for (auto const &position : chunks_to_rebuild) {
        rebuildChunk(world, position);
    }
}

void renderWorld(const World &world) {
    activateShaderProgram(world.chunk_shader_program);
    uniformMat4f(world.chunk_shader_program, "view", 1, GL_FALSE, world.camera.view_mat);
    uniformMat4f(world.chunk_shader_program, "projection", 1, GL_FALSE, world.camera.projection_mat);

    for (const auto &[_, ptr_chunk] : world.chunk_map) {
        uniformMat4f(world.chunk_shader_program, "model", 1, GL_FALSE, getChunkModel(ptr_chunk));
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ptr_chunk->mesh.ssbo_vertices);
        renderChunk(ptr_chunk);
    }

    glBindVertexArray(0);
}

void destroyWorld(World &world) {
    for (auto &[_, ptr_chunk] : world.chunk_map) {
        destroyChunk(ptr_chunk);
        delete ptr_chunk;
    }

    world.chunk_map.clear();
}

void breakBlock(World &world, const glm::ivec3 voxel_position) {
    const auto chunk_position = Conversion::toChunk(voxel_position);
    const auto local_position = Conversion::toLocal(voxel_position, chunk_position);
    const auto voxel_index = Conversion::toIndex(local_position);

    const Chunk* ptr_chunk = getChunkPointer(world, chunk_position);
    ptr_chunk->ptr_voxels[voxel_index] = 0;
    rebuildChunk(world, chunk_position);

    if (0 == local_position.x) {
        rebuildChunk(world, chunk_position - glm::ivec3(1, 0, 0));
    } else if (Constant::CHUNK_SIZE-1 == local_position.x) {
        rebuildChunk(world, chunk_position + glm::ivec3(1, 0, 0));
    }

    if (0 == local_position.y) {
        rebuildChunk(world, chunk_position - glm::ivec3(0, 1, 0));
    } else if (Constant::CHUNK_HEIGHT-1 == local_position.y) {
        rebuildChunk(world, chunk_position + glm::ivec3(0, 1, 0));
    }

    if (0 == local_position.z) {
        rebuildChunk(world, chunk_position - glm::ivec3(0, 0, 1));
    } else if (Constant::CHUNK_SIZE-1 == local_position.z) {
        rebuildChunk(world, chunk_position + glm::ivec3(0, 0, 1));
    }
}

void placeBlock(World &world, const glm::ivec3 adjacent_voxel) {
    const auto chunk_position = Conversion::toChunk(adjacent_voxel);
    const auto local_position = Conversion::toLocal(adjacent_voxel, chunk_position);
    const auto voxel_index = Conversion::toIndex(local_position);
    const Chunk* ptr_chunk = getChunkPointer(world, chunk_position);

    if (chunk_position.y != 0) return;
    ptr_chunk->ptr_voxels[voxel_index] = 1;
    rebuildChunk(world, chunk_position);

    if (0 == local_position.x) {
        rebuildChunk(world, chunk_position - glm::ivec3(1, 0, 0));
    } else if (Constant::CHUNK_SIZE-1 == local_position.x) {
        rebuildChunk(world, chunk_position + glm::ivec3(1, 0, 0));
    }

    if (0 == local_position.y) {
        rebuildChunk(world, chunk_position - glm::ivec3(0, 1, 0));
    } else if (Constant::CHUNK_HEIGHT-1 == local_position.y) {
        rebuildChunk(world, chunk_position + glm::ivec3(0, 1, 0));
    }

    if (0 == local_position.z) {
        rebuildChunk(world, chunk_position - glm::ivec3(0, 0, 1));
    } else if (Constant::CHUNK_SIZE-1 == local_position.z) {
        rebuildChunk(world, chunk_position + glm::ivec3(0, 0, 1));
    }
}