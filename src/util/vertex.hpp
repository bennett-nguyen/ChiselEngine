#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>

struct Vertex {
    glm::ivec3 m_pos;
    GLuint m_voxel_id, m_face_id;

    Vertex() {}

    Vertex(GLint x, GLint y, GLint z, GLuint voxel_id, GLuint face_id)
        : m_pos(x, y, z), m_voxel_id(voxel_id), m_face_id(face_id) {}

    Vertex(glm::ivec3 pos, GLuint voxel_id, GLuint face_id)
        : m_pos(pos), m_voxel_id(voxel_id), m_face_id(face_id) {}
};

struct CubeMeshVertex {
    glm::vec3 m_pos;
    glm::vec2 m_tex_coords;

    CubeMeshVertex() {}
};

size_t getOffsetOfPos();
size_t getOffsetOfVoxelID();
size_t getOffsetOfFaceID();
size_t getOffsetOfCubeMeshPos();
size_t getOffsetOfCubeMeshTexCoords();

#endif