#ifndef CHUNK_POOL_HPP
#define CHUNK_POOL_HPP

#include <queue>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <glm/gtx/hash.hpp>

#include "chunk.hpp"

typedef size_t ChunkID;

namespace ChunkPool {
    void init();
    void destroy();

    void use(ChunkPosition);
    void free(ChunkPosition);

    void enqueueForBuilding(ChunkPosition);
    void enqueueForRebuilding(ChunkPosition);

    void build(ChunkPosition);
    void rebuild(ChunkPosition);
    void buildQueuedChunks();
    void rebuildQueuedChunks();

    void render(ChunkPosition);
    void setVoxelIDAtPositionInChunk(VoxelID, LocalPosition, ChunkPosition);

    [[nodiscard]] bool isBuilt(ChunkPosition);
    [[nodiscard]] bool isChunkUsed(ChunkPosition);
    [[nodiscard]] bool isVoidAtInChunk(LocalPosition, ChunkPosition);
    [[nodiscard]] bool isVisible(ChunkPosition, const std::array<glm::vec4, 6> &frustum_planes);
    [[nodiscard]] std::vector<ChunkPosition> getUsedChunksPositions();
}

#endif
