#ifndef DISPLAY_HPP
#define DISPLAY_HPP

// C++ Standard Libraries
#include <iostream>
#include <string>

// External Libraries
#include <SDL2/SDL.h>
#include <GL/glew.h>

class Window {
public:
    int width, height;

    Window(const std::string &window_name, int width, int height, Uint32 SDL_init_flags, Uint32 window_flags);
    Window(const std::string &window_name, float screen_occupation_percentage, Uint32 SDL_init_flags, Uint32 window_flags);
    ~Window();

    SDL_Window* get_window();
    SDL_GLContext get_gl_context();

    void swap_buffers();
    void clear(const GLclampf r, const GLclampf g, const GLclampf b, const GLclampf a);
    float wh_ratio();

private:
    std::string name;
    SDL_Window *window;
    SDL_GLContext gl_context;
};

#endif