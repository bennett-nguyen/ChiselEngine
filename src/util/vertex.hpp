#ifndef VERTEX_HPP
#define VERTEX_HPP

struct Vertex {
    GLfloat x, y, z;
    GLfloat u, v;

    Vertex(GLfloat x, GLfloat y, GLfloat z) : 
        x(x), y(y), z(z) {}

    Vertex(GLfloat x, GLfloat y, GLfloat z, GLfloat u, GLfloat v) : 
        x(x), y(y), z(z), u(u), v(v) {}
};

GLvoid* vertex_offsetof_coordinates() {
    return (GLvoid*)(offsetof(struct Vertex, x));
}

GLvoid* vertex_offsetof_uv_coordinates() {
    return (GLvoid*)(offsetof(struct Vertex, u));
}

#endif