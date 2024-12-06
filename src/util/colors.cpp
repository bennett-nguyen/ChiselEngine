#include "colors.hpp"

const glm::vec4 Colors::BLACK                (0.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 Colors::WHITE                (1.0f, 1.0f, 1.0f, 1.0f);
const glm::vec4 Colors::RED                  (1.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 Colors::GREEN                (0.0f, 1.0f, 0.0f, 1.0f);
const glm::vec4 Colors::BLUE                 (0.0f, 0.0f, 1.0f, 1.0f);
const glm::vec4 Colors::DARK_SLATE_GRAY      (0.2f, 0.3f, 0.3f, 1.0f);
const glm::vec4 Colors::ORANGE_CHOCOLATE     (1.0f, 0.5f, 0.2f, 1.0f);
const glm::vec4 Colors::SKY_BLUE             (0.45490f, 0.70196f, 1.0f, 1.0f);

glm::vec4 Colors::toRGB_Vec4(float r, float g, float b, float a) {
    return glm::vec4(r, g, b, a);
}