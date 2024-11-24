#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <array>
#include <glm/vec3.hpp>
#include <cstring>
#include "constant.hpp"
#include "chunk_mesh.hpp"
#include <glm/gtc/noise.hpp>

class Chunk {
public:
    Chunk(glm::ivec3 chunk_coord);
    ~Chunk();

    void unload();
    void render();
    void build_voxels();
    void build_mesh(unsigned *north_neighbor, unsigned *south_neighbor,
        unsigned *east_neighbor, unsigned *west_neighbor);
    void destroy_mesh();
    glm::mat4 get_chunk_model();
    unsigned* get_pvoxels();

private:
    unsigned *m_pvoxels;
    ChunkMesh m_mesh;
    glm::ivec3 m_chunk_coord;
    bool m_is_empty = true;
};

#endif