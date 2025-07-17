#ifndef CONVERSIONS_HPP
#define CONVERSIONS_HPP

#include <glm/vec3.hpp>
#include <glm/common.hpp>

#include "constant.hpp"

typedef unsigned VoxelIndex;
typedef glm::ivec3 WorldPosition;
typedef glm::ivec3 ChunkPosition;
typedef glm::uvec3 LocalPosition;

namespace Conversion {
    [[nodiscard]] VoxelIndex toIndex(LocalPosition);

    [[nodiscard]] WorldPosition toWorld(LocalPosition, ChunkPosition);
    [[nodiscard]] WorldPosition toWorld(glm::vec3 any_position);
    [[nodiscard]] WorldPosition chunkToWorld(ChunkPosition);

    [[nodiscard]] ChunkPosition toChunk(WorldPosition);
    [[nodiscard]] ChunkPosition toChunk(glm::vec3 any_position);

    [[nodiscard]] LocalPosition toLocal(WorldPosition, ChunkPosition);
}

#endif