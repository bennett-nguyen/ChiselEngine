#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <GL/glew.h>
#include <glm/vec3.hpp>

struct Vertex {
    glm::ivec3 pos;
    GLuint voxel_id, face_id;

    Vertex() {}

    Vertex(GLint x, GLint y, GLint z, GLuint voxel_id, GLuint face_id)
        : pos(glm::ivec3(x, y, z)), voxel_id(voxel_id), face_id(face_id) {}

    Vertex(glm::ivec3 pos, GLuint voxel_id, GLuint face_id)
        : pos(pos), voxel_id(voxel_id), face_id(face_id) {}
};

size_t get_offset_of_pos();
size_t get_offset_of_voxel_id();
size_t get_offset_of_face_id();

#endif