#ifndef COLORS_HPP
#define COLORS_HPP

#include <glm/glm.hpp>

namespace Colors {
    extern const glm::vec4 BLACK;
    extern const glm::vec4 WHITE;
    extern const glm::vec4 RED;
    extern const glm::vec4 GREEN;
    extern const glm::vec4 BLUE;
    extern const glm::vec4 DARK_SLATE_GRAY;
    extern const glm::vec4 ORANGE_CHOCOLATE;
    extern const glm::vec4 SKY_BLUE;

    glm::vec4 toRGB_Vec4(float r, float g, float b, float a);
}

#endif