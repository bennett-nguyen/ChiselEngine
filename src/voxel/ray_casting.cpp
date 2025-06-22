#include "ray_casting.hpp"

float FRAC0(const float x) {
    return x - std::floor(x);
}

float FRAC1(const float x) {
    return 1.0f - x + std::floor(x);
}

void rayCast(RayCastResult &ray_cast_result, World &world, glm::vec3 position, glm::vec3 direction) {
    auto current_voxel = Conversion::toWorld(position);
    glm::vec3 normalized_direction = glm::normalize(direction);

    ray_cast_result.is_detected_voxel = false;

    const auto step_x = static_cast<int>(glm::sign(normalized_direction.x));
    const auto step_y = static_cast<int>(glm::sign(normalized_direction.y));
    const auto step_z = static_cast<int>(glm::sign(normalized_direction.z));
    FaceID face_x = Nil, face_y = Nil, face_z = Nil;

    float t_delta_x = 0.0f, t_delta_y = 0.0f, t_delta_z = 0.0f;
    float t_max_x = 0.0f, t_max_y = 0.0f, t_max_z = 0.0f;

    glm::uvec3 local_position;
    glm::ivec3 chunk_position_of_voxel;

    if (step_x == 0) {
        t_delta_x = 10000000.0f;
    } else {
        face_x = step_x > 0 ? FaceID::South : FaceID::North;
        t_delta_x = std::abs(std::min(static_cast<float>(1.0 / normalized_direction.x), 10000000.0f));

        if (step_x > 0) {
            t_max_x = FRAC1(position.x) * t_delta_x;
        } else {
            t_max_x = FRAC0(position.x) * t_delta_x;
        }
    }

    if (step_y == 0) {
        t_delta_y = 10000000.0f;
    } else {
        face_y = step_y > 0 ? FaceID::Bottom : FaceID::Top;
        t_delta_y = std::abs(std::min(static_cast<float>(1.0 / normalized_direction.y), 10000000.0f));

        if (step_y > 0) {
            t_max_y = FRAC1(position.y) * t_delta_y;
        } else {
            t_max_y = FRAC0(position.y) * t_delta_y;
        }
    }

    if (step_z == 0) {
        t_delta_z = 10000000.0f;
    } else {
        face_z = step_z > 0 ? FaceID::West : FaceID::East;
        t_delta_z = std::abs(std::min(static_cast<float>(1.0 / normalized_direction.z), 10000000.0f));

        if (step_z > 0) {
            t_max_z = FRAC1(position.z) * t_delta_z;
        } else {
            t_max_z = FRAC0(position.z) * t_delta_z;
        }
    }

    unsigned voxel_traversed = 0;
    float current_ray_length = 0.0f;

    while (voxel_traversed <= Constant::MAX_VOXEL_TRAVERSED || current_ray_length <= Constant::MAX_RAY_LENGTH) {
        chunk_position_of_voxel = Conversion::toChunk(current_voxel);

        if (isChunkExist(world, chunk_position_of_voxel)) {
            local_position = Conversion::toLocal(current_voxel, chunk_position_of_voxel);
            Chunk* ptr_chunk_of_current_voxel = getChunkPointer(world, chunk_position_of_voxel);

            if (!isVoid(local_position, ptr_chunk_of_current_voxel)) {
                ray_cast_result.is_detected_voxel = true;
                ray_cast_result.detected_voxel_world_pos = current_voxel;
                break;
            }
        }

        if (t_max_x < t_max_y) {
            if (t_max_x < t_max_z) {
                t_max_x += t_delta_x;
                current_voxel.x += step_x;
                ray_cast_result.detected_face = face_x;
                current_ray_length = t_max_x;
            } else {
                t_max_z += t_delta_z;
                current_voxel.z += step_z;
                ray_cast_result.detected_face = face_z;
                current_ray_length = t_max_z;
            }
        } else {
            if (t_max_y < t_max_z) {
                t_max_y += t_delta_y;
                current_voxel.y += step_y;
                ray_cast_result.detected_face = face_y;
                current_ray_length = t_max_y;
            } else {
                t_max_z += t_delta_z;
                current_voxel.z += step_z;
                ray_cast_result.detected_face = face_z;
                current_ray_length = t_max_z;
            }
        }

        voxel_traversed++;
    }
}

glm::ivec3 getAdjacentVoxel(const RayCastResult &ray_cast_result) {
    if (!ray_cast_result.is_detected_voxel) return glm::ivec3(0);
    glm::ivec3 adjacent_voxel = ray_cast_result.detected_voxel_world_pos;

    switch (ray_cast_result.detected_face) {
        case North:
            adjacent_voxel.x += 1;
            break;
        case South:
            adjacent_voxel.x -= 1;
            break;
        case East:
            adjacent_voxel.z += 1;
            break;
        case West:
            adjacent_voxel.z -= 1;
            break;
        case Top:
            adjacent_voxel.y += 1;
            break;
        case Bottom:
            adjacent_voxel.z -= 1;
            break;
        default: break;
    }

    return adjacent_voxel;
}