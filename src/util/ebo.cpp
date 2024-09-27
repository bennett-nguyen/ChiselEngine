#include "ebo.hpp"

EBO::EBO() {
    glGenBuffers(1, &this->ID);
}

EBO::~EBO() {
    glDeleteBuffers(1, &this->ID);
}

void EBO::bind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ID);
}

void EBO::buffer_data(GLsizeiptr size, const void* data, GLenum usage) {
    this->bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
}
