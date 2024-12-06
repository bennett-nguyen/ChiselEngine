#include "ebo.hpp"

EBO::EBO() {
    this->genBuffer();
}

EBO::~EBO() {
    if (glIsBuffer(m_ID)) this->deleteBuffer();
}

void EBO::bind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
}

void EBO::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::bufferData(GLsizeiptr size, const void* data, GLenum usage) {
    this->bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
}

void EBO::genBuffer() {
    glGenBuffers(1, &m_ID);
}

void EBO::deleteBuffer() {
    glDeleteBuffers(1, &m_ID);
}
