#ifndef DIRECTION_HPP
#define DIRECTION_HPP

#include <array>
#include <unordered_map>

#include "conversions.hpp"

enum class Direction : unsigned {
    Nil     = 0,
    Top     = 0b000001, // Y+
    Bottom  = 0b000010, // Y-
    North   = 0b000100, // X+
    South   = 0b001000, // X-
    East    = 0b010000, // Z+
    West    = 0b100000, // Z-
};

constexpr Direction operator |(Direction a, Direction b) {
    return static_cast<Direction>(static_cast<unsigned>(a) | static_cast<unsigned>(b));
}

constexpr Direction operator &(Direction a, Direction b) {
    return static_cast<Direction>(static_cast<unsigned>(a) & static_cast<unsigned>(b));
}

const std::unordered_map<Direction, unsigned> FACE_DIRECTION_TO_ID {
    { Direction::Top,    0 },
    { Direction::Bottom, 1 },
    { Direction::North,  2 },
    { Direction::South,  3 },
    { Direction::East,   4 },
    { Direction::West,   5 },
};

const std::unordered_map<Direction, std::array<LocalPosition, 4>> FACE_VERTICES {
    {
        Direction::Top, {{
            LocalPosition(0, 1, 0),
            LocalPosition(1, 1, 0),
            LocalPosition(1, 1, 1),
            LocalPosition(0, 1, 1)
        }}
    },

    {
        Direction::Bottom, {{
            LocalPosition(0, 0, 0),
            LocalPosition(1, 0, 0),
            LocalPosition(1, 0, 1),
            LocalPosition(0, 0, 1)
        }}
    },

    {
        Direction::North, {{
            LocalPosition(1, 0, 0),
            LocalPosition(1, 1, 0),
            LocalPosition(1, 1, 1),
            LocalPosition(1, 0, 1)
        }}
    },

    {
        Direction::South, {{
            LocalPosition(0, 0, 0),
            LocalPosition(0, 1, 0),
            LocalPosition(0, 1, 1),
            LocalPosition(0, 0, 1)
        }}
    },

    {
        Direction::East, {{
            LocalPosition(0, 0, 1),
            LocalPosition(0, 1, 1),
            LocalPosition(1, 1, 1),
            LocalPosition(1, 0, 1)
        }}
    },

    {
        Direction::West, {{
            LocalPosition(0, 0, 0),
            LocalPosition(0, 1, 0),
            LocalPosition(1, 1, 0),
            LocalPosition(1, 0, 0)
        }}
    }
};

const std::unordered_map<Direction, ChunkPosition> CHUNK_NEIGHBORS_DIRECTION {
    { Direction::Top,    ChunkPosition(0,  1, 0) },
    { Direction::Bottom, ChunkPosition(0, -1, 0) },
    { Direction::North,  ChunkPosition( 1, 0, 0) },
    { Direction::South,  ChunkPosition(-1, 0, 0) },
    { Direction::East,   ChunkPosition( 0, 0, 1) },
    { Direction::West,   ChunkPosition( 0, 0,-1) },

    { Direction::North | Direction::East, ChunkPosition( 1, 0,  1) },
    { Direction::North | Direction::West, ChunkPosition( 1, 0, -1) },
    { Direction::South | Direction::East, ChunkPosition(-1, 0,  1) },
    { Direction::South | Direction::West, ChunkPosition(-1, 0, -1) },

    { Direction::Top | Direction::North, ChunkPosition( 1, 1,  0) },
    { Direction::Top | Direction::South, ChunkPosition(-1, 1,  0) },
    { Direction::Top | Direction::East,  ChunkPosition( 0, 1,  1) },
    { Direction::Top | Direction::West,  ChunkPosition( 0, 1, -1) },

    { Direction::Bottom | Direction::North, ChunkPosition(1, -1, 0) },
    { Direction::Bottom | Direction::South, ChunkPosition(-1, -1, 0) },
    { Direction::Bottom | Direction::East, ChunkPosition(0, -1, 1) },
    { Direction::Bottom | Direction::West, ChunkPosition(0, -1, -1) },

    { Direction::Top | Direction::North | Direction::East, ChunkPosition( 1, 1,  1) },
    { Direction::Top | Direction::North | Direction::West, ChunkPosition( 1, 1, -1) },
    { Direction::Top | Direction::South | Direction::East, ChunkPosition(-1, 1,  1) },
    { Direction::Top | Direction::South | Direction::West, ChunkPosition(-1, 1, -1) },

    { Direction::Bottom | Direction::North | Direction::East, ChunkPosition( 1, -1,  1) },
    { Direction::Bottom | Direction::North | Direction::West, ChunkPosition (1, -1, -1) },
    { Direction::Bottom | Direction::South | Direction::East, ChunkPosition(-1, -1,  1) },
    { Direction::Bottom | Direction::South | Direction::West, ChunkPosition(-1, -1, -1) },
};//

const std::unordered_map<Direction, WorldPosition> WORLD_DIRECTIONS {
    { Direction::Top,    WorldPosition(0,  1, 0) },
    { Direction::Bottom, WorldPosition(0, -1, 0) },
    { Direction::North,  WorldPosition( 1, 0, 0) },
    { Direction::South,  WorldPosition(-1, 0, 0) },
    { Direction::East,   WorldPosition( 0, 0, 1) },
    { Direction::West,   WorldPosition( 0, 0,-1) },

    { Direction::North | Direction::East, WorldPosition( 1, 0,  1) },
    { Direction::North | Direction::West, WorldPosition( 1, 0, -1) },
    { Direction::South | Direction::East, WorldPosition(-1, 0,  1) },
    { Direction::South | Direction::West, WorldPosition(-1, 0, -1) },

    { Direction::Top | Direction::North, WorldPosition( 1, 1,  0) },
    { Direction::Top | Direction::South, WorldPosition(-1, 1,  0) },
    { Direction::Top | Direction::East,  WorldPosition( 0, 1,  1) },
    { Direction::Top | Direction::West,  WorldPosition( 0, 1, -1) },

    { Direction::Bottom | Direction::North, WorldPosition(1, -1, 0) },
    { Direction::Bottom | Direction::South, WorldPosition(-1, -1, 0) },
    { Direction::Bottom | Direction::East,  WorldPosition(0, -1, 1) },
    { Direction::Bottom | Direction::West,  WorldPosition(0, -1, -1) },

    { Direction::Top | Direction::North | Direction::East, WorldPosition( 1, 1,  1) },
    { Direction::Top | Direction::North | Direction::West, WorldPosition( 1, 1, -1) },
    { Direction::Top | Direction::South | Direction::East, WorldPosition(-1, 1,  1) },
    { Direction::Top | Direction::South | Direction::West, WorldPosition(-1, 1, -1) },

    { Direction::Bottom | Direction::North | Direction::East, WorldPosition( 1, -1,  1) },
    { Direction::Bottom | Direction::North | Direction::West, WorldPosition (1, -1, -1) },
    { Direction::Bottom | Direction::South | Direction::East, WorldPosition(-1, -1,  1) },
    { Direction::Bottom | Direction::South | Direction::West, WorldPosition(-1, -1, -1) },
};

#endif
