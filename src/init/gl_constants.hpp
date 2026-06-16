#ifndef GL_CONSTANT_HPP
#define GL_CONSTANT_HPP

#include <glad/gl.h>

namespace chisel {
    class GLConstants {
        const GLubyte *glVendor{};
        const GLubyte* glRenderer {};
        const GLubyte* glVersion {};
        const GLubyte* glShadingLanguageVersion {};

        GLfloat glMaxTextureMaxAnisotropy {};
        GLint glMaxSamples {};

        friend GLConstants loadGLConstants();
    public:
        const GLubyte* getGLVendor() const { return glVendor; }
        const GLubyte* getGLRenderer() const { return glRenderer; }
        const GLubyte* getGLVersion() const { return glVersion; }
        const GLubyte* getGLShadingLanguageVersion() const { return glShadingLanguageVersion; }

        GLfloat getMaxTextureMaxAnisotropy() const { return glMaxTextureMaxAnisotropy; }
        GLint getMaxSamples() const { return glMaxSamples; }
    };

    GLConstants loadGLConstants();
}

#endif