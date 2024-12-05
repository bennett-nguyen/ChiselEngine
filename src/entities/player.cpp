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
