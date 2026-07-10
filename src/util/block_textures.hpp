#ifndef BLOCK_TEXTURES_HPP
#define BLOCK_TEXTURES_HPP

#include <iostream>
#include <fstream>

#include <glad/gl.h>
#include <stb_image.h>

#include "block_registry.hpp"
#include "direction.hpp"

namespace chisel {
    class BlockTextures {
        GLuint texture_array {};
        GLuint sampler {};

        BlockTextures();
    public:
        static BlockTextures& getInstance();
        ~BlockTextures();

        void bind() const;
        static void unbind();
    };
}

#endif