#ifndef VAO_HPP
#define VAO_HPP

#include <GL/glew.h>

class VAO {
public:
    VAO();
    ~VAO();
    void bind();

private:
    GLuint ID;
};

#endif