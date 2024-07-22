#include "camera.hpp"
#include "constants.hpp"

Camera::Camera(float fov_y, float near, float far) {
    this->projection_mat = glm::perspective(fov_y, (float)Constant::WIDTH / (float)Constant::HEIGHT, near, far);
}

void Camera::update_view() {
    this->view_mat = this->look_at();
}

glm::mat4 Camera::look_at() {
    return glm::lookAt(this->camera_pos,
        this->camera_pos + this->camera_front, this->camera_up);
}

glm::mat4 Camera::get_model_mat() {
    return this->model_mat;
}

glm::mat4 Camera::get_view_mat() {
    return this->view_mat;
}

glm::mat4 Camera::get_projection_mat() {
    return this->projection_mat;
}

void Camera::move() {
    const Uint8 *kb_state = SDL_GetKeyboardState(NULL);

    if (kb_state[SDL_SCANCODE_D]) {
        this->camera_pos += glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
    } if (kb_state[SDL_SCANCODE_A]) {
        this->camera_pos -= glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
    } if (kb_state[SDL_SCANCODE_S]) {
        this->camera_pos -= camera_speed * camera_front;
    } if (kb_state[SDL_SCANCODE_W]) {
        this->camera_pos += camera_speed * camera_front;
    } 
}

void Camera::pan(const SDL_Event &event) {
    if (SDL_MOUSEMOTION != event.type || !SDL_GetRelativeMouseMode()) return;

    float xoffset = (float)event.motion.xrel;
    float yoffset = (float)event.motion.yrel;

    xoffset *= this->sensitivity;
    yoffset *= this->sensitivity;

    this->yaw   += xoffset;
    this->pitch += -yoffset; 

    this->pitch = std::min(this->pitch, 89.0f);
    this->pitch = std::max(this->pitch, -89.0f);

    this->direction.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    this->direction.y = sin(glm::radians(this->pitch));
    this->direction.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    this->camera_front = glm::normalize(this->direction);
}

glm::vec3 Camera::get_camera_position() {
    return this->camera_pos;
}