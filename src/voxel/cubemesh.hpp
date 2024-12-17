#ifndef CUBEMESH_HPP
#define CUBEMESH_HPP

#include <array>
#include <glm/glm.hpp>
#include "vertex.hpp"
#include "face_id.hpp"
#include "constant.hpp"
#include "shader_program.hpp"

class CubeMesh {
public:
    CubeMesh();

    void render();
    glm::mat4 getModel(glm::vec3 cube_mesh_position);

private:
    GLuint m_mesh_vao;
    GLuint m_mesh_vbo;
    GLuint m_mesh_ebo;

    glm::mat4 m_mesh_model;
    std::array<GLuint, 36> m_indices_data;
    std::array<CubeMeshVertex, 24> m_vertices_data;
};

#endif