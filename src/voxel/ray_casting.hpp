#ifndef RAY_CASTING_HPP
#define RAY_CASTING_HPP

#include "world.hpp"
#include "constant.hpp"
#include "conversions.hpp"

struct RayCastResult {
    bool is_detected_voxel;
    FaceID detected_face;
    glm::ivec3 detected_voxel_world_pos;
};

void rayCast(RayCastResult &ray_cast_result, World &world, glm::vec3 position, glm::vec3 direction);
glm::ivec3 getAdjacentVoxel(const RayCastResult &ray_cast_result);

#endif