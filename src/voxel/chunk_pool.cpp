#include "chunk_pool.hpp"

constexpr unsigned EXTRA_RESERVED = 0;
constexpr unsigned WORLD_SIZE = 2 * chisel::EngineConstants::LOAD_DISTANCE + 1;
constexpr unsigned POOL_RESERVED_SIZE = WORLD_SIZE * WORLD_SIZE + EXTRA_RESERVED;

chisel::ChunkPool::ChunkPool() {
    chunk_pool.reserve(POOL_RESERVED_SIZE+1);
    used_chunk_ids.reserve(POOL_RESERVED_SIZE+1);

    chunk_pool.emplace_back(nullptr);
    for (size_t ID = 1; ID <= POOL_RESERVED_SIZE; ID++) {
        auto p_chunk = std::make_unique<Chunk>();
        p_chunk->preload();

        chunk_pool.emplace_back(std::move(p_chunk));
        allocated_chunks.push(ID);
    }
}

chisel::ChunkID chisel::ChunkPool::getUsedChunkID(const ChunkPosition position) const {
    if (not isPositionUsed(position)) return NULL_CHUNK_ID;
    return used_chunk_ids.at(position);
}

void chisel::ChunkPool::use(const ChunkPosition position) {
    if (isPositionUsed(position)) return;

    if (allocated_chunks.empty()) {
        std::cerr << "WARNING :: Ran out of unused chunk!" << '\n';
        return;
    }

    const ChunkID ID = allocated_chunks.front();
    allocated_chunks.pop();

    used_chunk_ids.emplace(position, ID);
    chunk_pool.at(ID)->setPosition(position);
    chunk_pool.at(ID)->buildVoxels();
}

void chisel::ChunkPool::recycle(const ChunkPosition position) {
    if (not isPositionUsed(position)) return;
    const ChunkID ID = used_chunk_ids.at(position);

    chunk_pool.at(ID)->destroyMesh();
    chunk_pool.at(ID)->resetVoxels();

    allocated_chunks.emplace(ID);
    used_chunk_ids.erase(position);
}

bool chisel::ChunkPool::isPositionUsed(const ChunkPosition position) const {
    return used_chunk_ids.count(position) != 0;
}

void chisel::ChunkPool::enqueueForBuilding(const ChunkPosition position) {
    if (not isPositionUsed(position)) return;
    const auto [_, is_inserted] = chunks_to_build.emplace(position);
    if (not is_inserted) return;
    build_queue.emplace(position);
}

void chisel::ChunkPool::enqueueForRebuilding(const ChunkPosition position) {
    if (not isPositionUsed(position)) return;
    const auto [_, is_inserted] = chunks_to_rebuild.emplace(position);
    if (not is_inserted) return;
    rebuild_queue.emplace(position);
}

void chisel::ChunkPool::buildQueuedChunks() {
    if (build_queue.empty()) return;
    unsigned num_chunks = EngineConstants::CHUNKS_TO_BUILD_PER_FRAME;

    while (num_chunks != 0 and not build_queue.empty()) {
        const auto position = build_queue.front();
        chunks_to_build.erase(position);
        build_queue.pop();

        if (not isPositionUsed(position)) continue;
        build(position);
        num_chunks--;
    }
}

void chisel::ChunkPool::rebuildQueuedChunks() {
    if (rebuild_queue.empty()) return;
    unsigned num_chunks = EngineConstants::CHUNKS_TO_REBUILD_PER_FRAME;

    while (num_chunks != 0 and not rebuild_queue.empty()) {
        const auto position = rebuild_queue.front();
        chunks_to_rebuild.erase(position);
        rebuild_queue.pop();

        if (not isPositionUsed(position)) continue;
        rebuild(position);
        num_chunks--;
    }
}

void chisel::ChunkPool::build(const ChunkPosition position) const {
    if (not isPositionUsed(position)) return;
    const auto ID = getUsedChunkID(position);
    chunk_pool.at(ID)->fetchNeighbors(forwardNeighboringChunks(position));
    chunk_pool.at(ID)->buildMesh();
}

void chisel::ChunkPool::rebuild(const ChunkPosition position) const {
    if (not isPositionUsed(position)) return;
    const auto ID = getUsedChunkID(position);
    chunk_pool.at(ID)->destroyMesh();
    chunk_pool.at(ID)->fetchNeighbors(forwardNeighboringChunks(position));
    chunk_pool.at(ID)->buildMesh();
}

