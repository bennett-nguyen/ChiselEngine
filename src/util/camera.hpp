#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <cmath>
#include <vector>
#include <algorithm>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
    Camera(float fov_y, float near, float far, float width_height_ration);

    void move();
    void pan(const SDL_Event &event);
    void update_view();
    glm::mat4 look_at();
    glm::vec3 get_camera_position();
    glm::mat4 get_model_mat();
    glm::mat4 get_view_mat();
    glm::mat4 get_projection_mat();
    void set_model_mat(glm::mat4 new_model_mat);

private:
    glm::vec3 direction;
    glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 camera_up = glm::vec3(0.0f, 1.0f,  0.0f);

    float yaw = -90.0f;
    float pitch = 0.0f;
    const float camera_speed = 0.2f;
    const float sensitivity = 0.15f;

    glm::mat4 model_mat = glm::mat4(1.0f);
    glm::mat4 projection_mat;
    glm::mat4 view_mat = this->look_at();
};

#endif