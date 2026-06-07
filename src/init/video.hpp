#ifndef VIDEO_HPP
#define VIDEO_HPP

#include <string>
#include <iostream>

#include <glad/gl.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <glm/vec4.hpp>

#include "constant.hpp"
#include "debug_output.hpp"


#ifdef NDEBUG
    constexpr bool IS_DEBUGGING_ENABLE = false;
#else
    constexpr bool IS_DEBUGGING_ENABLE = true;
#endif

struct Window {
    std::string window_name;
    SDL_Window* ptr_window;
    SDL_GLContext gl_context;
};

void initVideo();
void initWindow(Window &window, const std::string& window_name, float screen_occupation_percentage);

void destroyWindow(const Window &window);
void swapBuffers(const Window &window);

void clearWindow(glm::vec4 color);

std::pair<int, int> getWindowWidthHeight(const Window &window);
float computeAspectRatio(const Window &window);

#endif
