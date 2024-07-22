#include "shader_loader.hpp"

std::string get_file_content(const char *filename) {
    std::ifstream in(filename, std::ios::binary);

    if (!in) {
        throw (errno);
    }

    std::string content;
    in.seekg(0, std::ios::end);
    content.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&content[0], content.size());
    in.close();
    return content;
}

void shader_compilation_failcheck(const GLuint &shader) {
    int success;
    char log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success) return;
    glGetShaderInfoLog(shader, 512, NULL, log);
    std::cerr << "error while compiling shader: " << log << std::endl;
}

void program_linking_failcheck(const GLuint &ID) {
    int success;
    char log[512];
    glGetProgramiv(ID, GL_LINK_STATUS, &success);

    if (success) return;
    glGetProgramInfoLog(ID, 512, NULL, log);
    std::cerr << "error while linking ID: " << log << std::endl;
}

Shader::Shader(const char *vshader_file, const char *fshader_file) {
    std::string vshader_content = get_file_content(vshader_file);
    std::string fshader_content = get_file_content(fshader_file);

    const char *vshader_source = vshader_content.c_str();
    const char *fshader_source = fshader_content.c_str();

    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
    this->ID = glCreateProgram();

    glShaderSource(vshader, 1, &vshader_source, NULL);
    glShaderSource(fshader, 1, &fshader_source, NULL);

    glCompileShader(vshader);
    glCompileShader(fshader);

    shader_compilation_failcheck(vshader);
    shader_compilation_failcheck(fshader);

    glAttachShader(this->ID, vshader);
    glAttachShader(this->ID, fshader);

    glLinkProgram(this->ID);

    program_linking_failcheck(this->ID);

    glDeleteShader(vshader);
    glDeleteShader(fshader);
}

void Shader::activate() {
    glUseProgram(this->ID);
}

void Shader::purge() {
    glDeleteProgram(this->ID);
}

unsigned Shader::get_uniform_location(const char *name) {
    return glGetUniformLocation(this->get_shader_id(), name);
}

GLuint Shader::get_shader_id() {
    return (this->ID);
}

// Uniform Variables

void Shader::uniform_int(const char *name, GLint v) {
    unsigned location = this->get_uniform_location(name);
    glUniform1i(location, v);
}

void Shader::uniform_float(const char *name, GLfloat v) {
    unsigned location = this->get_uniform_location(name);
    glUniform1f(location, v);
}

void Shader::uniform_unsigned(const char *name, GLuint v) {
    unsigned location = this->get_uniform_location(name);
    glUniform1ui(location, v);
}

void Shader::uniform_bool(const char *name, GLboolean v) {
    unsigned location = this->get_uniform_location(name);
    glUniform1i(location, v);
}

// Uniform Vector2

void Shader::uniform_vec2(const char *name, glm::vec2 v) {
    this->uniform_vec2(name, v.x, v.y);
}

void Shader::uniform_vec2(const char *name, GLint v0, GLint v1) {
    unsigned location = this->get_uniform_location(name);
    glUniform2i(location, v0, v1);
}

void Shader::uniform_vec2(const char *name, GLfloat v0, GLfloat v1) {
    unsigned location = this->get_uniform_location(name);
    glUniform2f(location, v0, v1);
}

void Shader::uniform_vec2(const char *name, GLuint v0, GLuint v1) {
    unsigned location = this->get_uniform_location(name);
    glUniform2ui(location, v0, v1);
}

void Shader::uniform_vec2(const char *name, GLboolean v0, GLboolean v1) {
    unsigned location = this->get_uniform_location(name);
    glUniform2i(location, v0, v1);
}

// Uniform Vector3

void Shader::uniform_vec3(const char *name, glm::vec3 v) {
    this->uniform_vec3(name, v.x, v.y, v.z);
}

void Shader::uniform_vec3(const char *name, GLint v0, GLint v1, GLint v2) {
    unsigned location = this->get_uniform_location(name);
    glUniform3i(location, v0, v1, v2);
}

