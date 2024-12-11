#ifndef VOXEL_MATH_HPP
#define VOXEL_MATH_HPP

#include <glm/glm.hpp>
#include "face_id.hpp"
#include "constant.hpp"

namespace VoxelMath {
    unsigned getVoxelIndex(unsigned x, unsigned y, unsigned z);
    unsigned getVoxelIndex(glm::uvec3 voxel_local_coords);
    glm::ivec3 getVoxelWorldCoords(glm::uvec3 voxel_local_coords, glm::ivec3 chunk_coords);
    glm::ivec3 getVoxelWorldCoordsFromPos(glm::vec3 pos);
    glm::ivec3 getVoxelWorldCoordsFromChunkCoords(glm::ivec3 chunk_coords);
    glm::ivec3 getChunkCoordsFromPos(glm::vec3 pos);
    glm::ivec3 getChunkCoordsFromVoxel(glm::ivec3 world_voxel_coords);
    glm::uvec3 getLocalVoxelCoords(glm::ivec3 voxel_world_coords, glm::ivec3 chunk_coords);
}


#endif