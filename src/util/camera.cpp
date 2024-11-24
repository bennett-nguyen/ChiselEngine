#include "camera.hpp"
#include "constant.hpp"

const float SENSITIVITY = 0.15f;
const float CAMERA_SPEED = 1.0f;

Camera::Camera(glm::vec3 position, float fov_y, float near, float far, float width_height_ratio) {
    m_camera_pos = position;
    m_projection_mat = glm::perspective(fov_y, width_height_ratio, near, far);
    this->compute_camera_front();
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

void Camera::move() {
    const Uint8 *kb_state = SDL_GetKeyboardState(NULL);

    if (kb_state[SDL_SCANCODE_D]) {
        m_camera_pos += glm::normalize(glm::cross(m_camera_front, m_camera_up)) * CAMERA_SPEED;
    } if (kb_state[SDL_SCANCODE_A]) {
        m_camera_pos -= glm::normalize(glm::cross(m_camera_front, m_camera_up)) * CAMERA_SPEED;
    } if (kb_state[SDL_SCANCODE_S]) {
        m_camera_pos -= CAMERA_SPEED * m_camera_front;
    } if (kb_state[SDL_SCANCODE_W]) {
        m_camera_pos += CAMERA_SPEED * m_camera_front;
    } if (kb_state[SDL_SCANCODE_LSHIFT] || kb_state[SDL_SCANCODE_RSHIFT]) {
        m_camera_pos.y -= CAMERA_SPEED;
    } if (kb_state[SDL_SCANCODE_SPACE]) {
        m_camera_pos.y +=  CAMERA_SPEED;
    }
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


void Camera::teleport(glm::vec3 position) {
    m_camera_pos = position;
}

glm::vec3 Camera::get_camera_front() {
    return m_camera_front;
}

glm::vec3 Camera::get_camera_position() {
    return m_camera_pos;
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