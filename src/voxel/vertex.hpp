#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>

struct Vertex {
    glm::ivec3 position {};
    glm::vec2 tex_coord {};
    GLuint voxel_id {};
    GLuint face_id {};

    Vertex() = default;
    Vertex(const glm::ivec3 position, const glm::vec2 tex_coord, const GLuint voxel_id, const GLuint face_id) :
        position(position), tex_coord(tex_coord), voxel_id(voxel_id), face_id(face_id) {}

    Vertex(const int x, const int y, const int z, const float x_tex, const float y_tex, const GLuint voxel_id, const GLuint face_id) :
        position(x, y, z), tex_coord(x_tex, y_tex), voxel_id(voxel_id), face_id(face_id)  {}
};

size_t getOffsetOfPos();
size_t getOffsetOfVoxelID();
size_t getOffsetOfFaceID();
size_t getOffsetOfTexCoord();

#endif