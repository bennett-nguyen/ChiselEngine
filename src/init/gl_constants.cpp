#include "gl_constants.hpp"

chisel::GLConstants chisel::loadGLConstants() {
    GLConstants constants {};

    constants.glVendor = glGetString(GL_VENDOR);
    constants.glRenderer = glGetString(GL_RENDERER);
    constants.glVersion = glGetString(GL_VERSION);
    constants.glShadingLanguageVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &constants.glMaxTextureMaxAnisotropy);
    glGetIntegerv(GL_MAX_SAMPLES, &constants.glMaxSamples);

    return constants;
}
