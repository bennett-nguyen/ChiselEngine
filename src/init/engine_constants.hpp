#ifndef ENGINE_CONSTANTS_HPP
#define ENGINE_CONSTANTS_HPP

#include <string>

#include <glad/gl.h>


namespace chisel::EngineConstants {
    constexpr std::string_view ENGINE_VERSION = "v0.2.0-dev";
    constexpr float SCREEN_OCCUPATION_RATIO = 0.8f;

    #ifdef NDEBUG
    constexpr bool IS_DEBUGGING_ENABLED = false;
    constexpr std::string_view ENGINE_BUILD_TYPE = "Release";
    #else
    constexpr bool IS_DEBUGGING_ENABLED = true;
    constexpr std::string_view ENGINE_BUILD_TYPE = "Debug";
    #endif

    constexpr unsigned CHUNK_SIZE = 31;
    constexpr unsigned CHUNK_HEIGHT = 31;
    constexpr unsigned CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
    constexpr unsigned CHUNK_VOLUME = CHUNK_AREA * CHUNK_HEIGHT;

    constexpr unsigned LOAD_DISTANCE = 12;

    constexpr float MAX_RAY_LENGTH = 8.78f;
    constexpr unsigned MAX_VOXEL_TRAVERSED = 8;
    constexpr GLsizei MULTISAMPLE_LEVEL = 3;
}


#endif
