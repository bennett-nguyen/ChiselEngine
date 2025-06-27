#include "vertex.hpp"

size_t getOffsetOfPos() {
    return offsetof(Vertex, position);
}

size_t getOffsetOfVoxelID() {
    return offsetof(Vertex, voxel_id);
}

size_t getOffsetOfFaceID() {
    return offsetof(Vertex, face_id);
}

size_t getOffsetOfTexCoord() {
    return offsetof(Vertex, tex_coord);
}