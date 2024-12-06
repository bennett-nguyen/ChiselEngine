#include "vao.hpp"

VAO::VAO() {}

VAO::~VAO() {
    if (glIsBuffer(m_ID)) this->deleteBuffer();
}

void VAO::genBuffer() {
    glGenVertexArrays(1, &m_ID);
}

void VAO::deleteBuffer() {
    glDeleteVertexArrays(1, &m_ID);
}

void VAO::bind() {
    glBindVertexArray(m_ID);
}

void VAO::unbind() {
    glBindVertexArray(0);
}
