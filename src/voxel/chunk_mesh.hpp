#ifndef CHUNK_MESH_HPP
#define CHUNK_MESH_HPP

#include <vector>

#include "vao.hpp"
#include "vbo.hpp"
#include "ebo.hpp"
#include "vertex.hpp"
#include "face_id.hpp"
#include "constant.hpp"
#include "shader_program.hpp"

unsigned get_voxel_idx(int x, int y, int z);

class ChunkMesh {
public:
    ChunkMesh();
    ~ChunkMesh();

    void render();
    void build_chunk_mesh(unsigned *m_pvoxels, unsigned *north_neighbor,
    unsigned *south_neighbor, unsigned *east_neighbor, unsigned *west_neighbor);
    void destroy_chunk_mesh();

private:
    VBO m_mesh_vbo;
    VAO m_mesh_vao;
    EBO m_mesh_ebo;
    std::vector<Vertex> m_vertices_data;
    std::vector<GLuint> m_indices_data;
};

#endif