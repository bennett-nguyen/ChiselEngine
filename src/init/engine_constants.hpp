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
    constexpr unsigned CHUNKS_TO_BUILD_PER_FRAME = 3;
    constexpr unsigned CHUNKS_TO_REBUILD_PER_FRAME = 3;

    constexpr unsigned LOAD_DISTANCE = 12;
    constexpr float MAX_RAY_LENGTH = 8.78f;
    constexpr unsigned MAX_VOXEL_TRAVERSED = 8;
    constexpr GLsizei MULTISAMPLE_LEVEL = 3;
}

namespace chisel::ChunkDataConstants {
    // WARNING: Vertex x, y, z value range must match CHUNK_SIZE and CHUNK_HEIGHT
    constexpr unsigned  X_SIZE = 4,
                        Y_SIZE = 4,
                        Z_SIZE = 5,
                        AO_ID_SIZE = 2,
                        FACE_ID_SIZE = 3,
                        VOXEL_ID_SIZE = 8;

    constexpr unsigned CHUNK_SIZE = (1 << X_SIZE) - 1;
    constexpr unsigned CHUNK_HEIGHT = (1 << Z_SIZE) - 1;

    constexpr unsigned CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
    constexpr unsigned CHUNK_VOLUME = CHUNK_AREA * CHUNK_HEIGHT;
}

#endif
