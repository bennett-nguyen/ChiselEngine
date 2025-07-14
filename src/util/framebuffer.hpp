#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <stdexcept>

#include <glad.h>

#include "constant.hpp"

/*
 * For a full list of GL Sized Internal Format, refers to:
 * https://registry.khronos.org/OpenGL-Refpages/es3.1/html/glRenderbufferStorage.xhtml
*/

class Framebuffer {
    GLenum sized_internal_format {};
    GLsizei width = 0, height = 0, samples = 0;
    bool is_multisample_fb = false;

    GLuint texture_color_buffer {}, renderbuffer {}, framebuffer {};

    void setupTextureColorBuffer() const;
    void setupRenderBuffer() const;
    void setupFrameBuffer() const;

public:
    void init();
    void initMultiSample(int samples);

    void setSize(GLsizei width, GLsizei height);
    void setSizedInternalFormat(GLenum sized_internal_format);
    void setUp() const;

    void bind() const;
    void blitTo(const Framebuffer& destination) const;

    [[nodiscard]] GLuint getTextureName() const;
    [[nodiscard]] GLuint getFramebufferName() const;
    [[nodiscard]] std::pair<GLsizei, GLsizei> getFramebufferDimension() const;

    ~Framebuffer();
};

#endif
