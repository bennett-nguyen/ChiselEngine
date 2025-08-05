#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <array>
#include <vector>
#include <tuple>
#include <algorithm>
#include <ostream>
#include <iostream>
#include <algorithm>

#include <glad.h>
#include <unordered_map>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/extended_min_max.hpp>

#include "constant.hpp"
#include "conversions.hpp"

/*
 * packed_data (32-bit):
 * Data           Size            Value Range (inclusive)     Constant name that holds such size
 * x              4 bit           0..15                       X_SIZE
 * y              5 bit           0..31                       Y_SIZE
 * z              4 bit           0..15                       Z_SIZE
 * ao_id          2 bit           0..3                        AO_ID_SIZE
 * face_id        3 bit           0..6                        FACE_ID_SIZE
 * voxel_id       8 bit           0..255                      VOXEL_ID_SIZE
 */

class Chunk;

typedef uint8_t VoxelID;

enum class Direction : unsigned {
    Nil     = 0,
    Top     = 0b000001, // Y+
    Bottom  = 0b000010, // Y-
    North   = 0b000100, // X+
    South   = 0b001000, // X-
    East    = 0b010000, // Z+
    West    = 0b100000, // Z-

    faceBegin = Top,
    faceEnd   = West,
};

constexpr Direction operator |(const Direction a, const Direction b) {
    return static_cast<Direction>(static_cast<unsigned>(a) | static_cast<unsigned>(b));
}

constexpr Direction& operator |=(Direction& a, const Direction b) {
    return a = a | b;
}

const std::unordered_map<Direction, unsigned> FACE_ID {
    { Direction::Top,    0 },
    { Direction::Bottom, 1 },
    { Direction::North,  2 },
    { Direction::South,  3 },
    { Direction::East,   4 },
    { Direction::West,   5 },
};

const std::unordered_map<Direction, ChunkPosition> DIRECTION_VECTORS {
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
};

[[nodiscard]] inline bool isVoxelAtChunkBoundaryEast(const LocalPosition local) {
    return Constant::CHUNK_SIZE-1 == local.z;
}

[[nodiscard]] inline bool isVoxelAtChunkBoundaryWest(const LocalPosition local) {
    return 0 == local.z;
}

[[nodiscard]] inline bool isVoxelAtChunkBoundaryNorth(const LocalPosition local) {
    return Constant::CHUNK_SIZE-1 == local.x;
}

[[nodiscard]] inline bool isVoxelAtChunkBoundarySouth(const LocalPosition local) {
    return 0 == local.x;
}

[[nodiscard]] inline bool isVoxelAtChunkBoundaryTop(const LocalPosition local) {
    return Constant::CHUNK_HEIGHT-1 == local.y;
}

[[nodiscard]] inline bool isVoxelAtChunkBoundaryBottom(const LocalPosition local) {
    return 0 == local.y;
}

struct Vertex {
    GLuint packed_data;

    Vertex() = default;
    Vertex(int vertex_index, const LocalPosition &voxel_origin, unsigned ao_id, Direction face_direction, VoxelID voxel_id);
};

struct ChunkMesh {
    GLuint ssbo_vertices {}, ebo {}, vao {};

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
};

struct AABB {
    glm::vec3 vmin {}, vmax {};

    void reset();
    void translate(ChunkPosition);
    void updateWithCubeFace(Direction face, LocalPosition voxel_origin);
};

struct ChunkNeighbors {
    const Chunk* north = nullptr;
    const Chunk* south = nullptr;
    const Chunk* east = nullptr;
    const Chunk* west = nullptr;

    const Chunk* north_east = nullptr;
    const Chunk* north_west = nullptr;
    const Chunk* south_east = nullptr;
    const Chunk* south_west = nullptr;
};

class Chunk {
    ChunkMesh mesh;
    AABB bounding_box {};
    ChunkPosition position {};
    ChunkNeighbors neighbors {};

    std::array<VoxelID, Constant::CHUNK_VOLUME> voxel_ids {};

    bool is_empty = true;
    bool is_built = false;

    // kill me
    [[nodiscard]] bool isVoidEastNeighbor(LocalPosition) const;
    [[nodiscard]] bool isVoidWestNeighbor(LocalPosition) const;
    [[nodiscard]] bool isVoidNorthNeighbor(LocalPosition) const;
    [[nodiscard]] bool isVoidSouthNeighbor(LocalPosition) const;
    [[nodiscard]] bool isVoidTopNeighbor(LocalPosition) const;
    [[nodiscard]] bool isVoidBottomNeighbor(LocalPosition) const;

    [[nodiscard]] bool isVoidNorthEastNeighbor(LocalPosition) const;
    [[nodiscard]] bool isVoidNorthWestNeighbor(LocalPosition) const;
    [[nodiscard]] bool isVoidSouthEastNeighbor(LocalPosition) const;
    [[nodiscard]] bool isVoidSouthWestNeighbor(LocalPosition) const;

    [[nodiscard]] bool isVoidTopNorthNeighbor(LocalPosition) const;
    [[nodiscard]] bool isVoidTopSouthNeighbor(LocalPosition) const;
    [[nodiscard]] bool isVoidTopEastNeighbor(LocalPosition) const;
    [[nodiscard]] bool isVoidTopWestNeighbor(LocalPosition) const;
    [[nodiscard]] bool isVoidTopNorthEastNeighbor(LocalPosition) const;
    [[nodiscard]] bool isVoidTopNorthWestNeighbor(LocalPosition) const;
    [[nodiscard]] bool isVoidTopSouthEastNeighbor(LocalPosition) const;
    [[nodiscard]] bool isVoidTopSouthWestNeighbor(LocalPosition) const;

    [[nodiscard]] bool isVoidBottomNorthNeighbor(LocalPosition) const;
    [[nodiscard]] bool isVoidBottomSouthNeighbor(LocalPosition) const;
    [[nodiscard]] bool isVoidBottomEastNeighbor(LocalPosition) const;
    [[nodiscard]] bool isVoidBottomWestNeighbor(LocalPosition) const;
    [[nodiscard]] bool isVoidBottomNorthEastNeighbor(LocalPosition) const;
    [[nodiscard]] bool isVoidBottomNorthWestNeighbor(LocalPosition) const;
    [[nodiscard]] bool isVoidBottomSouthEastNeighbor(LocalPosition) const;
    [[nodiscard]] bool isVoidBottomSouthWestNeighbor(LocalPosition) const;

    [[nodiscard]] std::array<unsigned, 4> getVertexAO(Direction, LocalPosition) const;

    void setBuilt(bool);
    void setEmpty(bool);
public:
    Chunk() = default;
    explicit Chunk(const ChunkPosition position) : position(position) {}

    ~Chunk() { destroyMesh(); }

    void fetchNeighbors(const ChunkNeighbors &);

    void preload();
    void buildVoxels();
    void buildMesh();

    void destroyMesh();
    void resetVoxels();

    void render() const;

    void setPosition(ChunkPosition position);
    void setVoxelIDAtPosition(VoxelID voxel_id, LocalPosition local);

    [[nodiscard]] bool isBuilt() const;
    [[nodiscard]] bool isEmpty() const;
    [[nodiscard]] bool isVoidAt(LocalPosition local) const;
    [[nodiscard]] bool isChunkVisible(const std::array<glm::vec4, 6>& frustum_planes) const;

    [[nodiscard]] VoxelID getVoxelID(LocalPosition local) const;
};

#endif