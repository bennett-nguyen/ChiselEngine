#include "framebuffer.hpp"

GLenum getRenderbufferAttachment(const GLenum sized_internal_formal) {
    switch (sized_internal_formal) {
        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32F:
            return GL_DEPTH_ATTACHMENT;
        case GL_DEPTH24_STENCIL8:
        case GL_DEPTH32F_STENCIL8:
            return GL_DEPTH_STENCIL_ATTACHMENT;
        case GL_STENCIL_INDEX8:
            return GL_STENCIL_ATTACHMENT;
        default:
            throw std::invalid_argument("Framebuffer Error: invalid sized internal format");
    }
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteRenderbuffers(1, &renderbuffer);
    glDeleteTextures(1, &texture_color_buffer);
}

void Framebuffer::init() {
    glCreateTextures(GL_TEXTURE_2D, 1, &texture_color_buffer);
    glCreateRenderbuffers(1, &renderbuffer);
    glCreateFramebuffers(1, &framebuffer);
}

void Framebuffer::initMultiSample(const GLsizei samples) {
    glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &texture_color_buffer);
    glCreateRenderbuffers(1, &renderbuffer);
    glCreateFramebuffers(1, &framebuffer);
    is_multisample_fb = true;
    this->samples = samples;
}

void Framebuffer::setSizedInternalFormat(const GLenum sized_internal_format) {
    this->sized_internal_format = sized_internal_format;
}

void Framebuffer::setSize(const GLsizei width, const GLsizei height) {
    this->width = width;
    this->height = height;
}

void Framebuffer::setupTextureColorBuffer() const {
    if (not is_multisample_fb) {
        glBindTexture(GL_TEXTURE_2D, texture_color_buffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTextureParameteri(texture_color_buffer, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(texture_color_buffer, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        return;
    }

    glTextureStorage2DMultisample(
        texture_color_buffer,
        samples,
        GL_RGB8,
        width, height,
        GL_TRUE
    );
}

void Framebuffer::setupRenderBuffer() const {
    if (not is_multisample_fb) {
        glNamedRenderbufferStorage(renderbuffer, sized_internal_format, width, height);
        return;
    }

    glNamedRenderbufferStorageMultisample(renderbuffer, samples, sized_internal_format, width, height);
}

void Framebuffer::setupFrameBuffer() const {
    const GLenum attachment = getRenderbufferAttachment(sized_internal_format);

    glNamedFramebufferTexture(framebuffer, GL_COLOR_ATTACHMENT0, texture_color_buffer, 0);
    glNamedFramebufferRenderbuffer(framebuffer, attachment, GL_RENDERBUFFER, renderbuffer);

    if (glCheckNamedFramebufferStatus(framebuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Framebuffer Error: Framebuffer is not complete");
    }
}

void Framebuffer::blitTo(const Framebuffer& destination) const {
    const auto [dest_width, dest_height] = destination.getFramebufferDimension();
    glBlitNamedFramebuffer(framebuffer, destination.getFramebufferName(), 0, 0, width, height, 0, 0, dest_width, dest_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

std::pair<GLsizei, GLsizei> Framebuffer::getFramebufferDimension() const {
    return std::make_pair(width, height);
}

void Framebuffer::setUp() const {
    setupTextureColorBuffer();
    setupRenderBuffer();
    setupFrameBuffer();
}

void Framebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

GLuint Framebuffer::getTextureName() const {
    return texture_color_buffer;
}

GLuint Framebuffer::getFramebufferName() const {
    return framebuffer;
}
