#include "gl_constants.hpp"

chisel::GLConstants& chisel::GLConstants::getInstance() {
    static GLConstants instance {};
    return instance;
}

chisel::GLConstants::GLConstants() {
    glVendor = glGetString(GL_VENDOR);
    glRenderer = glGetString(GL_RENDERER);
    glVersion = glGetString(GL_VERSION);
    glShadingLanguageVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &glMaxTextureMaxAnisotropy);
    glGetIntegerv(GL_MAX_SAMPLES, &glMaxSamples);
}

