#include "voxel_math.hpp"

unsigned get_voxel_idx(int x, int y, int z) {
    return (unsigned)x + Constant::CHUNK_SIZE * (unsigned)z + Constant::CHUNK_AREA * (unsigned)y;
}

unsigned get_voxel_idx(glm::ivec3 voxel_local_coords) {
    return (unsigned)voxel_local_coords.x + Constant::CHUNK_SIZE
        * (unsigned)voxel_local_coords.z + Constant::CHUNK_AREA * (unsigned)voxel_local_coords.y;
}

glm::ivec3 get_voxel_world_coords(glm::ivec3 voxel_local_coords, glm::ivec3 chunk_coords) {
    return glm::ivec3(
        voxel_local_coords.x + chunk_coords.x * int(Constant::CHUNK_SIZE),
        voxel_local_coords.y + chunk_coords.y * int(Constant::CHUNK_HEIGHT),
        voxel_local_coords.z + chunk_coords.z * int(Constant::CHUNK_SIZE)
    );
}

glm::ivec3 get_voxel_world_coords_from_pos(glm::vec3 pos) {
    return glm::ivec3(
        std::floor(pos.x),
        std::floor(pos.y),
        std::floor(pos.z)
    );
}

glm::ivec3 get_chunk_coords_from_pos(glm::vec3 pos) {
    return glm::ivec3(
        std::floor(pos.x / (float)Constant::CHUNK_SIZE),
        std::floor(pos.y / (float)Constant::CHUNK_HEIGHT),
        std::floor(pos.z / (float)Constant::CHUNK_SIZE)
    );
}

glm::ivec3 get_voxel_world_coords_from_chunk_coords(glm::ivec3 chunk_coords) {
    return glm::ivec3(
        chunk_coords.x * Constant::CHUNK_SIZE,
        chunk_coords.y * Constant::CHUNK_HEIGHT,
        chunk_coords.z * Constant::CHUNK_SIZE
    );
}

glm::ivec3 get_chunk_coords_from_voxel(glm::ivec3 voxel_world_coords) {
    return glm::ivec3(
        std::floor(voxel_world_coords.x / (float)Constant::CHUNK_SIZE),
        std::floor(voxel_world_coords.y / (float)Constant::CHUNK_HEIGHT),
        std::floor(voxel_world_coords.z / (float)Constant::CHUNK_SIZE)
    );
}

glm::ivec3 get_local_voxel_coords(glm::ivec3 voxel_world_coords, glm::ivec3 chunk_coords) {
    return glm::ivec3(
        voxel_world_coords.x - chunk_coords.x * int(Constant::CHUNK_SIZE),
        voxel_world_coords.y - chunk_coords.y * int(Constant::CHUNK_HEIGHT),
        voxel_world_coords.z - chunk_coords.z * int(Constant::CHUNK_SIZE)
    );
}