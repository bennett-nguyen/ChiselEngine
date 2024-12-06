#include "player.hpp"
#include <iostream>

Player::Player() 
    : m_position(glm::vec3(0, 51, 0)) {
}

void Player::switchGamemode(GameMode mode) {
    m_mode = mode;
}

void Player::initCamera(float getWidthHeightRatio) {
    m_camera = Camera(glm::radians(Constant::FOV), 0.1f, 150.0f, getWidthHeightRatio);
    m_camera.setCameraPosition(m_position);
}

void Player::move() {
    const Uint8 *kb_state = SDL_GetKeyboardState(NULL);

    glm::vec3 camera_front = m_camera.getCameraFront();
    glm::vec3 camera_right = m_camera.getCameraRight();

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

    m_camera.setCameraPosition(m_position);
}

void Player::setPosition(glm::vec3 position) {
    m_position = position;
    m_camera.setCameraPosition(position);
}

glm::vec3 Player::getPosition() {
    return m_position;
}
