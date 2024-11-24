#include "vao.hpp"

VAO::VAO() {}

VAO::~VAO() {
    if (glIsBuffer(m_ID)) this->delete_buffer();
}

void VAO::gen_buffer() {
    glGenVertexArrays(1, &m_ID);
}

void VAO::delete_buffer() {
    glDeleteVertexArrays(1, &m_ID);
}

void VAO::bind() {
    glBindVertexArray(m_ID);
}

void VAO::unbind() {
    glBindVertexArray(0);
}
