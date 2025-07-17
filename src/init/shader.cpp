#include "shader.hpp"

GLenum getShaderType(const std::string &file_extension) {
    if ("vert" == file_extension) return GL_VERTEX_SHADER;
    if ("tesc" == file_extension) return GL_TESS_CONTROL_SHADER;
    if ("tese" == file_extension) return GL_TESS_EVALUATION_SHADER;
    if ("geom" == file_extension) return GL_GEOMETRY_SHADER;
    if ("frag" == file_extension) return GL_FRAGMENT_SHADER;
    if ("comp" == file_extension) return GL_COMPUTE_SHADER;
    throw std::runtime_error("Shader Error: Unknown GLSL shader extension: " + file_extension);
}

std::string getFileContent(const std::string &filename) {
    std::ifstream in(filename, std::ios::binary);

    if (!in) {
        throw std::runtime_error("Shader File Error: Shader file cannot be found: " + filename);
    }

    std::string content;
    in.seekg(0, std::ios::end);
    content.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&content[0], content.size());
    in.close();
    return content;
}

void shaderCompilationCheck(const ShaderID shader) {
    int success;
    char log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success) return;
    glGetShaderInfoLog(shader, 512, nullptr, log);
    throw std::runtime_error("Shader Compilation Error:\n\n" + std::string(log));
}

void programLinkingCheck(const ShaderProgramID shader_program) {
    int success;
    char log[512];
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);

    if (success) return;
    glGetProgramInfoLog(shader_program, 512, nullptr, log);
    throw std::runtime_error("Shader Program Linking Error:\n\n" + std::string(log));
}

void attachShader(const std::string &filename, const ShaderProgramID shader_program) {
    const ShaderID shader = makeShader(filename);
    glAttachShader(shader_program, shader);
    deleteShader(shader);
}

ShaderID makeShader(const std::string &filename) {
    const std::string shader_content = getFileContent(filename);
    const char *source = shader_content.c_str();

    const std::string FILE_EXTENSION = filename.substr(filename.find_last_of(".") + 1);
    const GLenum SHADER_TYPE = getShaderType(FILE_EXTENSION);

    const ShaderID shader = glCreateShader(SHADER_TYPE);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    shaderCompilationCheck(shader);

    return shader;
}

void deleteShader(const ShaderID shader) {
    glDeleteShader(shader);
}

void linkProgram(const ShaderProgramID shader_program) {
    glLinkProgram(shader_program);
    programLinkingCheck(shader_program);
}

void activateShaderProgram(const ShaderProgramID shader_program) {
    glUseProgram(shader_program);
}

void deleteShaderProgram(const ShaderProgramID shader_program) {
    glDeleteProgram(shader_program);
}

// Uniform Variables

void uniformInt(const ShaderProgramID shader_program, const char *name, const GLint v) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniform1i(location, v);
}

void uniformFloat(const ShaderProgramID shader_program, const char *name, const GLfloat v) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniform1f(location, v);
}

void uniformUint(const ShaderProgramID shader_program, const char *name, const GLuint v) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniform1ui(location, v);
}

void uniformBool(const ShaderProgramID shader_program, const char *name, const GLboolean v) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniform1i(location, v);
}

// Uniform Vector2

void uniformVec2(const ShaderProgramID shader_program, const char *name, const glm::vec2 v) {
    uniformVec2(shader_program, name, v.x, v.y);
}

void uniformVec2(const ShaderProgramID shader_program, const char *name, const GLint v0, const GLint v1) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniform2i(location, v0, v1);
}

void uniformVec2(const ShaderProgramID shader_program, const char *name, const GLfloat v0, const GLfloat v1) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniform2f(location, v0, v1);
}

void uniformVec2(const ShaderProgramID shader_program, const char *name, const GLuint v0, const GLuint v1) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniform2ui(location, v0, v1);
}

void uniformVec2(const ShaderProgramID shader_program, const char *name, const GLboolean v0, const GLboolean v1) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniform2i(location, v0, v1);
}

// Uniform Vector3

