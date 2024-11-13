#ifndef COLORS_HPP
#define COLORS_HPP

#include <glm/vec4.hpp>

namespace Colors {
    glm::vec4 BLACK(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 WHITE(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 RED(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 GREEN(0.0f, 1.0f, 0.0f, 1.0f);
    glm::vec4 BLUE(0.0f, 0.0f, 1.0f, 1.0f);
    glm::vec4 DARK_SLATE_GRAY(0.2f, 0.3f, 0.3f, 1.0f);
    glm::vec4 ORANGE_CHOCOLATE(1.0f, 0.5f, 0.2f, 1.0f);
    glm::vec4 SKY_BLUE(0.45490f, 0.70196f, 1.0f, 1.0f);

    glm::vec4 to_rgb_vec4(float r, float g, float b, float a) {
        return glm::vec4(r, g, b, a);
    }
}

#endif