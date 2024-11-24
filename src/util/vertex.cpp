#include "vertex.hpp"

size_t get_offset_of_pos() {
    return offsetof(Vertex, m_pos);
}

size_t get_offset_of_voxel_id() {
    return offsetof(Vertex, m_voxel_id);
}

size_t get_offset_of_face_id() {
    return offsetof(Vertex, m_face_id);
}