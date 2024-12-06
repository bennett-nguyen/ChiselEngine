#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <cmath>
#include <vector>
#include <algorithm>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

extern const float SENSITIVITY;
extern const float CAMERA_SPEED;

class Camera {
public:
    Camera();
    Camera(float fov_y, float near, float far, float width_height_ratio);

    void updateView();
    void pan(const SDL_Event &event);
    void computeCameraFront();
    void computeCameraRight();
    void computeCameraUp();
    void setCameraPosition(glm::vec3 player_position);

    glm::mat4 lookAt();
    glm::mat4 getViewMat();
    glm::mat4 getProjectionMat();
    glm::vec3 getCameraFront();
    glm::vec3 getCameraUp();
    glm::vec3 getCameraRight();
    glm::vec3 getCameraPosition();
    std::string getXYZ_Directions();
    std::string getCardinalDirections();

private:
    glm::vec3 m_camera_pos;
    glm::vec3 m_camera_front;
    glm::vec3 m_camera_up;
    glm::vec3 m_camera_right;

    float m_yaw = 90.0f;
    float m_pitch = 0.0f;

    glm::mat4 m_projection_mat;
    glm::mat4 m_view_mat;
};

#endif