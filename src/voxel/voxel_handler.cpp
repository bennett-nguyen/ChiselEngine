#include "voxel_handler.hpp"

float FRAC0(float x) {
    return x - std::floor(x);
}

float FRAC1(float x) {
    return 1.0f - x + std::floor(x);
}

bool VoxelHandler::IsDetectedVoxel() {
    return m_is_detected_voxel;
}

glm::ivec3 VoxelHandler::getVoxelWorldCoordsNextToDetectedVoxel() {
    return m_voxel_world_coords_next_to_detected_voxel;
}

glm::ivec3 VoxelHandler::getDetectedVoxelWorldCoords() {
    return m_detected_voxel_world_coords;
}

FaceID VoxelHandler::getDetectedVoxelFace() {
    return m_detected_voxel_face;
}

void VoxelHandler::rayCast(glm::vec3 current_pos, glm::vec3 normalized_direction) {
    auto chunk_map = *mp_chunk_map;
    glm::ivec3 current_voxel = VoxelMath::getVoxelWorldCoordsFromPos(current_pos);

    m_is_detected_voxel = false;

    int step_x = (int)glm::sign(normalized_direction.x);
    int step_y = (int)glm::sign(normalized_direction.y);
    int step_z = (int)glm::sign(normalized_direction.z);
    FaceID face_x, face_y, face_z;

    float t_delta_x = 0.0f, t_delta_y = 0.0f, t_delta_z = 0.0f;
    float t_max_x = 0.0f, t_max_y = 0.0f, t_max_z = 0.0f;

    unsigned voxel_idx;
    glm::uvec3 voxel_local_coords;
    glm::ivec3 chunk_coords_of_voxel;

    if (step_x == 0) {
        t_delta_x = 10000000.0f;
    } else {
        face_x = step_x > 0 ? South : North;
        t_delta_x = std::abs(std::min(float(1.0 / normalized_direction.x), 10000000.0f));

        if (step_x > 0) {
            t_max_x = FRAC1(current_pos.x) * t_delta_x;
        } else {
            t_max_x = FRAC0(current_pos.x) * t_delta_x;
        }
    }

    if (step_y == 0) {
        t_delta_y = 10000000.0f;
    } else {
        face_y = step_y > 0 ? Bottom : Top;
        t_delta_y = std::abs(std::min(float(1.0 / normalized_direction.y), 10000000.0f));

        if (step_y > 0) {
            t_max_y = FRAC1(current_pos.y) * t_delta_y;
        } else {
            t_max_y = FRAC0(current_pos.y) * t_delta_y;
        }
    }

    if (step_z == 0) {
        t_delta_z = 10000000.0f;
    } else {
        face_z = step_z > 0 ? West : East;
        t_delta_z = std::abs(std::min(float(1.0 / normalized_direction.z), 10000000.0f));

        if (step_z > 0) {
            t_max_z = FRAC1(current_pos.z) * t_delta_z;
        } else {
            t_max_z = FRAC0(current_pos.z) * t_delta_z;
        }
    }

    unsigned voxel_traversed = 0;
    float current_ray_length = 0.0f;

    while (voxel_traversed <= Constant::MAX_VOXEL_TRAVERSED || current_ray_length <= Constant::MAX_RAY_LENGTH) {
        chunk_coords_of_voxel = VoxelMath::getChunkCoordsFromVoxel(current_voxel);

        if (1 == mp_chunk_map->count(chunk_coords_of_voxel)) {
            voxel_local_coords = VoxelMath::getLocalVoxelCoords(current_voxel, chunk_coords_of_voxel);
            voxel_idx = VoxelMath::getVoxelIndex(voxel_local_coords);

            if (!chunk_map[chunk_coords_of_voxel]->isAirAt(voxel_idx)) {
                m_is_detected_voxel = true;
                m_detected_voxel_world_coords = current_voxel;

                if (m_detected_voxel_face == FaceID::North) {
                    m_voxel_world_coords_next_to_detected_voxel = glm::ivec3(current_voxel.x+1, current_voxel.y, current_voxel.z);
                } else if (m_detected_voxel_face == FaceID::South) {
                    m_voxel_world_coords_next_to_detected_voxel = glm::ivec3(current_voxel.x-1, current_voxel.y, current_voxel.z);
                } else if (m_detected_voxel_face == FaceID::East) {
                    m_voxel_world_coords_next_to_detected_voxel = glm::ivec3(current_voxel.x, current_voxel.y, current_voxel.z+1);
                } else if (m_detected_voxel_face == FaceID::West) {
                    m_voxel_world_coords_next_to_detected_voxel = glm::ivec3(current_voxel.x, current_voxel.y, current_voxel.z-1);
                } else if (m_detected_voxel_face == FaceID::Top) {
                    m_voxel_world_coords_next_to_detected_voxel = glm::ivec3(current_voxel.x, current_voxel.y+1, current_voxel.z);
                } else if (m_detected_voxel_face == FaceID::Bottom) {
                    m_voxel_world_coords_next_to_detected_voxel = glm::ivec3(current_voxel.x, current_voxel.y-1, current_voxel.z);
                }

                break;
            }
        }

        if (t_max_x < t_max_y) {
            if (t_max_x < t_max_z) {
                t_max_x += t_delta_x;
                current_voxel.x += step_x;
                m_detected_voxel_face = face_x;
                current_ray_length = t_max_x;
            } else {
                t_max_z += t_delta_z;
                current_voxel.z += step_z;
                m_detected_voxel_face = face_z;
                current_ray_length = t_max_z;
            }
        } else {
            if (t_max_y < t_max_z) {
                t_max_y += t_delta_y;
                current_voxel.y += step_y;
                m_detected_voxel_face = face_y;
                current_ray_length = t_max_y;
            } else {
                t_max_z += t_delta_z;
                current_voxel.z += step_z;
                m_detected_voxel_face = face_z;
                current_ray_length = t_max_z;
            }
        }

        voxel_traversed++;
    }
}