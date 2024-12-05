#include "voxel_handler.hpp"

float FRAC0(float x) {
    return x - std::floor(x);
}

float FRAC1(float x) {
    return 1.0f - x + std::floor(x);
}

bool VoxelHandler::get_is_detected_voxel() {
    return m_is_detected_voxel;
}

glm::ivec3 VoxelHandler::get_detected_voxel() {
    return m_detected_voxel;
}

glm::ivec3 VoxelHandler::get_chunk_coords_of_detected_voxel() {
    return m_chunk_coords_of_detected_voxel;
}

FaceID VoxelHandler::get_detected_voxel_face() {
    return m_detected_voxel_face;
}

unsigned VoxelHandler::get_detected_voxel_idx() {
    return m_detected_voxel_idx;
}

void VoxelHandler::ray_cast(glm::vec3 current_pos, glm::vec3 normalized_direction) {
    auto chunk_map = *mp_chunk_map;
    glm::ivec3 current_voxel = get_voxel_world_coords_from_pos(current_pos);

    m_is_detected_voxel = false;

    int step_x = glm::sign(normalized_direction.x);
    int step_y = glm::sign(normalized_direction.y);
    int step_z = glm::sign(normalized_direction.z);
    FaceID face_x, face_y, face_z;

    float t_delta_x, t_delta_y, t_delta_z;
    float t_max_x, t_max_y, t_max_z;
    float current_distance;
    
    unsigned voxel_idx;
    glm::ivec3 voxel_local_coords, chunk_coords_of_voxel;

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

    while (current_distance < Constant::MAX_RAY_LENGTH) {
        chunk_coords_of_voxel = get_chunk_coords_from_voxel(current_voxel);

        if (1 == mp_chunk_map->count(chunk_coords_of_voxel)) {
            voxel_local_coords = get_local_voxel_coords(current_voxel, chunk_coords_of_voxel);
            voxel_idx = get_voxel_idx(voxel_local_coords);

            if (!chunk_map[chunk_coords_of_voxel]->is_air_at(voxel_idx)) {
                m_is_detected_voxel = true;
                m_detected_voxel = voxel_local_coords;
                m_detected_voxel_idx = voxel_idx;
                m_chunk_coords_of_detected_voxel = chunk_coords_of_voxel;
                break;
            }
        }

        if (t_max_x < t_max_y) {
            if (t_max_x < t_max_z) {
                t_max_x += t_delta_x;
                current_voxel.x += step_x;
                m_detected_voxel_face = face_x;
                current_distance = t_max_x;
            } else {
                t_max_z += t_delta_z;
                current_voxel.z += step_z;
                m_detected_voxel_face = face_z;
                current_distance = t_max_z;
            }
        } else {
            if (t_max_y < t_max_z) {
                t_max_y += t_delta_y;
                current_voxel.y += step_y;
                m_detected_voxel_face = face_y;
                current_distance = t_max_y;
            } else {
                t_max_z += t_delta_z;
                current_voxel.z += step_z;
                m_detected_voxel_face = face_z;
                current_distance = t_max_z;
            }
        }
    }
}