#ifndef CHUNK_HPP
#define CHUNK_HPP


#include <vector>
#include <algorithm>

#include <GL/glew.h>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vertex.hpp"
#include "constant.hpp"
#include "conversions.hpp"

enum FaceID {
    Top,    // Y+
    Bottom, // Y-
    North,  // X+
    South,  // X-
    East,   // Z+
    West,   // Z-
    Nil
};

struct AABB {
    glm::vec3 vmin, vmax;
};

struct ChunkMesh {
    GLuint ssbo_vertices, ebo, vao;
    
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
};

struct Chunk {
    unsigned *ptr_voxels;
    ChunkMesh mesh;
    glm::ivec3 position;
    AABB bounding_box;

    Chunk(const int x, const int y, const int z) : position(x, y, z) {}
    Chunk(const glm::ivec3 position) : position(position) {}
};

bool isVoid(glm::uvec3 local_position, const Chunk* ptr_chunk);
bool isVoidEast(glm::uvec3 local_position, const Chunk* ptr_chunk, const Chunk* ptr_echunk);
bool isVoidWest(glm::uvec3 local_position, const Chunk* ptr_chunk, const Chunk* ptr_wchunk);
bool isVoidNorth(glm::uvec3 local_position, const Chunk* ptr_chunk, const Chunk* ptr_nchunk);
bool isVoidSouth(glm::uvec3 local_position, const Chunk* ptr_chunk, const Chunk* ptr_schunk);
bool isVoidTop(glm::uvec3 local_position, const Chunk* ptr_chunk);
bool isVoidBottom(glm::uvec3 local_position, const Chunk* ptr_chunk);

void buildVoxels(Chunk *ptr_chunk);

void buildMesh(Chunk* ptr_chunk,
                Chunk* ptr_nchunk,
                Chunk* ptr_schunk,
                Chunk* ptr_echunk,
                Chunk* ptr_wchunk);

void destroyMesh(Chunk* ptr_chunk);
void destroyChunk(Chunk* ptr_chunk);
void renderChunk(const Chunk* ptr_chunk);
unsigned getVoxelID(const Chunk* ptr_chunk, glm::uvec3 local_position);
glm::mat4 getChunkModel(const Chunk* ptr_chunk);

void updateBoundingBox(Chunk* ptr_chunk);
bool isChunkVisible(const Chunk *ptr_chunk, const std::vector<glm::vec4>& frustum_planes);

#endif