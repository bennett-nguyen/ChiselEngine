#ifndef SHADER_HPP
#define SHADER_HPP

#include <iostream>
#include <fstream>
#include <errno.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

std::string get_file_content(const char *filename);
void shader_compilation_failcheck(const GLuint &shader);
void program_linking_failcheck(const GLuint &ID);

class Shader {
public:
    Shader(const char *vshader_filename, const char *fshader_filename);

    void activate();
    void purge();
    GLuint get_shader_id();

    // Uniform Variable
    void uniform_int(const char *, GLint);
    void uniform_float(const char *, GLfloat);
    void uniform_unsigned(const char *, GLuint);
    void uniform_bool(const char *, GLboolean);

    // Uniform Vector
    void uniform_vec2(const char *, glm::vec2);
    void uniform_vec3(const char *, glm::vec3);
    void uniform_vec4(const char *, glm::vec4);

    void uniform_vec2(const char *, GLint, GLint);
    void uniform_vec3(const char *, GLint, GLint, GLint);
    void uniform_vec4(const char *, GLint, GLint, GLint, GLint);

    void uniform_vec2(const char *, GLuint, GLuint);
    void uniform_vec3(const char *, GLuint, GLuint, GLuint);
    void uniform_vec4(const char *, GLuint, GLuint, GLuint, GLuint);

    void uniform_vec2(const char *, GLfloat, GLfloat);
    void uniform_vec3(const char *, GLfloat, GLfloat, GLfloat);
    void uniform_vec4(const char *, GLfloat, GLfloat, GLfloat, GLfloat);

    void uniform_vec2(const char *, GLboolean, GLboolean);
    void uniform_vec3(const char *, GLboolean, GLboolean, GLboolean);
    void uniform_vec4(const char *, GLboolean, GLboolean, GLboolean, GLboolean);

    // Uniform Matrix
    void uniform_mat2f(const char *, GLsizei, GLboolean, glm::mat2);
    void uniform_mat3f(const char *, GLsizei, GLboolean, glm::mat3);
    void uniform_mat4f(const char *, GLsizei, GLboolean, glm::mat4);

    void uniform_mat2x3f(const char *, GLsizei, GLboolean, glm::mat2x3);
    void uniform_mat3x2f(const char *, GLsizei, GLboolean, glm::mat3x2);
    void uniform_mat2x4f(const char *, GLsizei, GLboolean, glm::mat2x4);
    void uniform_mat4x2f(const char *, GLsizei, GLboolean, glm::mat4x2);
    void uniform_mat3x4f(const char *, GLsizei, GLboolean, glm::mat3x4);
    void uniform_mat4x3f(const char *, GLsizei, GLboolean, glm::mat4x3);

    unsigned get_uniform_location(const char *name);

private:
    GLuint ID;
};

#endif