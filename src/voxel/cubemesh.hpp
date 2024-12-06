#ifndef CUBEMESH_HPP
#define CUBEMESH_HPP

#include "vao.hpp"
#include "vbo.hpp"
#include "ebo.hpp"
#include "shader_program.hpp"
#include "vertex.hpp"
#include <array>
#include <glm/glm.hpp>
#include "face_id.hpp"
#include "constant.hpp"

class CubeMesh {
public:
    CubeMesh();

    void render(glm::mat4 view, glm::mat4 projection, glm::vec3 position);

private:
    std::array<GLuint, 36> m_indices_data;
    std::array<CubeMeshVertex, 24> m_vertices_data;
    VAO m_mesh_vao;
    VBO m_mesh_vbo;
    EBO m_mesh_ebo;
    glm::mat4 m_mesh_model;
    ShaderProgram m_shader_program;
};

#endif