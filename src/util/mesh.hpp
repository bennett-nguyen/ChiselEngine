#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include "vertex.hpp"
#include "shader_program.hpp"
#include "vbo.hpp"
#include "vao.hpp"
#include "ebo.hpp"

class Mesh {
public:
    std::vector<GLuint> indices;
    std::vector<Vertex> vertices;
    std::vector<GLuint> outline_indices;

    Mesh();
    ~Mesh();

    void load();
    void setup();
    void destroy();
    void draw(ShaderProgram &shader_program, glm::vec4 voxel_fill, glm::vec4 voxel_outline);
private:
    VBO vbo;
    VAO vao;
    EBO ebo;
};

#endif