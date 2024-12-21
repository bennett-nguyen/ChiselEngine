#include "camera.hpp"

const float SENSITIVITY = 0.3f;

Camera::Camera() {}

Camera::Camera(float fov_y, float near, float far, float width_height_ratio) {
    m_projection_mat = glm::perspective(fov_y, width_height_ratio, near, far);
    computeCameraFront();
    updateView();
}

void Camera::updateView() {
    m_view_mat = this->lookAt();
}

glm::mat4 Camera::lookAt() {
    return glm::lookAt(m_camera_pos,
        m_camera_pos + m_camera_front, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 Camera::getViewMat() {
    return m_view_mat;
}

glm::mat4 Camera::getProjectionMat() {
    return m_projection_mat;
}

void Camera::pan(const SDL_Event &event, float delta_time) {
    if (SDL_MOUSEMOTION != event.type || !SDL_GetRelativeMouseMode()) return;
    float xoffset = (float)event.motion.xrel;
    float yoffset = (float)event.motion.yrel;

    xoffset *= SENSITIVITY;
    yoffset *= SENSITIVITY;

    m_yaw   += xoffset;
    m_pitch -= yoffset; 

    m_pitch = std::min(m_pitch, 89.0f);
    m_pitch = std::max(m_pitch, -89.0f);
    computeCameraFront();
    computeCameraRight();
    computeCameraUp();
}

void Camera::computeCameraFront() {
    m_camera_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_camera_front.y = sin(glm::radians(m_pitch));
    m_camera_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_camera_front = glm::normalize(m_camera_front);
}

void Camera::computeCameraRight() {
    m_camera_right = glm::cross(m_camera_front, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::computeCameraUp() {
    m_camera_up = glm::cross(m_camera_front, m_camera_right);
}

glm::vec3 Camera::getCameraPosition() {
    return m_camera_pos;
}
void Camera::setCameraPosition(glm::vec3 player_position) {
    m_camera_pos = player_position;
    m_camera_pos.y += Constant::PLAYER_HEIGHT;
}

glm::vec3 Camera::getCameraFront() {
    return m_camera_front;
}

glm::vec3 Camera::getCameraUp() {
    return m_camera_up;
}

glm::vec3 Camera::getCameraRight() {
    return m_camera_right;
}

std::string Camera::getCardinalDirections() {
    std::string x_direction = m_camera_front.x >= 0 ? "North" : "South";
    std::string y_direction = m_camera_front.y >= 0 ? "Up" : "Down";
    std::string z_direction = m_camera_front.z >= 0 ? "East" : "West";

    return "(" + x_direction + ", " + z_direction + ", " + y_direction + ")";
}

std::string Camera::getXYZ_Directions() {
    std::string x_direction = m_camera_front.x >= 0 ? "X+" : "X-";
    std::string y_direction = m_camera_front.y >= 0 ? "Y+" : "Y-";
    std::string z_direction = m_camera_front.z >= 0 ? "Z+" : "Z-";

    return "(" + x_direction + ", " + z_direction + ", " + y_direction + ")";
}