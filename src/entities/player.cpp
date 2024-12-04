#include "player.hpp"
#include <iostream>

Player::Player() 
    : m_position(glm::vec3(0, 51, 0)) {
}

void Player::switch_gamemode(GameMode mode) {
    m_mode = mode;
}

void Player::init_camera(float wh_ratio) {
    m_camera = Camera(glm::radians(Constant::FOV), 0.1f, 100.0f, wh_ratio);
    m_camera.set_camera_position(m_position);
}

void Player::move() {
    const Uint8 *kb_state = SDL_GetKeyboardState(NULL);

    glm::vec3 camera_front = m_camera.get_camera_front();
    glm::vec3 camera_up = m_camera.get_camera_up();
    glm::vec3 camera_right = glm::cross(camera_front, camera_up);

    glm::vec3 moving_direction(0.0f);

    if (kb_state[SDL_SCANCODE_W]) {
        moving_direction += camera_front;
    } if (kb_state[SDL_SCANCODE_S]) {
        moving_direction -= camera_front;
    } if (kb_state[SDL_SCANCODE_A]) {
        moving_direction -= camera_right;
    } if (kb_state[SDL_SCANCODE_D]) {
        moving_direction += camera_right;
    }
    
    moving_direction.y = 0.0f;

    if (kb_state[SDL_SCANCODE_SPACE]) {
        moving_direction.y += 1.0f;
    } if (kb_state[SDL_SCANCODE_LSHIFT]) {
        moving_direction.y -= 1.0f;
    }

    if (glm::vec3(0.0f) != moving_direction) {
        m_position += glm::normalize(moving_direction) * Constant::PLAYER_SPEED;
    }

    m_camera.set_camera_position(m_position);
}

glm::ivec3 Player::get_player_chunk_coords() {
    return glm::ivec3(
        int(m_position.x / (float)Constant::CHUNK_SIZE),
        int(m_position.y / (float)Constant::CHUNK_HEIGHT),
        int(m_position.z / (float)Constant::CHUNK_SIZE)
    );
}

void Player::set_position(glm::vec3 position) {
    m_position = position;
    m_camera.set_camera_position(position);
}

glm::vec3 Player::get_position() {
    return m_position;
}

float FRAC0(float x) {
    return x - std::floor(x);
}

float FRAC1(float x) {
    return 1.0 - x + std::floor(x);
}

std::pair<glm::ivec3, int> Player::ray_cast(std::unordered_map<glm::ivec3, Chunk*> m_chunk_map) {
    glm::vec3 camera_position = m_camera.get_camera_position();
    glm::vec3 ray_direction = m_camera.get_camera_front(); // this vector is normalized
    glm::ivec3 current_voxel = glm::ivec3(
        int(camera_position.x),
        int(camera_position.y),
        int(camera_position.z)
    );

    int step_x = glm::sign(ray_direction.x);
    int step_y = glm::sign(ray_direction.y);
    int step_z = glm::sign(ray_direction.z);

    float t_delta_x, t_delta_y, t_delta_z;
    float t_max_x, t_max_y, t_max_z;

    float current_distance = 0.0f;

    if (step_x == 0) {
        t_delta_x = 1e30;
    } else {
        t_delta_x = std::abs(std::min(1.0 / ray_direction.x, 1e30));

        if (step_x > 0) {
            t_max_x = FRAC1(camera_position.x) * t_delta_x;
        } else {
            t_max_x = FRAC0(camera_position.x) * t_delta_x;
        }
    }

    if (step_y == 0) {
        t_delta_y = 1e30;
    } else {
        t_delta_y = std::abs(std::min(1.0 / ray_direction.y, 1e30));

        if (step_y > 0) {
            t_max_y = FRAC1(camera_position.y) * t_delta_y;
        } else {
            t_max_y = FRAC0(camera_position.y) * t_delta_y;
        }
    }

    if (step_z == 0) {
        t_delta_z = 1e30;
    } else {
        t_delta_z = std::abs(std::min(1.0 / ray_direction.z, 1e30));

        if (step_z > 0) {
            t_max_z = FRAC1(camera_position.z) * t_delta_z;
        } else {
            t_max_z = FRAC0(camera_position.z) * t_delta_z;
        }
    }

    std::pair<glm::ivec3, int> voxel_info;
    for (int _ = 0; _ < Constant::MAX_RAY_LENGTH; _++) {
        if (t_max_x < t_max_y) {
            if (t_max_x < t_max_z) {
                t_max_x += t_delta_x;
                current_voxel.x += step_x;
            } else {
                t_max_z += t_delta_z;
                current_voxel.z += step_z;
            }
        } else {
            if (t_max_y < t_max_z) {
                t_max_y += t_delta_y;
                current_voxel.y += step_y;
            } else {
                t_max_z += t_delta_z;
                current_voxel.z += step_z;
            }
        }

        voxel_info = get_voxel_info(m_chunk_map, current_voxel);
        if (voxel_info.second != -1) {
            return voxel_info;
        }
    }

    return voxel_info;
}

std::pair<glm::ivec3, int> Player::get_voxel_info(std::unordered_map<glm::ivec3, Chunk*> m_chunk_map, glm::ivec3 voxel_pos) {
    glm::ivec3 current_player_chunk_coord = get_player_chunk_coords();

    glm::ivec3 voxel_chunk_coord(
        int(voxel_pos.x / (float)Constant::CHUNK_SIZE),
        int(voxel_pos.y / (float)Constant::CHUNK_HEIGHT),
        int(voxel_pos.z / (float)Constant::CHUNK_SIZE)
    );

    int local_x = voxel_pos.x - voxel_chunk_coord.x * Constant::CHUNK_SIZE;
    int local_y = voxel_pos.y - voxel_chunk_coord.y * Constant::CHUNK_HEIGHT;
    int local_z = voxel_pos.z - voxel_chunk_coord.z * Constant::CHUNK_SIZE;
    unsigned voxel_idx = get_voxel_idx(local_x, local_y, local_z);

    if (1 == m_chunk_map.count(voxel_chunk_coord) && !m_chunk_map[voxel_chunk_coord]->is_air_at(voxel_idx)) {
        return std::make_pair(voxel_chunk_coord, get_voxel_idx(local_x, local_y, local_z));
    }

    return std::make_pair(glm::ivec3(0.0f), -1);
}