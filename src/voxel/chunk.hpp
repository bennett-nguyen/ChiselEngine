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

enum FaceID {
    Top,    // Y+
    Bottom, // Y-
    North,  // X+
    South,  // X-
    East,   // Z+
    West,   // Z-
    Nil
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
    VoxelID *ptr_voxels {};
    ChunkPosition position;

    [[nodiscard]] bool isVoidTop   (LocalPosition voxel_origin) const;
    [[nodiscard]] bool isVoidBottom(LocalPosition voxel_origin) const;
    [[nodiscard]] bool isVoidEast  (LocalPosition voxel_origin, const Chunk* ptr_east_neighbor)  const;
    [[nodiscard]] bool isVoidWest  (LocalPosition voxel_origin, const Chunk* ptr_west_neighbor)  const;
    [[nodiscard]] bool isVoidNorth (LocalPosition voxel_origin, const Chunk* ptr_north_neighbor) const;
    [[nodiscard]] bool isVoidSouth (LocalPosition voxel_origin, const Chunk* ptr_south_neighbor) const;

    void translateBoundingBox();
    void updateBoundingBoxWithCubeFace(FaceID face, const LocalPosition &voxel_origin);

public:
    explicit Chunk(const glm::ivec3 position) : position(position) {}
    Chunk(const int x, const int y, const int z) : position(x, y, z) {}

    ~Chunk() { destroyChunk(); }

    void buildVoxels();
    void buildMesh(const Chunk* ptr_north_neighbor, const Chunk* ptr_south_neighbor, const Chunk* ptr_east_neighbor, const Chunk* ptr_west_neighbor);

    void destroyMesh();
    void destroyChunk();

    void render() const;
    void setVoxelIDAtPosition(VoxelID voxel_id, LocalPosition local) const;

    [[nodiscard]] bool isVoidAt(LocalPosition local) const;
    [[nodiscard]] bool isChunkVisible(const std::vector<glm::vec4>& frustum_planes) const;

    [[nodiscard]] VoxelID   getVoxelID(LocalPosition local) const;
    [[nodiscard]] glm::mat4 getChunkModel() const;
};

#endif