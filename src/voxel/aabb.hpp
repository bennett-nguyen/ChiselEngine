#ifndef AABB_HPP
#define AABB_HPP

#include <glm/vec3.hpp>
#include <glm/gtx/extended_min_max.hpp>

#include "direction.hpp"
#include "conversions.hpp"

struct AABB {
    glm::vec3 vmin {}, vmax {};

    void reset();
    void translate(ChunkPosition);
    void updateWithCubeFace(Direction face, LocalPosition voxel_origin);
};

#endif
