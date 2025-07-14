#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <cerrno>
#include <fstream>
#include <iostream>

#include <glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef GLuint ShaderID;
typedef GLuint ShaderProgramID;

ShaderID makeShader(const std::string &filename);
void deleteShader(ShaderID shader);

void attachShader(const std::string &filename, ShaderProgramID shader_program);
void activateShaderProgram(ShaderProgramID shader_program);
void deleteShaderProgram(ShaderProgramID shader_program);

std::string getFileContent(const std::string &filename);
void shaderCompilationCheck(ShaderID shader);
void linkProgram(ShaderProgramID shader_program);
void programLinkingCheck(ShaderProgramID shader_program);

// Uniform Variable
void uniformInt(ShaderProgramID shader_program,   const char *, GLint);
void uniformFloat(ShaderProgramID shader_program, const char *, GLfloat);
void uniformUint(ShaderProgramID shader_program,  const char *, GLuint);
void uniformBool(ShaderProgramID shader_program,  const char *, GLboolean);

// Uniform Vector
void uniformVec2(ShaderProgramID shader_program, const char *, glm::vec2);
void uniformVec3(ShaderProgramID shader_program, const char *, glm::vec3);
void uniformVec4(ShaderProgramID shader_program, const char *, glm::vec4);

void uniformVec2(ShaderProgramID shader_program, const char *, GLint, GLint);
void uniformVec3(ShaderProgramID shader_program, const char *, GLint, GLint, GLint);
void uniformVec4(ShaderProgramID shader_program, const char *, GLint, GLint, GLint, GLint);

void uniformVec2(ShaderProgramID shader_program, const char *, GLuint, GLuint);
void uniformVec3(ShaderProgramID shader_program, const char *, GLuint, GLuint, GLuint);
void uniformVec4(ShaderProgramID shader_program, const char *, GLuint, GLuint, GLuint, GLuint);

void uniformVec2(ShaderProgramID shader_program, const char *, GLfloat, GLfloat);
void uniformVec3(ShaderProgramID shader_program, const char *, GLfloat, GLfloat, GLfloat);
void uniformVec4(ShaderProgramID shader_program, const char *, GLfloat, GLfloat, GLfloat, GLfloat);

void uniformVec2(ShaderProgramID shader_program, const char *, GLboolean, GLboolean);
void uniformVec3(ShaderProgramID shader_program, const char *, GLboolean, GLboolean, GLboolean);
void uniformVec4(ShaderProgramID shader_program, const char *, GLboolean, GLboolean, GLboolean, GLboolean);

// Uniform Matrix
void uniformMat2f(ShaderProgramID shader_program, const char *, GLsizei, GLboolean, glm::mat2);
void uniformMat3f(ShaderProgramID shader_program, const char *, GLsizei, GLboolean, glm::mat3);
void uniformMat4f(ShaderProgramID shader_program, const char *, GLsizei, GLboolean, glm::mat4);

void uniformMat2x3f(ShaderProgramID shader_program, const char *, GLsizei, GLboolean, glm::mat2x3);
void uniformMat3x2f(ShaderProgramID shader_program, const char *, GLsizei, GLboolean, glm::mat3x2);
void uniformMat2x4f(ShaderProgramID shader_program, const char *, GLsizei, GLboolean, glm::mat2x4);
void uniformMat4x2f(ShaderProgramID shader_program, const char *, GLsizei, GLboolean, glm::mat4x2);
void uniformMat3x4f(ShaderProgramID shader_program, const char *, GLsizei, GLboolean, glm::mat3x4);
void uniformMat4x3f(ShaderProgramID shader_program, const char *, GLsizei, GLboolean, glm::mat4x3);

#endif