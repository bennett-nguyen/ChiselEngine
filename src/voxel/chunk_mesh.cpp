#include "chunk_mesh.hpp"

bool isVoidInChunk(glm::ivec3 local_voxel_pos, unsigned *m_pvoxels) {
    int x = local_voxel_pos.x, y = local_voxel_pos.y, z = local_voxel_pos.z;
    return VoxelID::Air == m_pvoxels[VoxelMath::getVoxelIndex((unsigned)x, (unsigned)y, (unsigned)z)];
}

bool isVoidNorth(glm::ivec3 local_voxel_pos, unsigned *m_pvoxels, unsigned *north_neighbor) {
    int x = local_voxel_pos.x, y = local_voxel_pos.y, z = local_voxel_pos.z;

    if (x >= (int)Constant::CHUNK_SIZE) {
        if (nullptr == north_neighbor) return false;
        return isVoidInChunk(glm::ivec3(0, y, z), north_neighbor);
    }

    return isVoidInChunk(local_voxel_pos, m_pvoxels);
}

bool isVoidSouth(glm::ivec3 local_voxel_pos, unsigned *m_pvoxels, unsigned *south_neighbor) {
    int x = local_voxel_pos.x, y = local_voxel_pos.y, z = local_voxel_pos.z;

    if (0 > x) {
        if (nullptr == south_neighbor) return false;
        return isVoidInChunk(glm::ivec3(Constant::CHUNK_SIZE-1, y, z), south_neighbor);
    }

    return isVoidInChunk(local_voxel_pos, m_pvoxels);
}

bool isVoidEast(glm::ivec3 local_voxel_pos, unsigned *m_pvoxels, unsigned *east_neighbor) {
    int x = local_voxel_pos.x, y = local_voxel_pos.y, z = local_voxel_pos.z;

    if (z >= (int)Constant::CHUNK_SIZE) {
        if (nullptr == east_neighbor) return false;
        return isVoidInChunk(glm::ivec3(x, y, 0), east_neighbor);
    }

    return isVoidInChunk(local_voxel_pos, m_pvoxels);
}

bool isVoidWest(glm::ivec3 local_voxel_pos, unsigned *m_pvoxels, unsigned *west_neighbor) {
    int x = local_voxel_pos.x, y = local_voxel_pos.y, z = local_voxel_pos.z;

    if (0 > z) {
        if (nullptr == west_neighbor) return false;
        return isVoidInChunk(glm::ivec3(x, y, Constant::CHUNK_SIZE-1), west_neighbor);
    }

    return isVoidInChunk(local_voxel_pos, m_pvoxels);
}

bool isVoidTop(glm::ivec3 local_voxel_pos, unsigned *m_pvoxels) {
    int y = local_voxel_pos.y;

    if (y >= (int)Constant::CHUNK_HEIGHT) {
        return true;
    }

    return isVoidInChunk(local_voxel_pos, m_pvoxels);
}

bool isVoidBottom(glm::ivec3 local_voxel_pos, unsigned *m_pvoxels) {
    int y = local_voxel_pos.y;

    if (0 > y) {
        return true;
    }

    return isVoidInChunk(local_voxel_pos, m_pvoxels);
}

ChunkMesh::ChunkMesh() {}

ChunkMesh::~ChunkMesh() {
    destroyChunkMesh();
}

