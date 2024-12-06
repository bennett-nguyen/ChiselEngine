#ifndef SHADERPROGRAM_HPP
#define SHADERPROGRAM_HPP

#include <iostream>
#include <fstream>
#include <errno.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

std::string getFileContent(const char *filename);
void shaderCompilationFailcheck(const GLuint &shader);
void programLinkingFailcheck(const GLuint &ID);

class ShaderProgram {
public:
    ShaderProgram();
    ShaderProgram(const char *vshader_filename, const char *fshader_filename);

    void activate();
    void purge();
    GLuint getShaderID();

    // Uniform Variable
    void uniformInt(const char *, GLint);
    void uniformFloat(const char *, GLfloat);
    void uniformUint(const char *, GLuint);
    void uniformBool(const char *, GLboolean);

    // Uniform Vector
    void uniformVec2(const char *, glm::vec2);
    void uniformVec3(const char *, glm::vec3);
    void uniformVec4(const char *, glm::vec4);

    void uniformVec2(const char *, GLint, GLint);
    void uniformVec3(const char *, GLint, GLint, GLint);
    void uniformVec4(const char *, GLint, GLint, GLint, GLint);

    void uniformVec2(const char *, GLuint, GLuint);
    void uniformVec3(const char *, GLuint, GLuint, GLuint);
    void uniformVec4(const char *, GLuint, GLuint, GLuint, GLuint);

    void uniformVec2(const char *, GLfloat, GLfloat);
    void uniformVec3(const char *, GLfloat, GLfloat, GLfloat);
    void uniformVec4(const char *, GLfloat, GLfloat, GLfloat, GLfloat);

    void uniformVec2(const char *, GLboolean, GLboolean);
    void uniformVec3(const char *, GLboolean, GLboolean, GLboolean);
    void uniformVec4(const char *, GLboolean, GLboolean, GLboolean, GLboolean);

    // Uniform Matrix
    void uniformMat2f(const char *, GLsizei, GLboolean, glm::mat2);
    void uniformMat3f(const char *, GLsizei, GLboolean, glm::mat3);
    void uniformMat4f(const char *, GLsizei, GLboolean, glm::mat4);

    void uniformMat2x3f(const char *, GLsizei, GLboolean, glm::mat2x3);
    void uniformMat3x2f(const char *, GLsizei, GLboolean, glm::mat3x2);
    void uniformMat2x4f(const char *, GLsizei, GLboolean, glm::mat2x4);
    void uniformMat4x2f(const char *, GLsizei, GLboolean, glm::mat4x2);
    void uniformMat3x4f(const char *, GLsizei, GLboolean, glm::mat3x4);
    void uniformMat4x3f(const char *, GLsizei, GLboolean, glm::mat4x3);

    GLint getUniformLocation(const char *name);

private:
    GLuint m_ID;
};

#endif