#ifndef VBO_HPP
#define VBO_HPP

#include "GL/glew.h"

class VBO {
public:
    VBO();
    ~VBO();
    void bind();
    void unbind();
    void bufferData(GLsizeiptr size, const void* data, GLenum usage);

    void attrib(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
    void attribI(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
    void attribL(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);

    void enable_attrib_array(GLuint index);
    void genBuffer();
    void deleteBuffer();
private:
    GLuint m_ID;
};

#endif