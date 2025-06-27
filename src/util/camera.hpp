#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <string>
#include <vector>

#include <SDL2/SDL.h>

#include <glm/glm.hpp>
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
void move(Camera &camera);

std::vector<glm::vec4> getFrustumPlanes(Camera &camera);
std::string getCardinalDirections(const Camera &camera);

#endif