void ChunkMesh::buildChunkMesh(unsigned *m_pvoxels, unsigned *north_neighbor,
    unsigned *south_neighbor, unsigned *east_neighbor, unsigned *west_neighbor) {
    GLuint index = 0;

    for (int x = 0; x < (int)Constant::CHUNK_SIZE; x++) {
        for (int z = 0; z < (int)Constant::CHUNK_SIZE; z++) {
            for (int y = 0; y < (int)Constant::CHUNK_HEIGHT; y++) {
                unsigned voxel_id = m_pvoxels[VoxelMath::getVoxelIndex((unsigned)x, (unsigned)y, (unsigned)z)];
                if (0 == voxel_id) continue;

                Vertex v0, v1, v2, v3;

                // Top
                if (isVoidTop(glm::ivec3(x, y+1, z), m_pvoxels)) {
                    v0 = Vertex(x,   y+1, z,   voxel_id, FaceID::Top);
                    v1 = Vertex(x+1, y+1, z,   voxel_id, FaceID::Top);
                    v2 = Vertex(x+1, y+1, z+1, voxel_id, FaceID::Top);
                    v3 = Vertex(x,   y+1, z+1, voxel_id, FaceID::Top);

                    m_vertices_data.insert(m_vertices_data.end(), { v0, v1, v2, v3 });
                    m_indices_data.insert(m_indices_data.end(), { index+0, index+3, index+2, index+0, index+2, index+1 });
                    index += 4;
                }

                // Bottom
                if (isVoidBottom(glm::ivec3(x, y-1, z), m_pvoxels)) {
                    v0 = Vertex(x,   y, z,   voxel_id, FaceID::Bottom);
                    v1 = Vertex(x+1, y, z,   voxel_id, FaceID::Bottom);
                    v2 = Vertex(x+1, y, z+1, voxel_id, FaceID::Bottom);
                    v3 = Vertex(x,   y, z+1, voxel_id, FaceID::Bottom);

                    m_vertices_data.insert(m_vertices_data.end(), { v0, v1, v2, v3 });
                    m_indices_data.insert(m_indices_data.end(), { index+0, index+2, index+3, index+0, index+1, index+2 });
                    index += 4;
                }

                // North
                if (isVoidNorth(glm::ivec3(x+1, y, z), m_pvoxels, north_neighbor)) {
                    v0 = Vertex(x+1, y,   z,   voxel_id, FaceID::North);
                    v1 = Vertex(x+1, y+1, z,   voxel_id, FaceID::North);
                    v2 = Vertex(x+1, y+1, z+1, voxel_id, FaceID::North);
                    v3 = Vertex(x+1, y,   z+1, voxel_id, FaceID::North);

                    m_vertices_data.insert(m_vertices_data.end(), { v0, v1, v2, v3 });
                    m_indices_data.insert(m_indices_data.end(), { index+0, index+1, index+2, index+0, index+2, index+3 });
                    index += 4;
                }

                // South
                if (isVoidSouth(glm::ivec3(x-1, y, z), m_pvoxels, south_neighbor)) {
                    v0 = Vertex(x, y,   z,   voxel_id, FaceID::South);
                    v1 = Vertex(x, y+1, z,   voxel_id, FaceID::South);
                    v2 = Vertex(x, y+1, z+1, voxel_id, FaceID::South);
                    v3 = Vertex(x, y,   z+1, voxel_id, FaceID::South);

                    m_vertices_data.insert(m_vertices_data.end(), { v0, v1, v2, v3 });
                    m_indices_data.insert(m_indices_data.end(), { index+0, index+2, index+1, index+0, index+3, index+2 });
                    index += 4;
                }

                // East
                if (isVoidEast(glm::ivec3(x, y, z+1), m_pvoxels, east_neighbor)) {
                    v0 = Vertex(x,   y,   z+1, voxel_id, FaceID::East);
                    v1 = Vertex(x,   y+1, z+1, voxel_id, FaceID::East);
                    v2 = Vertex(x+1, y+1, z+1, voxel_id, FaceID::East);
                    v3 = Vertex(x+1, y,   z+1, voxel_id, FaceID::East);

                    m_vertices_data.insert(m_vertices_data.end(), { v0, v1, v2, v3 });
                    m_indices_data.insert(m_indices_data.end(), { index+0, index+2, index+1, index+0, index+3, index+2 });
                    index += 4;
                }

                // West
                if (isVoidWest(glm::ivec3(x, y, z-1), m_pvoxels, west_neighbor)) {
                    v0 = Vertex(x,   y,   z, voxel_id, FaceID::West);
                    v1 = Vertex(x,   y+1, z, voxel_id, FaceID::West);
                    v2 = Vertex(x+1, y+1, z, voxel_id, FaceID::West);
                    v3 = Vertex(x+1, y,   z, voxel_id, FaceID::West);

                    m_vertices_data.insert(m_vertices_data.end(), { v0, v1, v2, v3 });
                    m_indices_data.insert(m_indices_data.end(), { index+0, index+1, index+2, index+0, index+2, index+3 });
                    index += 4;
                }
            }
        }
    }

    glGenBuffers(1, &m_mesh_vbo);
    glGenBuffers(1, &m_mesh_ebo);
    glGenVertexArrays(1, &m_mesh_vao);

    glBindVertexArray(m_mesh_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_mesh_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_mesh_ebo);

    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(m_vertices_data.size() * sizeof(Vertex)), m_vertices_data.data(), GL_STATIC_DRAW);
    glVertexAttribIPointer(0, 3, GL_INT, sizeof(Vertex), (void*)getOffsetOfPos());
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void*)getOffsetOfVoxelID());
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void*)getOffsetOfFaceID());

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(m_indices_data.size() * sizeof(GLuint)), m_indices_data.data(), GL_STATIC_DRAW);
}

void ChunkMesh::destroyChunkMesh() {
    glDeleteVertexArrays(1, &m_mesh_vao);
    glDeleteBuffers(1, &m_mesh_vbo);
    glDeleteBuffers(1, &m_mesh_ebo);
    m_vertices_data.clear();
    m_indices_data.clear();
}

void ChunkMesh::render() {
    glBindVertexArray(m_mesh_vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)(m_indices_data.size()), GL_UNSIGNED_INT, 0);
}