#include "chunk_pool.hpp"

constexpr unsigned CHUNKS_TO_BUILD_PER_FRAME = 5;
constexpr unsigned CHUNKS_TO_REBUILD_PER_FRAME = 3;

std::vector<Chunk*> chunk_pool;

std::queue<ChunkID> allocated_chunks;
std::queue<ChunkPosition> build_queue, rebuild_queue;

std::unordered_set<ChunkPosition> chunks_to_build, chunks_to_rebuild;
std::unordered_map<ChunkPosition, ChunkID> used_chunks_id;

[[nodiscard]] Chunk* getChunkPointer(const ChunkID ID) {
    return chunk_pool[ID];
}

[[nodiscard]] Chunk* getUsedChunkPointer(const ChunkPosition position) {
    if (!ChunkPool::isChunkUsed(position)) return nullptr;
    const ChunkID ID = used_chunks_id[position];
    return getChunkPointer(ID);
}

void ChunkPool::init() {
    constexpr unsigned EXTRA_RESERVED = 0;
    constexpr unsigned WORLD_SIZE = 2 * Constant::LOAD_DISTANCE + 1;
    constexpr unsigned POOL_RESERVED_SIZE = WORLD_SIZE * WORLD_SIZE + EXTRA_RESERVED;
    chunk_pool.reserve(POOL_RESERVED_SIZE);

    for (size_t ID = 0; ID < POOL_RESERVED_SIZE; ID++) {
        chunk_pool[ID] = new Chunk();
        allocated_chunks.push(ID);
    }

    std::cout << "LOG :: Chunk Pool created with " << allocated_chunks.size() << " allocated chunks." << '\n';
}

void ChunkPool::destroy() {
    for (const auto &ptr_chunk : chunk_pool) {
        delete ptr_chunk;
    }
}

void ChunkPool::use(const ChunkPosition position) {
    if (isChunkUsed(position)) return;

    if (allocated_chunks.empty()) {
        std::cerr << "WARNING :: Ran out of unused chunk!" << '\n';
        return;
    }

    const ChunkID ID = allocated_chunks.front();
    allocated_chunks.pop();

    used_chunks_id.insert({ position, ID });
    const auto current_chunk = getChunkPointer(ID);
    current_chunk->setPosition(position);
    current_chunk->buildVoxels();
}

void ChunkPool::free(const ChunkPosition position) {
    if (!isChunkUsed(position)) return;
    const auto current_chunk = getUsedChunkPointer(position);

    current_chunk->destroyMesh();
    current_chunk->resetVoxels();

    allocated_chunks.push(used_chunks_id[position]);
    used_chunks_id.erase(position);
}

void ChunkPool::enqueueForBuilding(const ChunkPosition position) {
    if (chunks_to_build.find(position) != chunks_to_build.end()) return;
    chunks_to_build.insert(position);
    build_queue.push(position);
}


void ChunkPool::enqueueForRebuilding(const ChunkPosition position) {
    if (chunks_to_rebuild.find(position) != chunks_to_rebuild.end()) return;
    chunks_to_rebuild.insert(position);
    rebuild_queue.push(position);
}

void ChunkPool::build(const ChunkPosition position) {
    if (!isChunkUsed(position)) return;
    const auto current_chunk = getUsedChunkPointer(position);

    const auto north = position + ChunkPosition(1, 0, 0);
    const auto south = position - ChunkPosition(1, 0, 0);
    const auto east  = position + ChunkPosition(0, 0, 1);
    const auto west  = position - ChunkPosition(0, 0, 1);

    const auto north_neighbor = getUsedChunkPointer(north);
    const auto south_neighbor = getUsedChunkPointer(south);
    const auto east_neighbor  = getUsedChunkPointer(east);
    const auto west_neighbor  = getUsedChunkPointer(west);

    current_chunk->buildMesh(north_neighbor, south_neighbor, east_neighbor, west_neighbor);
}

void ChunkPool::rebuild(const ChunkPosition position) {
    if (!isChunkUsed(position)) return;
    getUsedChunkPointer(position)->destroyMesh();
    build(position);
}

void ChunkPool::buildQueuedChunks() {
    if (build_queue.empty()) return;
    unsigned num_chunks = CHUNKS_TO_BUILD_PER_FRAME;

    while (num_chunks && !build_queue.empty()) {
        const ChunkPosition position = build_queue.front();
        build_queue.pop();
        chunks_to_build.erase(position);

        if (!isChunkUsed(position)) {
            continue;
        }

        build(position);
        num_chunks--;
    }
}

void ChunkPool::rebuildQueuedChunks() {
    if (rebuild_queue.empty()) return;
    unsigned num_chunks = CHUNKS_TO_REBUILD_PER_FRAME;

    while (num_chunks && !rebuild_queue.empty()) {
        const ChunkPosition position = rebuild_queue.front();
        rebuild_queue.pop();
        chunks_to_rebuild.erase(position);

        if (!isChunkUsed(position)) {
            continue;
        }

        rebuild(position);
        num_chunks--;
    }
}

void ChunkPool::render(const ChunkPosition position) {
    if (!isChunkUsed(position)) return;
    getUsedChunkPointer(position)->render();
}

void ChunkPool::setVoxelIDAtPositionInChunk(const VoxelID voxel_id, const LocalPosition local, const ChunkPosition chunk) {
    if (!isChunkUsed(chunk)) return;
    getUsedChunkPointer(chunk)->setVoxelIDAtPosition(voxel_id, local);
}

bool ChunkPool::isChunkUsed(const ChunkPosition position) {
    return 0 != used_chunks_id.count(position);
}

bool ChunkPool::isVoidAtInChunk(const LocalPosition local, const ChunkPosition chunk) {
    if (!isChunkUsed(chunk)) return false;
    return getUsedChunkPointer(chunk)->isVoidAt(local);
}

bool ChunkPool::isVisible(const ChunkPosition position, const std::vector<glm::vec4> &frustum_planes) {
    if (!isChunkUsed(position)) return false;
    return getUsedChunkPointer(position)->isChunkVisible(frustum_planes);
}

std::vector<ChunkPosition> ChunkPool::getUsedChunksPositions() {
    std::vector<ChunkPosition> positions;
    for (const auto &[position, _] : used_chunks_id) {
        positions.push_back(position);
    }

    return positions;
}

bool ChunkPool::isBuilt(const ChunkPosition position) {
    if (!isChunkUsed(position)) return false;
    return getUsedChunkPointer(position)->isBuilt();
}
