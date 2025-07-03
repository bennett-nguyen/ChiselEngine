#ifndef VIDEO_HPP
#define VIDEO_HPP

#include <string>
#include <iostream>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/vec4.hpp>
#include "constant.hpp"

struct Window {
    std::string window_name;
    SDL_Window* ptr_window;
    SDL_GLContext gl_context;
};

void initVideo();
void initWindow(Window &window, const std::string& window_name, float screen_occupation_percentage);

void destroyWindow(const Window &window);
void swapBuffers(Window &window);

void clearWindow(GLclampf r, GLclampf g, GLclampf b, GLclampf a);
void clearWindow(glm::vec4 color);
float computeAspectRatio(const Window &window);

#endif