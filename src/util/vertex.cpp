#include "vertex.hpp"

size_t get_offset_of_pos() {
    return offsetof(Vertex, pos);
}

size_t get_offset_of_voxel_id() {
    return offsetof(Vertex, voxel_id);
}

size_t get_offset_of_face_id() {
    return offsetof(Vertex, face_id);
}