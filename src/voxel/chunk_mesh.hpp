#ifndef CHUNK_MESH_HPP
#define CHUNK_MESH_HPP

#include <vector>

#include "vertex.hpp"
#include "face_id.hpp"
#include "voxel_id.hpp"
#include "constant.hpp"
#include "shader_program.hpp"
#include "voxel_math.hpp"

class ChunkMesh {
public:
    ChunkMesh();
    ~ChunkMesh();

    void render();
    void buildChunkMesh(unsigned *m_pvoxels, unsigned *north_neighbor,
    unsigned *south_neighbor, unsigned *east_neighbor, unsigned *west_neighbor);
    void destroyChunkMesh();

private:
    GLuint m_mesh_vbo;
    GLuint m_mesh_vao;
    GLuint m_mesh_ebo;

    std::vector<Vertex> m_vertices_data;
    std::vector<GLuint> m_indices_data;
};

#endif