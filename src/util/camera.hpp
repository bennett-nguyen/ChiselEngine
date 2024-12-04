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

    void update_view();
    void pan(const SDL_Event &event);
    void compute_camera_front();
    void set_camera_position(glm::vec3 player_position);

    glm::mat4 look_at();
    glm::mat4 get_view_mat();
    glm::mat4 get_projection_mat();
    glm::vec3 get_camera_front();
    glm::vec3 get_camera_up();
    glm::vec3 get_camera_position();
    std::string get_xyz_directions();
    std::string get_cardinal_directions();

private:
    glm::vec3 m_camera_pos;
    glm::vec3 m_camera_front;
    glm::vec3 m_camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 m_camera_right;

    float m_yaw = 90.0f;
    float m_pitch = 0.0f;

    glm::mat4 m_projection_mat;
    glm::mat4 m_view_mat;
};

#endif