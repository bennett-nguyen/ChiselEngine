#include "aabb.hpp"

void AABB::reset() {
    vmin = glm::vec3(10000.0f);
    vmax = glm::vec3(-10000.0f);
}

void AABB::translate(const ChunkPosition chunk) {
    vmax += glm::vec3(1.0f);
    const glm::mat4 chunk_model = Conversion::toChunkModel(chunk);
    vmin = glm::vec3(chunk_model * glm::vec4(vmin.x, vmin.y, vmin.z, 1.0f));
    vmax = glm::vec3(chunk_model * glm::vec4(vmax.x, vmax.y, vmax.z, 1.0f));
}

void AABB::updateWithCubeFace(const Direction face, const LocalPosition voxel_origin) {
    const glm::vec3 v0 = voxel_origin + FACE_VERTICES.at(face).at(0);
    const glm::vec3 v1 = voxel_origin + FACE_VERTICES.at(face).at(1);
    const glm::vec3 v2 = voxel_origin + FACE_VERTICES.at(face).at(2);
    const glm::vec3 v3 = voxel_origin + FACE_VERTICES.at(face).at(3);

    const glm::vec3 min_v = glm::min(v0, v1, v2, v3);
    const glm::vec3 max_v = glm::max(v0, v1, v2, v3);

    vmin = glm::min(vmin, min_v);
    vmax = glm::max(vmax, max_v);
}