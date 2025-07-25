#ifndef CONSTANT_HPP
#define CONSTANT_HPP

#include <glad.h>

namespace Constant {
    constexpr unsigned FPS = 60;
    constexpr float SCREEN_OCCUPATION_RATIO = 0.9f;

    constexpr unsigned CHUNK_SIZE = 16;
    constexpr unsigned CHUNK_HEIGHT = 32;
    constexpr unsigned CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
    constexpr unsigned CHUNK_VOLUME = CHUNK_AREA * CHUNK_HEIGHT;

    // for the sake of your computer, don't put it above 16
    constexpr unsigned LOAD_DISTANCE = 16;

    constexpr float MAX_RAY_LENGTH = 8.78f;
    constexpr unsigned MAX_VOXEL_TRAVERSED = 8;
    constexpr GLsizei MULTISAMPLE_LEVEL = 4;
}

#endif