#ifndef VAO_HPP
#define VAO_HPP

#include <GL/glew.h>

class VAO {
public:
    VAO();
    ~VAO();
    void bind();
    void unbind();
    void genBuffer();
    void deleteBuffer();

private:
    GLuint m_ID;
};

#endif