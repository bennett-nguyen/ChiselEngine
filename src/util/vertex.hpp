#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <GL/glew.h>
#include <glm/vec3.hpp>

struct Vertex {
    glm::ivec3 m_pos;
    GLuint m_voxel_id, m_face_id;

    Vertex() {}

    Vertex(GLint x, GLint y, GLint z, GLuint voxel_id, GLuint face_id)
        : m_pos(glm::ivec3(x, y, z)), m_voxel_id(voxel_id), m_face_id(face_id) {}

    Vertex(glm::ivec3 pos, GLuint voxel_id, GLuint face_id)
        : m_pos(pos), m_voxel_id(voxel_id), m_face_id(face_id) {}
};

size_t get_offset_of_pos();
size_t get_offset_of_voxel_id();
size_t get_offset_of_face_id();

#endif