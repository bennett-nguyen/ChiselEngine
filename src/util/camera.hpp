#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <array>
#include <string>

#include <SDL2/SDL.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
    glm::vec3 front {};
    glm::vec3 right {};
    glm::vec3 up {};
    glm::vec3 position {};

    float yaw = 90.0f;
    float pitch = 0.0f;

    glm::mat4 projection {};
    glm::mat4 view {};

    void computeFront();
    void computeRight();
    void computeUp();
    void computeAxes();
public:
    Camera(float fov_y, float aspect, float near, float far);
    ~Camera() = default;
    Camera(const Camera&) = default;

    void pan(const SDL_Event &event);
    void move(float delta_time);
    void updateView();

    void setPosition(glm::vec3 any_position);

    [[nodiscard]] glm::vec3 getPosition() const;
    [[nodiscard]] glm::vec3 getViewingDirection() const;

    [[nodiscard]] glm::mat4 getView() const;
    [[nodiscard]] glm::mat4 getProjection() const;

    [[nodiscard]] std::string getCardinalDirection() const;
    [[nodiscard]] std::array<glm::vec4, 6> getFrustumPlanes() const;
};

#endif
