#include "ray_casting.hpp"

float FRAC0(const float x) {
    return x - std::floor(x);
}

float FRAC1(const float x) {
    return 1.0f - x + std::floor(x);
}

void rayCast(RayCastResult &ray_cast_result, glm::vec3 position, glm::vec3 direction) {
    auto current_voxel = Conversion::toWorld(position);
    glm::vec3 normalized_direction = glm::normalize(direction);

    ray_cast_result.is_detected_voxel = false;

    const auto step_x = static_cast<int>(glm::sign(normalized_direction.x));
    const auto step_y = static_cast<int>(glm::sign(normalized_direction.y));
    const auto step_z = static_cast<int>(glm::sign(normalized_direction.z));
    auto face_x = Direction::Nil, face_y = Direction::Nil, face_z = Direction::Nil;

    float t_delta_x = 0.0f, t_delta_y = 0.0f, t_delta_z = 0.0f;
    float t_max_x = 0.0f, t_max_y = 0.0f, t_max_z = 0.0f;

    LocalPosition voxel_origin;
    ChunkPosition chunk_position_of_voxel;

    if (step_x == 0) {
        t_delta_x = 10000000.0f;
    } else {
        face_x = step_x > 0 ? Direction::South : Direction::North;
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
        face_y = step_y > 0 ? Direction::Bottom : Direction::Top;
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
        face_z = step_z > 0 ? Direction::West : Direction::East;
        t_delta_z = std::abs(std::min(static_cast<float>(1.0 / normalized_direction.z), 10000000.0f));

        if (step_z > 0) {
            t_max_z = FRAC1(position.z) * t_delta_z;
        } else {
            t_max_z = FRAC0(position.z) * t_delta_z;
        }
    }

    unsigned voxel_traversed = 0;
    float current_ray_length = 0.0f;

    while (voxel_traversed <= Constant::MAX_VOXEL_TRAVERSED or current_ray_length <= Constant::MAX_RAY_LENGTH) {
        chunk_position_of_voxel = Conversion::toChunk(current_voxel);

        if (ChunkPool::isChunkUsed(chunk_position_of_voxel)) {
            voxel_origin = Conversion::toLocal(current_voxel, chunk_position_of_voxel);

            if (not ChunkPool::isVoidAtInChunk(voxel_origin, chunk_position_of_voxel)) {
                ray_cast_result.is_detected_voxel = true;
                ray_cast_result.detected_voxel_position = current_voxel;
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

WorldPosition getAdjacentVoxel(const RayCastResult &ray_cast_result) {
    if (not ray_cast_result.is_detected_voxel) return WorldPosition(0);
    const WorldPosition adjacent_voxel = ray_cast_result.detected_voxel_position + DIRECTION_VECTORS.at(ray_cast_result.detected_face);
    return adjacent_voxel;
}

void breakBlock(const WorldPosition voxel_position) {
    const auto chunk_position = Conversion::toChunk(voxel_position);
    const auto local_position = Conversion::toLocal(voxel_position, chunk_position);

    ChunkPool::setVoxelIDAtPositionInChunk(0, local_position, chunk_position);
    ChunkPool::rebuild(chunk_position);

    if (isVoxelAtChunkBoundarySouth(local_position)) {
        ChunkPool::rebuild(chunk_position + DIRECTION_VECTORS.at(Direction::South));
    } else if (isVoxelAtChunkBoundaryNorth(local_position)) {
        ChunkPool::rebuild(chunk_position + DIRECTION_VECTORS.at(Direction::North));
    }

    if (isVoxelAtChunkBoundaryBottom(local_position)) {
        ChunkPool::rebuild(chunk_position + DIRECTION_VECTORS.at(Direction::Bottom));
    } else if (isVoxelAtChunkBoundaryTop(local_position)) {
        ChunkPool::rebuild(chunk_position + DIRECTION_VECTORS.at(Direction::Top));
    }

    if (isVoxelAtChunkBoundaryWest(local_position)) {
        ChunkPool::rebuild(chunk_position + DIRECTION_VECTORS.at(Direction::West));
    } else if (isVoxelAtChunkBoundaryEast(local_position)) {
        ChunkPool::rebuild(chunk_position + DIRECTION_VECTORS.at(Direction::East));
    }
}

void placeBlock(const WorldPosition adjacent_voxel_position) {
    const auto chunk_position = Conversion::toChunk(adjacent_voxel_position);
    const auto local_position = Conversion::toLocal(adjacent_voxel_position, chunk_position);

    if (chunk_position.y != 0) return;

    ChunkPool::setVoxelIDAtPositionInChunk(1, local_position, chunk_position);
    ChunkPool::rebuild(chunk_position);

    if (isVoxelAtChunkBoundarySouth(local_position)) {
        ChunkPool::rebuild(chunk_position + DIRECTION_VECTORS.at(Direction::South));
    } else if (isVoxelAtChunkBoundaryNorth(local_position)) {
        ChunkPool::rebuild(chunk_position + DIRECTION_VECTORS.at(Direction::North));
    }

    if (isVoxelAtChunkBoundaryBottom(local_position)) {
        ChunkPool::rebuild(chunk_position + DIRECTION_VECTORS.at(Direction::Bottom));
    } else if (isVoxelAtChunkBoundaryTop(local_position)) {
        ChunkPool::rebuild(chunk_position + DIRECTION_VECTORS.at(Direction::Top));
    }

    if (isVoxelAtChunkBoundaryWest(local_position)) {
        ChunkPool::rebuild(chunk_position + DIRECTION_VECTORS.at(Direction::West));
    } else if (isVoxelAtChunkBoundaryEast(local_position)) {
        ChunkPool::rebuild(chunk_position + DIRECTION_VECTORS.at(Direction::East));
    }
}