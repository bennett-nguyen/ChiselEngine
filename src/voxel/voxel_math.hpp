#ifndef VOXEL_MATH_HPP
#define VOXEL_MATH_HPP

#include <glm/glm.hpp>
#include "constant.hpp"

unsigned get_voxel_idx(int x, int y, int z);
unsigned get_voxel_idx(glm::ivec3 voxel_local_coords);
glm::ivec3 get_voxel_world_coords(glm::ivec3 voxel_local_coords, glm::ivec3 chunk_coords);
glm::ivec3 get_voxel_world_coords_from_pos(glm::vec3 pos);
glm::ivec3 get_voxel_world_coords_from_chunk_coords(glm::ivec3 chunk_coords);
glm::ivec3 get_chunk_coords_from_pos(glm::vec3 pos);
glm::ivec3 get_chunk_coords_from_voxel(glm::ivec3 world_voxel_coords);
glm::ivec3 get_local_voxel_coords(glm::ivec3 voxel_world_coords, glm::ivec3 chunk_coords);

#endif