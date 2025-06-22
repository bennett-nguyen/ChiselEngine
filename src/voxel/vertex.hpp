#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>

struct Vertex {
    glm::ivec3 position;
    GLuint voxel_id;
    GLuint face_id;

    Vertex(const glm::ivec3 position, const GLuint voxel_id, const GLuint face_id) :
        position(position), voxel_id(voxel_id), face_id(face_id) {}

    Vertex(const int x, const int y, const int z, const GLuint voxel_id, const GLuint face_id) :
        position(x, y, z), voxel_id(voxel_id), face_id(face_id) {}

};

size_t getOffsetOfPos();
size_t getOffsetOfVoxelID();
size_t getOffsetOfFaceID();

#endif