#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "camera.hpp"
#include "constant.hpp"
#include <SDL2/SDL.h>
#include <glm/vec3.hpp>
#include "chunk.hpp"
#include <cmath>
#include <glm/gtx/hash.hpp>

enum GameMode {
    Survival,
    Creative
};

class Player {
public:
    Camera m_camera;

    Player();

    void move(float delta_time);
    void switchGamemode(GameMode mode);
    void initCamera(float getWidthHeightRatio);

    glm::vec3 getPosition();
    void setPosition(glm::vec3 position);

private:
    glm::vec3 m_position;
    GameMode m_mode;
    bool m_is_falling = false;
    float m_y_velocity;
};

#endif