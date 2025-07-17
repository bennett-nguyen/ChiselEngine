#include "ubo_view_projection.hpp"

GLuint UBO_ViewProjection;

void setupUBOViewProjection() {
    glCreateBuffers(1, &UBO_ViewProjection);
    glNamedBufferStorage(UBO_ViewProjection, static_cast<GLsizeiptr>(2 * sizeof(glm::mat4)), nullptr, GL_DYNAMIC_STORAGE_BIT);
}

void setView(const glm::mat4 &view) {
    glNamedBufferSubData(UBO_ViewProjection, 0, sizeof(glm::mat4), glm::value_ptr(view));
}

void setProjection(const glm::mat4 &projection) {
    glNamedBufferSubData(UBO_ViewProjection, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
}

void bindUBOViewProjection() {
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO_ViewProjection);
}