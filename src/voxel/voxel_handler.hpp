#ifndef VOXEL_HANDLER
#define VOXEL_HANDLER

#include <limits>
#include <glm/glm.hpp>
#include <unordered_map>
#include "constant.hpp"
#include "chunk.hpp"
#include "face_id.hpp"
#include <glm/gtx/hash.hpp>
#include "voxel_math.hpp"

class VoxelHandler {
public:
    VoxelHandler(std::unordered_map<glm::ivec3, Chunk*> *p_chunk_map)
        : mp_chunk_map(p_chunk_map) {}

    void rayCast(glm::vec3 current_pos, glm::vec3 normalized_direction);

    bool IsDetectedVoxel();
    FaceID getDetectedVoxelFace();
    unsigned getDetectedVoxelIndex();
    glm::uvec3 getDetectedVoxelLocalCoords();
    glm::ivec3 getChunkCoordsOfDetectedVoxel();
    glm::ivec3 getVoxelWorldCoordsNextToDetectedVoxel();

private:
    bool m_is_detected_voxel;
    FaceID m_detected_voxel_face;
    unsigned m_detected_voxel_idx;
    glm::uvec3 m_detected_voxel_local_coords;
    glm::ivec3 m_chunk_coords_of_detected_voxel;
    glm::ivec3 m_voxel_world_coords_next_to_detected_voxel;

    std::unordered_map<glm::ivec3, Chunk*> *mp_chunk_map;
};

#endif