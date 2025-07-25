#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <array>
#include <vector>
#include <algorithm>
#include <ostream>
#include <iostream>
#include <algorithm>

#include <glad.h>
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

typedef uint8_t VoxelID;

enum class FaceID : unsigned {
    Top     = 0, // Y+
    Bottom  = 1, // Y-
    North   = 2, // X+
    South   = 3, // X-
    East    = 4, // Z+
    West    = 5, // Z-
    Nil        ,

    begin   = 0,
    end     = Nil - 1
};

struct Vertex {
    GLuint packed_data;

    Vertex() = default;
    Vertex(int vertex_index, const LocalPosition &voxel_origin, FaceID face_id, VoxelID voxel_id);
};

struct ChunkMesh {
    GLuint ssbo_vertices {}, ebo {}, vao {};

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
};

struct AABB {
    glm::vec3 vmin, vmax;
};

class Chunk {
    ChunkMesh mesh;
    AABB bounding_box {};
    ChunkPosition position {};

    std::array<VoxelID, Constant::CHUNK_VOLUME> voxel_ids {};

    bool is_empty = true;
    bool is_built = false;

    [[nodiscard]] bool isVoidTop   (LocalPosition voxel_origin) const;
    [[nodiscard]] bool isVoidBottom(LocalPosition voxel_origin) const;
    [[nodiscard]] bool isVoidEast  (LocalPosition voxel_origin, const Chunk* ptr_east_neighbor)  const;
    [[nodiscard]] bool isVoidWest  (LocalPosition voxel_origin, const Chunk* ptr_west_neighbor)  const;
    [[nodiscard]] bool isVoidNorth (LocalPosition voxel_origin, const Chunk* ptr_north_neighbor) const;
    [[nodiscard]] bool isVoidSouth (LocalPosition voxel_origin, const Chunk* ptr_south_neighbor) const;

    void translateBoundingBox();
    void updateBoundingBoxWithCubeFace(FaceID face, const LocalPosition &voxel_origin);

    void setBuilt(bool);
    void setEmpty(bool);
public:
    Chunk() = default;
    explicit Chunk(const ChunkPosition position) : position(position) {}

    ~Chunk() { destroyMesh(); }

    void buildVoxels();
    void buildMesh(const Chunk* ptr_north_neighbor, const Chunk* ptr_south_neighbor, const Chunk* ptr_east_neighbor, const Chunk* ptr_west_neighbor);

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