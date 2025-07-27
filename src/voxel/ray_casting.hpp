#ifndef RAY_CASTING_HPP
#define RAY_CASTING_HPP

#include "constant.hpp"
#include "chunk_pool.hpp"
#include "conversions.hpp"

struct RayCastResult {
    bool is_detected_voxel;
    Direction detected_face;
    WorldPosition detected_voxel_position;
};

void rayCast(RayCastResult &ray_cast_result, glm::vec3 position, glm::vec3 direction);
WorldPosition getAdjacentVoxel(const RayCastResult &ray_cast_result);
void breakBlock(WorldPosition voxel_position);
void placeBlock(WorldPosition adjacent_voxel_position);

#endif