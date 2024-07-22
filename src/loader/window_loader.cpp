#include "window_loader.hpp"

Window::Window(const std::string &_window_name, int width, int height, Uint32 SDL_init_flags, Uint32 window_flags) {
    this->window_name = _window_name;

    if (SDL_Init(SDL_init_flags) < 0) {
        std::cerr << "SDL could not be initialized: " << std::endl
            << SDL_GetError();

        exit(1);
    }

    std::cout << this->window_name << ": SDL is initialized" << std::endl;

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    this->window = SDL_CreateWindow(
        this->window_name.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        window_flags
    );

    if (NULL == this->window) {
        std::cerr << this->window_name << ": failed to create window" << std::endl
            << SDL_GetError();
        exit(1);
    }

    std::cout << this->window_name << ": window is initialized" << std::endl;

    this->gl_context = SDL_GL_CreateContext(this->window);
    std::cout << this->window_name << ": GL context is initialized" << std::endl;

    if (GLEW_OK != glewInit()) {
        std::cerr << "failed to initialized GLEW" << std::endl;
    }
}

Window::~Window() {
    SDL_GL_DeleteContext(this->gl_context);
    std::cout << this->window_name << ": destroyed GL context" << std::endl;

    SDL_DestroyWindow(window);
    std::cout << this->window_name << ": destroyed window" << std::endl;
}

SDL_Window* Window::get_window_ptr() {
    return this->window;
}

void Window::swap_buffers() {
    SDL_GL_SwapWindow(this->window);
}

void Window::clear(const GLclampf r, const GLclampf g, const GLclampf b, const GLclampf a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::apply_default_settings() {
    SDL_SetRelativeMouseMode(SDL_TRUE);
    glEnable(GL_DEPTH_TEST);
}