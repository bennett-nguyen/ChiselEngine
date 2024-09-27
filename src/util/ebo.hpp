#ifndef EBO_HPP
#define EBO_HPP

#include <GL/glew.h>

class EBO {
public:
    EBO();
    ~EBO();
    void bind();
    void buffer_data(GLsizeiptr size, const void* data, GLenum usage);

private:
    GLuint ID;
};

#endif