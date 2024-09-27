#include "vao.hpp"

VAO::VAO() {
    glGenVertexArrays(1, &this->ID);
}

VAO::~VAO() {
    glDeleteVertexArrays(1, &this->ID);
}

void VAO::bind() {
    glBindVertexArray(this->ID);
}