void Shader::uniform_vec3(const char *name, GLfloat v0, GLfloat v1, GLfloat v2) {
    unsigned location = this->get_uniform_location(name);
    glUniform3f(location, v0, v1, v2);
}

void Shader::uniform_vec3(const char *name, GLuint v0, GLuint v1, GLuint v2) {
    unsigned location = this->get_uniform_location(name);
    glUniform3ui(location, v0, v1, v2);
}

void Shader::uniform_vec3(const char *name, GLboolean v0, GLboolean v1, GLboolean v2) {
    unsigned location = this->get_uniform_location(name);
    glUniform3i(location, v0, v1, v2);
}

// Uniform Vector4

void Shader::uniform_vec4(const char *name, glm::vec4 v) {
    this->uniform_vec4(name, v.x, v.y, v.z, v.w);
}

void Shader::uniform_vec4(const char *name, GLint v0, GLint v1, GLint v2, GLint v3) {
    unsigned location = this->get_uniform_location(name);
    glUniform4i(location, v0, v1, v2, v3);
}

void Shader::uniform_vec4(const char *name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
    unsigned location = this->get_uniform_location(name);
    glUniform4f(location, v0, v1, v2, v3);
}

void Shader::uniform_vec4(const char *name, GLuint v0, GLuint v1, GLuint v2, GLuint v3) {
    unsigned location = this->get_uniform_location(name);
    glUniform4ui(location, v0, v1, v2, v3);
}

void Shader::uniform_vec4(const char *name, GLboolean v0, GLboolean v1, GLboolean v2, GLboolean v3) {
    unsigned location = this->get_uniform_location(name);
    glUniform4i(location, v0, v1, v2, v3);
}

// Uniform Matrix
void Shader::uniform_mat2f(const char *name, GLsizei count, GLboolean transpose, glm::mat2 mat) {
    unsigned location = this->get_uniform_location(name);
    glUniformMatrix2fv(location, count, transpose, glm::value_ptr(mat));
}

void Shader::uniform_mat3f(const char *name, GLsizei count, GLboolean transpose, glm::mat3 mat) {
    unsigned location = this->get_uniform_location(name);
    glUniformMatrix3fv(location, count, transpose, glm::value_ptr(mat));
}

void Shader::uniform_mat4f(const char *name, GLsizei count, GLboolean transpose, glm::mat4 mat) {
    unsigned location = this->get_uniform_location(name);
    glUniformMatrix4fv(location, count, transpose, glm::value_ptr(mat));
}

void Shader::uniform_mat2x3f(const char *name, GLsizei count, GLboolean transpose, glm::mat2x3 mat) {
    unsigned location = this->get_uniform_location(name);
    glUniformMatrix2x3fv(location, count, transpose, glm::value_ptr(mat));
}

void Shader::uniform_mat3x2f(const char *name, GLsizei count, GLboolean transpose, glm::mat3x2 mat) {
    unsigned location = this->get_uniform_location(name);
    glUniformMatrix3x2fv(location, count, transpose, glm::value_ptr(mat));
}

void Shader::uniform_mat2x4f(const char *name, GLsizei count, GLboolean transpose, glm::mat2x4 mat) {
    unsigned location = this->get_uniform_location(name);
    glUniformMatrix2x4fv(location, count, transpose, glm::value_ptr(mat));
}

void Shader::uniform_mat4x2f(const char *name, GLsizei count, GLboolean transpose, glm::mat4x2 mat) {
    unsigned location = this->get_uniform_location(name);
    glUniformMatrix4x2fv(location, count, transpose, glm::value_ptr(mat));
}

void Shader::uniform_mat3x4f(const char *name, GLsizei count, GLboolean transpose, glm::mat3x4 mat) {
    unsigned location = this->get_uniform_location(name);
    glUniformMatrix3x4fv(location, count, transpose, glm::value_ptr(mat));
}

void Shader::uniform_mat4x3f(const char *name, GLsizei count, GLboolean transpose, glm::mat4x3 mat) {
    unsigned location = this->get_uniform_location(name);
    glUniformMatrix4x3fv(location, count, transpose, glm::value_ptr(mat));
}