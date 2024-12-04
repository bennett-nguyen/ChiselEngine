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

    void move();
    void switch_gamemode(GameMode mode);
    void init_camera(float wh_ratio);

    glm::vec3 get_position();
    void set_position(glm::vec3 position);

    glm::ivec3 get_player_chunk_coords();
    std::pair<glm::ivec3, int> ray_cast(std::unordered_map<glm::ivec3, Chunk*> m_chunk_map);
    std::pair<glm::ivec3, int> get_voxel_info(std::unordered_map<glm::ivec3, Chunk*> m_chunk_map, glm::ivec3 voxel_pos);

private:
    glm::vec3 m_position;
    GameMode m_mode;
    bool m_is_falling = false;
    float m_y_velocity;
};

#endif