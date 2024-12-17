#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <array>
#include <random>
#include <cstring>
#include "constant.hpp"
#include "chunk_mesh.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

class Chunk {
public:
    Chunk(glm::ivec3 chunk_coord);
    ~Chunk();

    void unload();
    void render();
    void buildVoxels();
    void buildMesh(unsigned *north_neighbor, unsigned *south_neighbor,
        unsigned *east_neighbor, unsigned *west_neighbor);
    void destroyMesh();
    void setVoxelID(unsigned voxel_idx, unsigned value);
    bool isAirAt(unsigned voxel_idx);
    glm::mat4 getChunkModel();
    unsigned* getVoxelsPointer();

private:
    unsigned *m_pvoxels;
    ChunkMesh m_mesh;
    glm::ivec3 m_chunk_coord;
    bool m_is_empty = true;
};

#endif