ChunkNeighbors chisel::ChunkPool::forwardNeighboringChunks(const ChunkPosition chunk) const {
    const auto ID_NORTH = getUsedChunkID(chunk + CHUNK_NEIGHBORS_DIRECTION.at(Direction::North));
    const auto ID_SOUTH = getUsedChunkID(chunk + CHUNK_NEIGHBORS_DIRECTION.at(Direction::South));
    const auto ID_EAST = getUsedChunkID(chunk + CHUNK_NEIGHBORS_DIRECTION.at(Direction::East));
    const auto ID_WEST = getUsedChunkID(chunk + CHUNK_NEIGHBORS_DIRECTION.at(Direction::West));

    const auto ID_NORTH_EAST = getUsedChunkID(chunk + CHUNK_NEIGHBORS_DIRECTION.at(Direction::North | Direction::East));
    const auto ID_NORTH_WEST = getUsedChunkID(chunk + CHUNK_NEIGHBORS_DIRECTION.at(Direction::North | Direction::West));
    const auto ID_SOUTH_EAST = getUsedChunkID(chunk + CHUNK_NEIGHBORS_DIRECTION.at(Direction::South | Direction::East));
    const auto ID_SOUTH_WEST = getUsedChunkID(chunk + CHUNK_NEIGHBORS_DIRECTION.at(Direction::South | Direction::West));

    return {
        .north = ID_NORTH != NULL_CHUNK_ID ? chunk_pool.at(ID_NORTH).get() : nullptr,
        .south = ID_SOUTH != NULL_CHUNK_ID ? chunk_pool.at(ID_SOUTH).get() : nullptr,
        .east  = ID_EAST != NULL_CHUNK_ID ? chunk_pool.at(ID_EAST).get() : nullptr,
        .west  = ID_WEST != NULL_CHUNK_ID ? chunk_pool.at(ID_WEST).get() : nullptr,

        .north_east = ID_NORTH_EAST != NULL_CHUNK_ID ? chunk_pool.at(ID_NORTH_EAST).get() : nullptr,
        .north_west = ID_NORTH_WEST != NULL_CHUNK_ID ? chunk_pool.at(ID_NORTH_WEST).get() : nullptr,
        .south_east = ID_SOUTH_EAST != NULL_CHUNK_ID ? chunk_pool.at(ID_SOUTH_EAST).get() : nullptr,
        .south_west = ID_SOUTH_WEST != NULL_CHUNK_ID ? chunk_pool.at(ID_SOUTH_WEST).get() : nullptr
    };
}

void chisel::ChunkPool::renderUsedChunk(const ChunkPosition position) const {
    if (not isPositionUsed(position)) return;
    const auto ID = getUsedChunkID(position);
    chunk_pool.at(ID)->render();
}

void chisel::ChunkPool::setVoxelIDAtPositionInChunk(const VoxelID voxel_id, const LocalPosition local, const ChunkPosition chunk) const {
    if (not isPositionUsed(chunk)) return;
    const auto ID = getUsedChunkID(chunk);
    chunk_pool.at(ID)->setVoxelIDAtPosition(voxel_id, local);
}

bool chisel::ChunkPool::isVoidAtInChunk(const LocalPosition local, const ChunkPosition chunk) const {
    if (not isPositionUsed(chunk)) return false;
    const auto ID = getUsedChunkID(chunk);
    return chunk_pool.at(ID)->isVoidAt(local);
}

bool chisel::ChunkPool::isVisible(const ChunkPosition position, const std::array<glm::vec4, 6> &frustum_planes) const {
    if (not isPositionUsed(position)) return false;
    const auto ID = getUsedChunkID(position);
    return chunk_pool.at(ID)->isChunkVisible(frustum_planes);
}

bool chisel::ChunkPool::isBuilt(const ChunkPosition position) const {
    if (not isPositionUsed(position)) return false;
    const auto ID = getUsedChunkID(position);
    return chunk_pool.at(ID)->isBuilt();
}

const std::vector<ChunkPosition>& chisel::ChunkPool::getUsedChunks() const {
    static std::vector<ChunkPosition> used_chunks {};
    used_chunks.clear();

    for (auto const& [position, _] : used_chunk_ids) {
        used_chunks.emplace_back(position);
    }

    return used_chunks;
}