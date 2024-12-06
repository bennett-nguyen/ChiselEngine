#ifndef DISPLAY_HPP
#define DISPLAY_HPP

// C++ Standard Libraries
#include <iostream>
#include <string>

// External Libraries
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/vec4.hpp>

class Window {
public:
    Window(const std::string &window_name, float screen_occupation_percentage, Uint32 SDL_init_flags, Uint32 window_flags);
    ~Window();

    SDL_Window* getWindowPointer();
    SDL_GLContext getGLContext();

    void swapBuffers();
    void clear(const GLclampf r, const GLclampf g, const GLclampf b, const GLclampf a);
    void clear(glm::vec4 color);

    int getWidth();
    int getHeight();
    float getWidthHeightRatio();

private:
    int m_width, m_height;
    std::string m_name;
    SDL_Window *m_pwindow;
    SDL_GLContext m_gl_context;
};

#endif