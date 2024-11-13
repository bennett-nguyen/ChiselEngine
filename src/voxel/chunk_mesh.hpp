#ifndef CHUNK_MESH_HPP
#define CHUNK_MESH_HPP

#include <vector>

#include "vao.hpp"
#include "vbo.hpp"
#include "vertex.hpp"
#include "face_id.hpp"
#include "constant.hpp"
#include "shader_program.hpp"

unsigned get_voxel_idx(int x, int y, int z);

class ChunkMesh {
public:
    ChunkMesh();
    ~ChunkMesh();

    void render(ShaderProgram *pchunk_shader, glm::mat4 view, glm::mat4 projection, glm::ivec3 chunk_coord);
    void build_chunk_mesh(unsigned *m_pvoxels);
    void destroy_chunk_mesh();

private:
    VBO m_mesh_vbo;
    VAO m_mesh_vao;
    std::vector<Vertex> m_vertices_data;
};

#endif