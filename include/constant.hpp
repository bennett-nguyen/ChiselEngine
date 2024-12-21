#ifndef CONSTANT_HPP
#define CONSTANT_HPP

namespace Constant {
    constexpr unsigned FPS = 60;
    constexpr float SCREEN_OCCUPATION_RATIO = 0.9f;

    constexpr unsigned CHUNK_SIZE = 16;
    constexpr unsigned CHUNK_HEIGHT = 50;
    constexpr unsigned CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
    constexpr unsigned CHUNK_VOLUME = CHUNK_AREA * CHUNK_HEIGHT;
    constexpr unsigned LOAD_DISTANCE = 10;

    constexpr float PLAYER_HEIGHT = 1.5f;
    constexpr float PLAYER_SPEED = 30.0f;
    constexpr float FOV = 60.0f; // in degrees, CONVERT TO RADIANS BEFORE USING!
    constexpr float GRAVITATIONAL_ACCELERATION = 0.3f;
    constexpr float JUMP_VELOCITY = 2.0f;
    constexpr unsigned MAX_VOXEL_TRAVERSED = 8;
    constexpr float MAX_RAY_LENGTH = 8.78f;
    constexpr float CUBE_MESH_SCALE = 1.0f / 256.0f;
}

#endif