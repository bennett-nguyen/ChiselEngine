#include "vbo.hpp"

VBO::VBO() {}

VBO::~VBO() {
    if (glIsBuffer(this->ID)) this->delete_buffer();
}

void VBO::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, this->ID);
}

void VBO::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::buffer_data(GLsizeiptr size, const void* data, GLenum usage) {
    this->bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}

void VBO::attrib(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer) {
    this->bind();
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

void VBO::attribI(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {
    this->bind();
    glVertexAttribIPointer(index, size, type, stride, pointer);
}

void VBO::attribL(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {
    this->bind();
    glVertexAttribLPointer(index, size, type, stride, pointer);
}

void VBO::enable_attrib_array(GLuint index) {
    this->bind();
    glEnableVertexAttribArray(index);
}

void VBO::gen_buffer() {
    glGenBuffers(1, &this->ID);
}

void VBO::delete_buffer() {
    glDeleteBuffers(1, &this->ID);
}
