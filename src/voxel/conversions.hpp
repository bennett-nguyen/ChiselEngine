#ifndef CONVERSIONS_HPP
#define CONVERSIONS_HPP

#include <glm/vec3.hpp>
#include <glm/common.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "constant.hpp"

using VoxelIndex = unsigned;
using WorldPosition = glm::ivec3;
using ChunkPosition = glm::ivec3;
using LocalPosition = glm::uvec3;

namespace Conversion {
    [[nodiscard]] VoxelIndex toIndex(LocalPosition);

    [[nodiscard]] WorldPosition toWorld(LocalPosition, ChunkPosition);
    [[nodiscard]] WorldPosition toWorld(glm::vec3 any_position);
    [[nodiscard]] WorldPosition chunkToWorld(ChunkPosition);

    [[nodiscard]] ChunkPosition toChunk(WorldPosition);
    [[nodiscard]] ChunkPosition toChunk(glm::vec3 any_position);

    [[nodiscard]] LocalPosition toLocal(WorldPosition, ChunkPosition);

    [[nodiscard]] glm::mat4 toChunkModel(ChunkPosition);

}

#endif