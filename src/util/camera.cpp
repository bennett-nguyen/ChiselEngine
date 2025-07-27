#include "camera.hpp"

constexpr float SENSITIVITY = 0.3f;

void computeCameraUp(Camera &camera) {
    camera.up = glm::cross(camera.front, camera.right);
}

void computeCameraRight(Camera &camera) {
    camera.right = glm::cross(camera.front, glm::vec3(0.0f, 1.0f, 0.0f));
}

void computeCameraFront(Camera &camera) {
    camera.front.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    camera.front.y = sin(glm::radians(camera.pitch));
    camera.front.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    camera.front   = glm::normalize(camera.front);
}

void updateView(Camera &camera) {
    camera.view_mat = glm::lookAt(camera.position,
        camera.position + camera.front, glm::vec3(0.0f, 1.0f, 0.0f));
}

Camera initCamera(const float fov_y, const float near, const float far, const float aspect_ratio) {
    Camera camera;

    camera.projection_mat = glm::perspective(fov_y, aspect_ratio, near, far);
    camera.position = glm::vec3(0);
    computeCameraFront(camera);
    updateView(camera);

    return camera;
}

void pan(Camera &camera, const SDL_Event &event) {
    if (SDL_MOUSEMOTION != event.type or not SDL_GetRelativeMouseMode()) return;
    auto xoffset = static_cast<float>(event.motion.xrel);
    auto yoffset = static_cast<float>(event.motion.yrel);

    xoffset *= SENSITIVITY;
    yoffset *= SENSITIVITY;

    camera.yaw   += xoffset;
    camera.pitch -= yoffset; 

    camera.pitch = std::min(camera.pitch, 89.0f);
    camera.pitch = std::max(camera.pitch, -89.0f);
    computeCameraFront(camera);
    computeCameraRight(camera);
    computeCameraUp(camera);
}

void move(Camera &camera) {
    const Uint8 *kb_state = SDL_GetKeyboardState(nullptr);

    glm::vec3 moving_direction(0.0f);

    if (kb_state[SDL_SCANCODE_W]) {
        moving_direction += camera.front;
    } if (kb_state[SDL_SCANCODE_S]) {
        moving_direction -= camera.front;
    } if (kb_state[SDL_SCANCODE_A]) {
        moving_direction -= camera.right;
    } if (kb_state[SDL_SCANCODE_D]) {
        moving_direction += camera.right;
    }
    
    moving_direction.y = 0.0f;

    if (kb_state[SDL_SCANCODE_SPACE]) {
        moving_direction.y += 1.0f;
    } if (kb_state[SDL_SCANCODE_LSHIFT]) {
        moving_direction.y -= 1.0f;
    }

    if (glm::vec3(0.0f) != moving_direction) {
        camera.position += glm::normalize(moving_direction) * 0.7f;
    }
}

std::vector<glm::vec4> getFrustumPlanes(const Camera &camera) {
    const glm::mat4 vpt = glm::transpose(camera.projection_mat * camera.view_mat);
    return {
        // left, right, bottom, top
        (vpt[3] + vpt[0]),
        (vpt[3] - vpt[0]),
        (vpt[3] + vpt[1]),
        (vpt[3] - vpt[1]),
        // near, far
        (vpt[3] + vpt[2]),
        (vpt[3] - vpt[2]),
    };
}

std::string getCardinalDirections(const Camera &camera) {
    const std::string x_direction = camera.front.x >= 0 ? "North" : "South";
    const std::string y_direction = camera.front.y >= 0 ? "Up" : "Down";
    const std::string z_direction = camera.front.z >= 0 ? "East" : "West";

    return "(" + x_direction + ", " + z_direction + ", " + y_direction + ")";
}