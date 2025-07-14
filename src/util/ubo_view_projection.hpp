#ifndef UBO_VIEW_PROJECTION_HPP
#define UBO_VIEW_PROJECTION_HPP

#include <glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

/*
 * Any vertex shader that uses this UBO requires this definition in the shader:
 *
 * layout(binding = 0, std140) uniform ViewProjection {
 *     mat4 view;
 *     mat4 projection;
 * };
*/

inline GLuint ubo_view_projection;

inline void setupUBOViewProjection() {
    glCreateBuffers(1, &ubo_view_projection);
    glNamedBufferStorage(ubo_view_projection, static_cast<GLsizeiptr>(2 * sizeof(glm::mat4)), nullptr, GL_DYNAMIC_STORAGE_BIT);
}

inline void setView(const glm::mat4 &view) {
    glNamedBufferSubData(ubo_view_projection, 0, sizeof(glm::mat4), glm::value_ptr(view));
}

inline void setProjection(const glm::mat4 &projection) {
    glNamedBufferSubData(ubo_view_projection, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
}

inline void bindUBOViewProjection() {
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_view_projection);
}

#endif
