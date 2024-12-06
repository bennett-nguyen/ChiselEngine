#ifndef EBO_HPP
#define EBO_HPP

#include <GL/glew.h>

class EBO {
public:
    EBO();
    ~EBO();
    void bind();
    void unbind();
    void bufferData(GLsizeiptr size, const void* data, GLenum usage);
    void genBuffer();
    void deleteBuffer();
private:
    GLuint m_ID;
};

#endif