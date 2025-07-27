#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <array>
#include <vector>
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
 * x              4 bit           0..15                       X_BIT
 * y              5 bit           0..31                       Y_BIT
 * z              4 bit           0..15                       Z_BIT
 * face_id        3 bit           0..6                        F_BIT
 * voxel_id       8 bit           0..255                      V_BIT
 */

class Chunk;

typedef uint8_t VoxelID;

enum class Direction : unsigned {
    Top     = 0, // Y+
    Bottom  = 1, // Y-
    North   = 2, // X+
    South   = 3, // X-
    East    = 4, // Z+
    West    = 5, // Z-

    NorthEast = 6,
    NorthWest = 7,
    SouthEast = 8,
    SouthWest = 9,

    faceBegin = Top,
    faceEnd   = West,

    Nil
};

const std::unordered_map<Direction, ChunkPosition> DIRECTION_VECTORS {
    { Direction::Top,    ChunkPosition(0,  1, 0) },
    { Direction::Bottom, ChunkPosition(0, -1, 0) },

    { Direction::North, ChunkPosition( 1, 0, 0) },
    { Direction::South, ChunkPosition(-1, 0, 0) },
    { Direction::East,  ChunkPosition( 0, 0, 1) },
    { Direction::West,  ChunkPosition( 0, 0,-1) },

    { Direction::NorthEast, ChunkPosition( 1, 0,  1) },
    { Direction::NorthWest, ChunkPosition( 1, 0, -1) },
    { Direction::SouthEast, ChunkPosition(-1, 0,  1) },
    { Direction::SouthWest, ChunkPosition(-1, 0, -1) },
};

struct Vertex {
    GLuint packed_data;

    Vertex() = default;
    Vertex(int vertex_index, const LocalPosition &voxel_origin, Direction face_id, VoxelID voxel_id);
};

struct ChunkMesh {
    GLuint ssbo_vertices {}, ebo {}, vao {};

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
};

struct AABB {
    glm::vec3 vmin, vmax;
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

    [[nodiscard]] bool isVoidInNextTo(Direction, LocalPosition) const;

    void translateBoundingBox();
    void updateBoundingBoxWithCubeFace(Direction face, const LocalPosition &voxel_origin);

    void setBuilt(bool);
    void setEmpty(bool);
public:
    Chunk() = default;
    explicit Chunk(const ChunkPosition position) : position(position) {}

    ~Chunk() { destroyMesh(); }

    void fetchNeighbors(const ChunkNeighbors &);

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
    [[nodiscard]] bool isChunkVisible(const std::vector<glm::vec4>& frustum_planes) const;

    [[nodiscard]] VoxelID   getVoxelID(LocalPosition local) const;
};

#endif