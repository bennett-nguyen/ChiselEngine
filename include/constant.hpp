#ifndef CONSTANT_HPP
#define CONSTANT_HPP

#include <glad.h>
#include <string>

namespace Constant {
    const std::string VERSION = "v0.2.0-dev";
    constexpr float SCREEN_OCCUPATION_RATIO = 0.9f;

    constexpr unsigned CHUNK_SIZE = 31;
    constexpr unsigned CHUNK_HEIGHT = 31;
    constexpr unsigned CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
    constexpr unsigned CHUNK_VOLUME = CHUNK_AREA * CHUNK_HEIGHT;

    constexpr unsigned LOAD_DISTANCE = 12;

    constexpr float MAX_RAY_LENGTH = 8.78f;
    constexpr unsigned MAX_VOXEL_TRAVERSED = 8;
    constexpr GLsizei MULTISAMPLE_LEVEL = 4;
}

#endif