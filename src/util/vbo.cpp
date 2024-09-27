#include "vbo.hpp"

VBO::VBO() {
    glGenBuffers(1, &this->ID);
}

VBO::~VBO() {
    glDeleteBuffers(1, &this->ID);
}

void VBO::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, this->ID);
}

void VBO::buffer_data(GLsizeiptr size, const void* data, GLenum usage) {
    this->bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}

void VBO::attrib_pointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) {
    this->bind();
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

void VBO::enable_attrib_array(GLuint index) {
    this->bind();
    glEnableVertexAttribArray(index);
}