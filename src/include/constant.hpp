#ifndef CONSTANT_HPP
#define CONSTANT_HPP

namespace Constant {
    constexpr unsigned FPS = 60;
    constexpr float SCREEN_OCCUPATION_RATIO = 0.8f;

    constexpr unsigned CHUNK_SIZE = 16;
    constexpr unsigned CHUNK_HEIGHT = 50;
    constexpr unsigned CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
    constexpr unsigned CHUNK_VOLUME = CHUNK_AREA * CHUNK_HEIGHT;

    constexpr unsigned LOAD_DISTANCE = 5;
}

#endif