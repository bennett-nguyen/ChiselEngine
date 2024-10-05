#include "mesh.hpp"

Mesh::Mesh() {}

Mesh::~Mesh() {
    this->vbo.delete_buffer();
    this->vao.delete_buffer();
    this->ebo.delete_buffer();
}

void Mesh::load() {
    this->vbo.gen_buffer();
    this->ebo.gen_buffer();
    this->vao.gen_buffer();
}

void Mesh::setup() {
    this->vao.bind();
    this->ebo.bind();
    this->vbo.bind();

    this->vbo.buffer_data(this->vertices.size() * sizeof(Vertex), this->vertices.data(), GL_STATIC_DRAW);
    this->vbo.attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(offsetof(Vertex, position)));
    this->vbo.enable_attrib_array(0);

    this->ebo.buffer_data(this->indices.size() * sizeof(GLuint), this->indices.data(), GL_STATIC_DRAW);

    this->vao.unbind();
    this->vbo.unbind();
    this->ebo.unbind();
}

void Mesh::destroy() {
    this->vertices.clear();
    this->indices.clear();
    this->vao.delete_buffer();
    this->vbo.delete_buffer();
    this->ebo.delete_buffer();
}

void Mesh::draw(ShaderProgram &shader_program, glm::vec4 voxel_fill, glm::vec4 voxel_outline) {
    this->vao.bind();
    shader_program.uniform_vec4("color", voxel_fill);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);

    shader_program.uniform_vec4("color", voxel_outline);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(1.5f);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    this->vao.unbind();
}
