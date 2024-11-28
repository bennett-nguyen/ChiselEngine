#include "camera.hpp"
#include "constant.hpp"
#include <iostream>

const float SENSITIVITY = 0.15f;
const float SPEED = 1.0f;

Camera::Camera() {}

Camera::Camera(float fov_y, float near, float far, float width_height_ratio) {
    m_projection_mat = glm::perspective(fov_y, width_height_ratio, near, far);
    this->compute_camera_front();
    update_view();
}

void Camera::update_view() {
    m_view_mat = this->look_at();
}

glm::mat4 Camera::look_at() {
    return glm::lookAt(m_camera_pos,
        m_camera_pos + m_camera_front, m_camera_up);
}

glm::mat4 Camera::get_view_mat() {
    return m_view_mat;
}

glm::mat4 Camera::get_projection_mat() {
    return m_projection_mat;
}

void Camera::pan(const SDL_Event &event) {
    if (SDL_MOUSEMOTION != event.type || !SDL_GetRelativeMouseMode()) return;
    float xoffset = (float)event.motion.xrel;
    float yoffset = (float)event.motion.yrel;

    xoffset *= SENSITIVITY;
    yoffset *= SENSITIVITY;

    m_yaw   += xoffset;
    m_pitch -= yoffset; 

    m_pitch = std::min(m_pitch, 89.0f);
    m_pitch = std::max(m_pitch, -89.0f);
    this->compute_camera_front();
}

void Camera::compute_camera_front() {
    m_direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_direction.y = sin(glm::radians(m_pitch));
    m_direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_camera_front = glm::normalize(m_direction);
}

void Camera::set_camera_position(glm::vec3 player_position) {
    m_camera_pos = player_position;
    m_camera_pos.y += Constant::PLAYER_HEIGHT;
}

glm::vec3 Camera::get_camera_front() {
    return m_camera_front;
}

glm::vec3 Camera::get_camera_up() {
    return m_camera_up;
}

std::string Camera::get_cardinal_directions() {
    std::string x_direction = m_camera_front.x >= 0 ? "North" : "South";
    std::string y_direction = m_camera_front.y >= 0 ? "Up" : "Down";
    std::string z_direction = m_camera_front.z >= 0 ? "East" : "West";

    return "(" + x_direction + ", " + z_direction + ", " + y_direction + ")";
}

std::string Camera::get_xyz_directions() {
    std::string x_direction = m_camera_front.x >= 0 ? "X+" : "X-";
    std::string y_direction = m_camera_front.y >= 0 ? "Y+" : "Y-";
    std::string z_direction = m_camera_front.z >= 0 ? "Z+" : "Z-";

    return "(" + x_direction + ", " + z_direction + ", " + y_direction + ")";
}