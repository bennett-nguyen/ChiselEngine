#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <array>
#include <string>

#include <SDL2/SDL.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera {
    glm::vec3 up;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 position;

    float yaw = 90.0f;
    float pitch = 0.0f;

    glm::mat4 projection_mat;
    glm::mat4 view_mat;
};

Camera initCamera(float fov_y, float near, float far, float aspect_ratio);

void computeCameraUp(Camera &camera);
void computeCameraFront(Camera &camera);
void computeCameraRight(Camera &camera);
void updateView(Camera &camera);
void pan(Camera &camera, const SDL_Event &event);
void move(Camera &camera, float delta_time);

[[nodiscard]] std::string getCardinalDirections(const Camera &camera);
[[nodiscard]] std::array<glm::vec4, 6> getFrustumPlanes(const Camera &camera);

#endif
