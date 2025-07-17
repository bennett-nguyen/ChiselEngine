#include "world.hpp"

void initChunkShader(World &world, const std::string &vshader_path, const std::string &fshader_path) {
    world.chunk_shader_program = glCreateProgram();
    attachShader(vshader_path, world.chunk_shader_program);
    attachShader(fshader_path, world.chunk_shader_program);
    linkProgram(world.chunk_shader_program);
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
    const Chunk* north_neighbor = getChunkPointer(world, north_position);
    const Chunk* south_neighbor = getChunkPointer(world, south_position);
    const Chunk* east_neighbor  = getChunkPointer(world, east_position);
    const Chunk* west_neighbor  = getChunkPointer(world, west_position);

    current_chunk->buildMesh(north_neighbor, south_neighbor, east_neighbor, west_neighbor);
}

void rebuildChunk(World &world, const glm::ivec3 chunk_position) {
    if (!isChunkExist(world, chunk_position)) return;
    Chunk* ptr_chunk = getChunkPointer(world, chunk_position);
    ptr_chunk->destroyMesh();
    buildChunk(world, chunk_position);
}

void loadChunks(World &world) {
    const ChunkPosition player_position = Conversion::toChunk(world.camera.position);
    std::vector<glm::ivec3> chunks_to_load;

    const int X_MIN = -static_cast<int>(Constant::LOAD_DISTANCE) + player_position.x;
    const int X_MAX =  static_cast<int>(Constant::LOAD_DISTANCE) + player_position.x;
    const int Z_MIN = -static_cast<int>(Constant::LOAD_DISTANCE) + player_position.z;
    const int Z_MAX =  static_cast<int>(Constant::LOAD_DISTANCE) + player_position.z;

    for (int x = X_MIN; x <= X_MAX; x++) {
        for (int z = Z_MIN; z <= Z_MAX; z++) {
            glm::ivec3 chunk_position(x, 0, z);
            if (isChunkExist(world, chunk_position)) continue;

            chunks_to_load.push_back(chunk_position);
            auto* ptr_chunk = new Chunk(chunk_position);
            ptr_chunk->buildVoxels();
            world.chunk_map[chunk_position] = ptr_chunk;
        }
    }

    for (auto const &position : chunks_to_load) {
        buildChunk(world, position);
    }
}

void removeChunks(World &world) {
    const auto player_position = Conversion::toChunk(world.camera.position);
    std::vector<glm::ivec3> chunks_to_remove;

    const int X_MIN = -static_cast<int>(Constant::LOAD_DISTANCE) + player_position.x;
    const int X_MAX =  static_cast<int>(Constant::LOAD_DISTANCE) + player_position.x;
    const int Z_MIN = -static_cast<int>(Constant::LOAD_DISTANCE) + player_position.z;
    const int Z_MAX =  static_cast<int>(Constant::LOAD_DISTANCE) + player_position.z;

    for (const auto &[chunk_position, ptr_chunk] : world.chunk_map) {
        if (X_MIN <= chunk_position.x && chunk_position.x <= X_MAX &&
                Z_MIN <= chunk_position.z && chunk_position.z <= Z_MAX) continue;

        chunks_to_remove.push_back(chunk_position);
    }

    for (auto const &chunk_position : chunks_to_remove) {
        const Chunk* ptr_chunk = getChunkPointer(world, chunk_position);
        delete ptr_chunk;
        world.chunk_map.erase(chunk_position);
    }
}

void rebuildChunks(World &world) {
    const auto current_player_position = Conversion::toChunk(world.camera.position);
    std::unordered_set<glm::ivec3> chunks_to_rebuild;

    const int old_x_min = world.prev_player_chunk_pos.x - static_cast<int>(Constant::LOAD_DISTANCE);
    const int old_x_max = world.prev_player_chunk_pos.x + static_cast<int>(Constant::LOAD_DISTANCE);
    const int old_z_min = world.prev_player_chunk_pos.z - static_cast<int>(Constant::LOAD_DISTANCE);
    const int old_z_max = world.prev_player_chunk_pos.z + static_cast<int>(Constant::LOAD_DISTANCE);

    const int new_x_min = current_player_position.x - static_cast<int>(Constant::LOAD_DISTANCE);
    const int new_x_max = current_player_position.x + static_cast<int>(Constant::LOAD_DISTANCE);
    const int new_z_min = current_player_position.z - static_cast<int>(Constant::LOAD_DISTANCE);
    const int new_z_max = current_player_position.z + static_cast<int>(Constant::LOAD_DISTANCE);

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

void destroyWorld(World &world) {
    for (auto &[_, ptr_chunk] : world.chunk_map) {
        delete ptr_chunk;
    }

    world.chunk_map.clear();
}

void breakBlock(World &world, const WorldPosition voxel_position) {
    const auto chunk_position = Conversion::toChunk(voxel_position);
    const auto local_position = Conversion::toLocal(voxel_position, chunk_position);

    const Chunk* ptr_chunk = getChunkPointer(world, chunk_position);
    ptr_chunk->setVoxelIDAtPosition(0, local_position);
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

void placeBlock(World &world, const WorldPosition adjacent_voxel) {
    const auto chunk_position = Conversion::toChunk(adjacent_voxel);
    const auto local_position = Conversion::toLocal(adjacent_voxel, chunk_position);

    const Chunk* ptr_chunk = getChunkPointer(world, chunk_position);
    if (chunk_position.y != 0) return;

    ptr_chunk->setVoxelIDAtPosition(1, local_position);
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