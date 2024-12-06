#include "vertex.hpp"

size_t getOffsetOfPos() {
    return offsetof(Vertex, m_pos);
}

size_t getOffsetOfVoxelID() {
    return offsetof(Vertex, m_voxel_id);
}

size_t getOffsetOfFaceID() {
    return offsetof(Vertex, m_face_id);
}

size_t getOffsetOfCubeMeshPos() {
    return offsetof(CubeMeshVertex, m_pos);
}

size_t getOffsetOfCubeMeshTexCoords() {
    return offsetof(CubeMeshVertex, m_tex_coords);
}