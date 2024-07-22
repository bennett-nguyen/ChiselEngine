#ifndef WINDOW_LOADER
#define WINDOW_LOADER

#include <iostream>
#include <string>

#include <SDL2/SDL.h>
#include <GL/glew.h>

class Window {
public:
    Window(const std::string &_window_name, int width, int height, Uint32 SDL_init_flags, Uint32 window_flags);
    ~Window();

    SDL_Window* get_window_ptr();

    void swap_buffers();
    void clear(const GLclampf r, const GLclampf g, const GLclampf b, const GLclampf a);
    void apply_default_settings();
private:
    std::string window_name;
    SDL_Window *window = nullptr;
    SDL_GLContext gl_context;
};

#endif