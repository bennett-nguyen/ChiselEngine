#ifndef RAY_CASTING_HPP
#define RAY_CASTING_HPP

#include "gl_constants.hpp"
#include "chunk_pool.hpp"
#include "conversions.hpp"

struct RayCastResult {
    bool is_detected_voxel;
    Direction detected_face;
    WorldPosition detected_voxel_position;
};

void rayCast(const chisel::ChunkPool& pool, RayCastResult &ray_cast_result, glm::vec3 position, glm::vec3 direction);
WorldPosition getAdjacentVoxel(const RayCastResult &ray_cast_result);
void breakBlock(chisel::ChunkPool& pool, WorldPosition voxel_position);
void placeBlock(chisel::ChunkPool& pool, WorldPosition adjacent_voxel_position, chisel::types::VoxelID block_id);

#endif