#ifndef EBO_HPP
#define EBO_HPP

#include <GL/glew.h>

class EBO {
public:
    EBO();
    ~EBO();
    void bind();
    void unbind();
    void buffer_data(GLsizeiptr size, const void* data, GLenum usage);
    void gen_buffer();
    void delete_buffer();
private:
    GLuint m_ID;
};

#endif