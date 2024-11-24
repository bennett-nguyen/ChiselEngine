#include "chunk_mesh.hpp"

unsigned get_voxel_idx(int x, int y, int z) {
    return unsigned(x + Constant::CHUNK_SIZE * z + Constant::CHUNK_AREA * y);
}

bool is_void_in_chunk(glm::ivec3 local_voxel_pos, unsigned *m_pvoxels) {
    int x = local_voxel_pos.x, y = local_voxel_pos.y, z = local_voxel_pos.z;

    if (0 > x || (unsigned)x >= Constant::CHUNK_SIZE ||
        0 > z || (unsigned)z >= Constant::CHUNK_SIZE ||
        0 > y || (unsigned)y >= Constant::CHUNK_HEIGHT) {
        return true;
    }

    return m_pvoxels[get_voxel_idx(x, y, z)] == 0;
}

ChunkMesh::ChunkMesh() {}

ChunkMesh::~ChunkMesh() {
    destroy_chunk_mesh();
}

void ChunkMesh::build_chunk_mesh(unsigned *m_pvoxels) {
    GLuint index = 0;

    for (int x = 0; x < Constant::CHUNK_SIZE; x++) {
        for (int z = 0; z < Constant::CHUNK_SIZE; z++) {
            for (int y = 0; y < Constant::CHUNK_HEIGHT; y++) {
                unsigned voxel_id = m_pvoxels[get_voxel_idx(x, y, z)];
                if (0 == voxel_id) continue;

                Vertex v0, v1, v2, v3;

                // Top
                if (is_void_in_chunk(glm::ivec3(x, y+1, z), m_pvoxels)) {
                    v0 = Vertex(x,   y+1, z,   voxel_id, Top);
                    v1 = Vertex(x+1, y+1, z,   voxel_id, Top);
                    v2 = Vertex(x+1, y+1, z+1, voxel_id, Top);
                    v3 = Vertex(x,   y+1, z+1, voxel_id, Top);

                    m_vertices_data.insert(m_vertices_data.end(), { v0, v1, v2, v3 });
                    m_indices_data.insert(m_indices_data.end(), { index+0, index+3, index+2, index+0, index+2, index+1 });
                    index += 4;
                }

                // Bottom
                if (is_void_in_chunk(glm::ivec3(x, y-1, z), m_pvoxels)) {
                    v0 = Vertex(x,   y, z,   voxel_id, Bottom);
                    v1 = Vertex(x+1, y, z,   voxel_id, Bottom);
                    v2 = Vertex(x+1, y, z+1, voxel_id, Bottom);
                    v3 = Vertex(x,   y, z+1, voxel_id, Bottom);

                    m_vertices_data.insert(m_vertices_data.end(), { v0, v1, v2, v3 });
                    m_indices_data.insert(m_indices_data.end(), { index+0, index+2, index+3, index+0, index+1, index+2 });
                    index += 4;
                }

                // North
                if (is_void_in_chunk(glm::ivec3(x+1, y, z), m_pvoxels)) {
                    v0 = Vertex(x+1, y,   z,   voxel_id, North);
                    v1 = Vertex(x+1, y+1, z,   voxel_id, North);
                    v2 = Vertex(x+1, y+1, z+1, voxel_id, North);
                    v3 = Vertex(x+1, y,   z+1, voxel_id, North);

                    m_vertices_data.insert(m_vertices_data.end(), { v0, v1, v2, v3 });
                    m_indices_data.insert(m_indices_data.end(), { index+0, index+1, index+2, index+0, index+2, index+3 });
                    index += 4;
                }

                // South
                if (is_void_in_chunk(glm::ivec3(x-1, y, z), m_pvoxels)) {
                    v0 = Vertex(x, y,   z,   voxel_id, South);
                    v1 = Vertex(x, y+1, z,   voxel_id, South);
                    v2 = Vertex(x, y+1, z+1, voxel_id, South);
                    v3 = Vertex(x, y,   z+1, voxel_id, South);

                    m_vertices_data.insert(m_vertices_data.end(), { v0, v1, v2, v3 });
                    m_indices_data.insert(m_indices_data.end(), { index+0, index+2, index+1, index+0, index+3, index+2 });
                    index += 4;
                }

                // East
                if (is_void_in_chunk(glm::ivec3(x, y, z+1), m_pvoxels)) {
                    v0 = Vertex(x,   y,   z+1, voxel_id, East);
                    v1 = Vertex(x,   y+1, z+1, voxel_id, East);
                    v2 = Vertex(x+1, y+1, z+1, voxel_id, East);
                    v3 = Vertex(x+1, y,   z+1, voxel_id, East);

                    m_vertices_data.insert(m_vertices_data.end(), { v0, v1, v2, v3 });
                    m_indices_data.insert(m_indices_data.end(), { index+0, index+2, index+1, index+0, index+3, index+2 });
                    index += 4;
                }

                // West
                if (is_void_in_chunk(glm::ivec3(x, y, z-1), m_pvoxels)) {
                    v0 = Vertex(x,   y,   z, voxel_id, West);
                    v1 = Vertex(x,   y+1, z, voxel_id, West);
                    v2 = Vertex(x+1, y+1, z, voxel_id, West);
                    v3 = Vertex(x+1, y,   z, voxel_id, West);

                    m_vertices_data.insert(m_vertices_data.end(), { v0, v1, v2, v3 });
                    m_indices_data.insert(m_indices_data.end(), { index+0, index+1, index+2, index+0, index+2, index+3 });
                    index += 4;
                }
            }
        }
    }

    m_mesh_vbo.gen_buffer();
    m_mesh_vao.gen_buffer();
    m_mesh_ebo.gen_buffer();

    m_mesh_vao.bind();
    m_mesh_ebo.bind();
    m_mesh_vbo.bind();

    m_mesh_vbo.buffer_data(m_vertices_data.size() * sizeof(Vertex), m_vertices_data.data(), GL_STATIC_DRAW);
    m_mesh_vbo.attribI(0, 3, GL_INT, sizeof(Vertex), (void*)get_offset_of_pos());
    m_mesh_vbo.attribI(1, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void*)get_offset_of_voxel_id());
    m_mesh_vbo.attribI(2, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void*)get_offset_of_face_id());

    m_mesh_vbo.enable_attrib_array(0);
    m_mesh_vbo.enable_attrib_array(1);
    m_mesh_vbo.enable_attrib_array(2);

    m_mesh_ebo.buffer_data(m_indices_data.size() * sizeof(GLuint), m_indices_data.data(), GL_STATIC_DRAW);
}

void ChunkMesh::destroy_chunk_mesh() {
    m_mesh_vbo.delete_buffer();
    m_mesh_vao.delete_buffer();
    m_mesh_ebo.delete_buffer();
    m_vertices_data.clear();
    m_indices_data.clear();
}

void ChunkMesh::render() {
    m_mesh_vao.bind();
    glDrawElements(GL_TRIANGLES, m_indices_data.size(), GL_UNSIGNED_INT, 0);
}