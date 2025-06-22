#ifndef CONVERSIONS_HPP
#define CONVERSIONS_HPP

#include <glm/glm.hpp>

#include "constant.hpp"

/*
* Coordinate Systems:
* Local: The coordinate system for voxels in a single chunk  (uvec3)
* Chunk: The coordinate system for chunks in the world       (ivec3)
* World: The coordinate system for voxels in the world       (ivec3)

* Misc:
* Index: The index of a voxel in a chunk
*/

namespace Conversion {
    unsigned toIndex(unsigned x, unsigned y, unsigned z);
    unsigned toIndex(glm::uvec3 local_position);

    glm::ivec3 toWorld(glm::uvec3 local_position, glm::ivec3 chunk_position);
    glm::ivec3 toWorld(glm::vec3 any_position);
    glm::ivec3 chunkToWorld(glm::ivec3 chunk_position);

    glm::ivec3 toChunk(glm::ivec3 world_position);
    glm::ivec3 toChunk(glm::vec3 any_position);

    glm::uvec3 toLocal(glm::ivec3 world_position, glm::ivec3 chunk_position);
}

#endif