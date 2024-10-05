#include "vao.hpp"

VAO::VAO() {}

VAO::~VAO() {
    if (glIsBuffer(this->ID)) this->delete_buffer();
}

void VAO::gen_buffer() {
    glGenVertexArrays(1, &this->ID);
}

void VAO::delete_buffer() {
    glDeleteVertexArrays(1, &this->ID);
}

void VAO::bind() {
    glBindVertexArray(this->ID);
}

void VAO::unbind() {
    glBindVertexArray(0);
}