void uniformVec3(const ShaderProgramID shader_program, const char *name, const glm::vec3 v) {
    uniformVec3(shader_program, name, v.x, v.y, v.z);
}

void uniformVec3(const ShaderProgramID shader_program, const char *name, const GLint v0, const GLint v1, const GLint v2) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniform3i(location, v0, v1, v2);
}

void uniformVec3(const ShaderProgramID shader_program, const char *name, const GLfloat v0, const GLfloat v1, const GLfloat v2) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniform3f(location, v0, v1, v2);
}

void uniformVec3(const ShaderProgramID shader_program, const char *name, const GLuint v0, const GLuint v1, const GLuint v2) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniform3ui(location, v0, v1, v2);
}

void uniformVec3(const ShaderProgramID shader_program, const char *name, const GLboolean v0, const GLboolean v1, const GLboolean v2) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniform3i(location, v0, v1, v2);
}

// Uniform Vector4

void uniformVec4(const ShaderProgramID shader_program, const char *name, const glm::vec4 v) {
    uniformVec4(shader_program, name, v.x, v.y, v.z, v.w);
}

void uniformVec4(const ShaderProgramID shader_program, const char *name, const GLint v0, const GLint v1, const GLint v2, const GLint v3) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniform4i(location, v0, v1, v2, v3);
}

void uniformVec4(const ShaderProgramID shader_program, const char *name, const GLfloat v0, const GLfloat v1, const GLfloat v2, const GLfloat v3) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniform4f(location, v0, v1, v2, v3);
}

void uniformVec4(const ShaderProgramID shader_program, const char *name, const GLuint v0, const GLuint v1, const GLuint v2, const GLuint v3) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniform4ui(location, v0, v1, v2, v3);
}

void uniformVec4(const ShaderProgramID shader_program, const char *name, const GLboolean v0, const GLboolean v1, const GLboolean v2, const GLboolean v3) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniform4i(location, v0, v1, v2, v3);
}

// Uniform Matrix
void uniformMat2f(const ShaderProgramID shader_program, const char *name, const GLsizei count, const GLboolean transpose, glm::mat2 mat) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniformMatrix2fv(location, count, transpose, glm::value_ptr(mat));
}

void uniformMat3f(const ShaderProgramID shader_program, const char *name, const GLsizei count, const GLboolean transpose, glm::mat3 mat) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniformMatrix3fv(location, count, transpose, glm::value_ptr(mat));
}

void uniformMat4f(const ShaderProgramID shader_program, const char *name, const GLsizei count, const GLboolean transpose, glm::mat4 mat) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniformMatrix4fv(location, count, transpose, glm::value_ptr(mat));
}

void uniformMat2x3f(const ShaderProgramID shader_program, const char *name, const GLsizei count, const GLboolean transpose, glm::mat2x3 mat) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniformMatrix2x3fv(location, count, transpose, glm::value_ptr(mat));
}

void uniformMat3x2f(const ShaderProgramID shader_program, const char *name, const GLsizei count, const GLboolean transpose, glm::mat3x2 mat) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniformMatrix3x2fv(location, count, transpose, glm::value_ptr(mat));
}

void uniformMat2x4f(const ShaderProgramID shader_program, const char *name, const GLsizei count, const GLboolean transpose, glm::mat2x4 mat) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniformMatrix2x4fv(location, count, transpose, glm::value_ptr(mat));
}

void uniformMat4x2f(const ShaderProgramID shader_program, const char *name, const GLsizei count, const GLboolean transpose, glm::mat4x2 mat) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniformMatrix4x2fv(location, count, transpose, glm::value_ptr(mat));
}

void uniformMat3x4f(const ShaderProgramID shader_program, const char *name, const GLsizei count, const GLboolean transpose, glm::mat3x4 mat) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniformMatrix3x4fv(location, count, transpose, glm::value_ptr(mat));
}

void uniformMat4x3f(const ShaderProgramID shader_program, const char *name, const GLsizei count, const GLboolean transpose, glm::mat4x3 mat) {
    const GLint location = glGetUniformLocation(shader_program, name);
    glUniformMatrix4x3fv(location, count, transpose, glm::value_ptr(mat));
}