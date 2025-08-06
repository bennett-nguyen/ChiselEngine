#include "camera.hpp"

constexpr float SENSITIVITY = 0.2f;

Camera::Camera(const float fov_y, const float aspect, const float near, const float far) {
    projection = glm::perspective(fov_y, aspect, near, far);
    computeAxes();
    updateView();
}

void Camera::pan(const SDL_Event &event) {
    if (SDL_MOUSEMOTION != event.type or not SDL_GetRelativeMouseMode()) return;
    auto x_offset = static_cast<float>(event.motion.xrel);
    auto y_offset = static_cast<float>(event.motion.yrel);

    x_offset *= SENSITIVITY;
    y_offset *= SENSITIVITY;

    yaw   += x_offset;
    pitch -= y_offset;

    pitch = std::min(pitch, 89.0f);
    pitch = std::max(pitch, -89.0f);
    computeAxes();
}

void Camera::computeAxes() {
    computeFront();
    computeRight();
    computeUp();
}

void Camera::updateView() {
    view = glm::lookAt(position, position + front, { 0.0f, 1.0f, 0.0f });
}

void Camera::computeFront() {
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front   = glm::normalize(front);
}

void Camera::computeRight() {
    right = glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::computeUp() {
    up = glm::cross(front, right);
}

void Camera::move(const float delta_time) {
    const Uint8 *kb_state = SDL_GetKeyboardState(nullptr);

    glm::vec3 moving_direction(0.0f);

    if (kb_state[SDL_SCANCODE_W]) {
        moving_direction += front;
    } if (kb_state[SDL_SCANCODE_S]) {
        moving_direction -= front;
    } if (kb_state[SDL_SCANCODE_A]) {
        moving_direction -= right;
    } if (kb_state[SDL_SCANCODE_D]) {
        moving_direction += right;
    }
    
    moving_direction.y = 0.0f;

    if (kb_state[SDL_SCANCODE_SPACE]) {
        moving_direction.y += 1.0f;
    } if (kb_state[SDL_SCANCODE_LSHIFT]) {
        moving_direction.y -= 1.0f;
    }

    if (glm::vec3(0.0f) != moving_direction) {
        position += glm::normalize(moving_direction) * 35.0f * delta_time;
    }
}

void Camera::setPosition(const glm::vec3 any_position) {
    position = any_position;
}

glm::vec3 Camera::getPosition() const {
    return position;
}

glm::vec3 Camera::getViewingDirection() const {
    return front;
}

glm::mat4 Camera::getView() const {
    return view;
}

glm::mat4 Camera::getProjection() const {
    return projection;
}

std::string Camera::getCardinalDirection() const {
    const std::string x_direction = front.x >= 0 ? "North" : "South";
    const std::string y_direction = front.y >= 0 ? "Up" : "Down";
    const std::string z_direction = front.z >= 0 ? "East" : "West";

    return "(" + x_direction + ", " + z_direction + ", " + y_direction + ")";
}

std::array<glm::vec4, 6> Camera::getFrustumPlanes() const {
    const glm::mat4 vpt = glm::transpose(projection * view);
    return {{
        // left, right, bottom, top
        (vpt[3] + vpt[0]),
        (vpt[3] - vpt[0]),
        (vpt[3] + vpt[1]),
        (vpt[3] - vpt[1]),
        // near, far
        (vpt[3] + vpt[2]),
        (vpt[3] - vpt[2]),
    }};
}
