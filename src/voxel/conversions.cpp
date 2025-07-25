#include "conversions.hpp"

VoxelIndex Conversion::toIndex(const LocalPosition local) {
    return local.x + Constant::CHUNK_SIZE * local.z + Constant::CHUNK_AREA * local.y;
}

WorldPosition Conversion::toWorld(const LocalPosition local, const ChunkPosition chunk) {
    return {
        static_cast<int>(local.x) + chunk.x * static_cast<int>(Constant::CHUNK_SIZE),
        static_cast<int>(local.y) + chunk.y * static_cast<int>(Constant::CHUNK_HEIGHT),
        static_cast<int>(local.z) + chunk.z * static_cast<int>(Constant::CHUNK_SIZE)
    };
}

WorldPosition Conversion::toWorld(const glm::vec3 any_position) {
    return WorldPosition { glm::floor(any_position) };
}

WorldPosition Conversion::chunkToWorld(const ChunkPosition chunk) {
    return {
        chunk.x * static_cast<int>(Constant::CHUNK_SIZE),
        chunk.y * static_cast<int>(Constant::CHUNK_HEIGHT),
        chunk.z * static_cast<int>(Constant::CHUNK_SIZE)
    };
}

ChunkPosition Conversion::toChunk(const WorldPosition world) {
    return {
        static_cast<int>(glm::floor(static_cast<float>(world.x) / static_cast<float>(Constant::CHUNK_SIZE))),
        static_cast<int>(glm::floor(static_cast<float>(world.y) / static_cast<float>(Constant::CHUNK_HEIGHT))),
        static_cast<int>(glm::floor(static_cast<float>(world.z) / static_cast<float>(Constant::CHUNK_SIZE)))
    };
}

ChunkPosition Conversion::toChunk(const glm::vec3 any_position) {
    return {
        static_cast<int>(glm::floor(any_position.x / static_cast<float>(Constant::CHUNK_SIZE))),
        static_cast<int>(glm::floor(any_position.y / static_cast<float>(Constant::CHUNK_HEIGHT))),
        static_cast<int>(glm::floor(any_position.z / static_cast<float>(Constant::CHUNK_SIZE)))
    };
}

LocalPosition Conversion::toLocal(const WorldPosition world, const ChunkPosition chunk) {
    return {
        static_cast<unsigned>(world.x - chunk.x * static_cast<int>(Constant::CHUNK_SIZE)),
        static_cast<unsigned>(world.y - chunk.y * static_cast<int>(Constant::CHUNK_HEIGHT)),
        static_cast<unsigned>(world.z - chunk.z * static_cast<int>(Constant::CHUNK_SIZE))
    };
}

glm::mat4 Conversion::toChunkModel(const ChunkPosition chunk) {
    glm::mat4 model(1.0f);
    const ChunkPosition chunk_position_to_world = chunkToWorld(chunk);
    model = glm::translate(model, glm::vec3(chunk_position_to_world));
    return model;
}
