#include "voxel_math.hpp"

unsigned VoxelMath::getVoxelIndex(unsigned x, unsigned y, unsigned z) {
    return x + Constant::CHUNK_SIZE * z + Constant::CHUNK_AREA * y;
}

unsigned VoxelMath::getVoxelIndex(glm::uvec3 voxel_local_coords) {
    return voxel_local_coords.x + Constant::CHUNK_SIZE
        * voxel_local_coords.z + Constant::CHUNK_AREA * voxel_local_coords.y;
}

glm::ivec3 VoxelMath::getVoxelWorldCoords(glm::uvec3 voxel_local_coords, glm::ivec3 chunk_coords) {
    return glm::ivec3(
        (int)voxel_local_coords.x + chunk_coords.x * (int)Constant::CHUNK_SIZE,
        (int)voxel_local_coords.y + chunk_coords.y * (int)Constant::CHUNK_HEIGHT,
        (int)voxel_local_coords.z + chunk_coords.z * (int)Constant::CHUNK_SIZE
    );
}

glm::ivec3 VoxelMath::getVoxelWorldCoordsFromPos(glm::vec3 pos) {
    return glm::ivec3(
        (int)std::floor(pos.x),
        (int)std::floor(pos.y),
        (int)std::floor(pos.z)
    );
}

glm::ivec3 VoxelMath::getChunkCoordsFromPos(glm::vec3 pos) {
    return glm::ivec3(
        (int)std::floor((float)pos.x / (float)Constant::CHUNK_SIZE),
        (int)std::floor((float)pos.y / (float)Constant::CHUNK_HEIGHT),
        (int)std::floor((float)pos.z / (float)Constant::CHUNK_SIZE)
    );
}

glm::ivec3 VoxelMath::getVoxelWorldCoordsFromChunkCoords(glm::ivec3 chunk_coords) {
    return glm::ivec3(
        chunk_coords.x * (int)Constant::CHUNK_SIZE,
        chunk_coords.y * (int)Constant::CHUNK_HEIGHT,
        chunk_coords.z * (int)Constant::CHUNK_SIZE
    );
}

glm::ivec3 VoxelMath::getChunkCoordsFromVoxel(glm::ivec3 voxel_world_coords) {
    return glm::ivec3(
        (int)std::floor((float)voxel_world_coords.x / (float)Constant::CHUNK_SIZE),
        (int)std::floor((float)voxel_world_coords.y / (float)Constant::CHUNK_HEIGHT),
        (int)std::floor((float)voxel_world_coords.z / (float)Constant::CHUNK_SIZE)
    );
}

glm::uvec3 VoxelMath::getLocalVoxelCoords(glm::ivec3 voxel_world_coords, glm::ivec3 chunk_coords) {
    return glm::uvec3(
        (unsigned)(voxel_world_coords.x - chunk_coords.x * (int)Constant::CHUNK_SIZE),
        (unsigned)(voxel_world_coords.y - chunk_coords.y * (int)Constant::CHUNK_HEIGHT),
        (unsigned)(voxel_world_coords.z - chunk_coords.z * (int)Constant::CHUNK_SIZE)
    );
}