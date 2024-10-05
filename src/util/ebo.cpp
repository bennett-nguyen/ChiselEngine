#include "ebo.hpp"

EBO::EBO() {
    this->gen_buffer();
}

EBO::~EBO() {
    if (glIsBuffer(this->ID)) this->delete_buffer();
}

void EBO::bind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ID);
}

void EBO::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::buffer_data(GLsizeiptr size, const void* data, GLenum usage) {
    this->bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
}

void EBO::gen_buffer() {
    glGenBuffers(1, &this->ID);
}

void EBO::delete_buffer() {
    glDeleteBuffers(1, &this->ID);
}
