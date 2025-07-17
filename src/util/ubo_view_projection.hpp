#ifndef UBO_VIEW_PROJECTION_HPP
#define UBO_VIEW_PROJECTION_HPP

#include <glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

/*
 * UBO Binding Point: 0
 *
 * UBO is hidden and can only be interacted with via declared functions
 * in this header file.
 *
 * Any vertex shader that uses this UBO requires this definition in the shader:
 * layout(binding = 0, std140) uniform ViewProjection {
 *     mat4 view;
 *     mat4 projection;
 * };
*/

void setupUBOViewProjection();
void setView(const glm::mat4 &view);
void setProjection(const glm::mat4 &projection);
void bindUBOViewProjection();

#endif
