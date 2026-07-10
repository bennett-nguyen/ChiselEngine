#ifndef CHUNK_POOL_HPP
#define CHUNK_POOL_HPP

#include <queue>
#include <vector>
#include <ranges>
#include <unordered_set>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "chunk.hpp"

namespace chisel {
    using ChunkID = size_t;
    constexpr ChunkID NULL_CHUNK_ID = 0;

    class ChunkPool {
        std::vector<ChunkPtr> chunk_pool {};
        std::queue<ChunkID> allocated_chunks {};
        std::unordered_map<ChunkPosition, ChunkID> used_chunk_ids {};

        std::queue<ChunkPosition> build_queue {};
        std::queue<ChunkPosition> rebuild_queue {};

        std::unordered_set<ChunkPosition> chunks_to_build {};
        std::unordered_set<ChunkPosition> chunks_to_rebuild {};

        void build(ChunkPosition) const;
        void rebuild(ChunkPosition) const;

        [[nodiscard]] ChunkNeighbors forwardNeighboringChunks(ChunkPosition) const;
    public:
         ChunkPool();
        ~ChunkPool() = default;

        void use(ChunkPosition);
        void recycle(ChunkPosition);

        [[nodiscard]] ChunkID getUsedChunkID(ChunkPosition) const;
        [[nodiscard]] bool isPositionUsed(ChunkPosition) const;

        void enqueueForBuilding(ChunkPosition);
        void enqueueForRebuilding(ChunkPosition);

        void buildQueuedChunks();
        void rebuildQueuedChunks();

        void renderUsedChunk(ChunkPosition) const;
        void setVoxelIDAtPositionInChunk(types::VoxelID, LocalPosition, ChunkPosition) const;

        [[nodiscard]] bool isVoidAtInChunk(LocalPosition, ChunkPosition) const;
        [[nodiscard]] bool isVisible(ChunkPosition position, const std::array<glm::vec4, 6> &frustum_planes) const;
        [[nodiscard]] bool isBuilt(ChunkPosition) const;

        [[nodiscard]] const std::vector<ChunkPosition>& getUsedChunks() const;

        ChunkPool(const ChunkPool&)            = delete;
        ChunkPool& operator=(const ChunkPool&) = delete;
        ChunkPool(ChunkPool&&)                 = delete;
        ChunkPool& operator=(ChunkPool&&)      = delete;
    };
}

#endif
