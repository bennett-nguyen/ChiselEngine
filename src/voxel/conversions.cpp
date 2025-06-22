#include "conversions.hpp"

unsigned Conversion::toIndex(const unsigned x, const unsigned y, const unsigned z) {
    return x + Constant::CHUNK_SIZE * z + Constant::CHUNK_AREA * y;
}

unsigned Conversion::toIndex(const glm::uvec3 local_position) {
    return toIndex(local_position.x, local_position.y, local_position.z);
}

glm::ivec3 Conversion::toWorld(const glm::uvec3 local_position, const glm::ivec3 chunk_position) {
    return {
        static_cast<int>(local_position.x) + chunk_position.x * static_cast<int>(Constant::CHUNK_SIZE),
        static_cast<int>(local_position.y) + chunk_position.y * static_cast<int>(Constant::CHUNK_HEIGHT),
        static_cast<int>(local_position.z) + chunk_position.z * static_cast<int>(Constant::CHUNK_SIZE)
    };
}

glm::ivec3 Conversion::toWorld(const glm::vec3 any_position) {
    return {
        static_cast<int>(std::floor(any_position.x)),
        static_cast<int>(std::floor(any_position.y)),
        static_cast<int>(std::floor(any_position.z))
    };
}

glm::ivec3 Conversion::chunkToWorld(const glm::ivec3 chunk_position) {
    return {
        chunk_position.x * static_cast<int>(Constant::CHUNK_SIZE),
        chunk_position.y * static_cast<int>(Constant::CHUNK_HEIGHT),
        chunk_position.z * static_cast<int>(Constant::CHUNK_SIZE)
    };
}

glm::ivec3 Conversion::toChunk(const glm::ivec3 world_position) {
    return {
        static_cast<int>(std::floor(static_cast<float>(world_position.x) / static_cast<float>(Constant::CHUNK_SIZE))),
        static_cast<int>(std::floor(static_cast<float>(world_position.y) / static_cast<float>(Constant::CHUNK_HEIGHT))),
        static_cast<int>(std::floor(static_cast<float>(world_position.z) / static_cast<float>(Constant::CHUNK_SIZE)))
    };
}

glm::ivec3 Conversion::toChunk(const glm::vec3 any_position) {
    return {
        static_cast<int>(std::floor(any_position.x / static_cast<float>(Constant::CHUNK_SIZE))),
        static_cast<int>(std::floor(any_position.y / static_cast<float>(Constant::CHUNK_HEIGHT))),
        static_cast<int>(std::floor(any_position.z / static_cast<float>(Constant::CHUNK_SIZE)))
    };
}

glm::uvec3 Conversion::toLocal(const glm::ivec3 world_position, const glm::ivec3 chunk_position) {
    return {
        static_cast<unsigned>(world_position.x - chunk_position.x * static_cast<int>(Constant::CHUNK_SIZE)),
        static_cast<unsigned>(world_position.y - chunk_position.y * static_cast<int>(Constant::CHUNK_HEIGHT)),
        static_cast<unsigned>(world_position.z - chunk_position.z * static_cast<int>(Constant::CHUNK_SIZE))
    };
}