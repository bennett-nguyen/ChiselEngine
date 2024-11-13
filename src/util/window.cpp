#include "window.hpp"
#include "iochars.hpp"

Window::Window(const std::string &window_name, int width, int height, Uint32 SDL_init_flags, Uint32 window_flags)
    : width(width), height(height), name(window_name) {
    using namespace IOChars;

    if (SDL_Init(SDL_init_flags) < 0) {
        std::cerr << "SDL could not be initialized:" << std::endl
            << SDL_GetError();
        exit(1);
    }

    std::cout << window_name << ": SDL is initialized" << std::endl;

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    this->window = SDL_CreateWindow(
        window_name.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        window_flags
    );

    if (NULL == this->window) {
        std::cerr << window_name << ": failed to create window" << endline
            << SDL_GetError();
        exit(1);
    }

    std::cout << window_name << ": window is initialized" << endline;

    this->gl_context = SDL_GL_CreateContext(this->window);
    std::cout << window_name << ": GL context is initialized" << endline;

    if (GLEW_OK != glewInit()) {
        std::cerr << "failed to initialized GLEW" << endline;
        exit(1);
    }

    std::cout << window_name << ": GLEW is initialized" << endline << endline;

    std::cout << "GPU Vendor: " << glGetString(GL_VENDOR) << endline;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << endline;
    std::cout << "Version: " << glGetString(GL_VERSION) << endline;
    std::cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endline << endline;
}

Window::Window(const std::string &window_name, float screen_occupation_percentage, Uint32 SDL_init_flags, Uint32 window_flags)
    : name(window_name) {
    using namespace IOChars;

    if (SDL_Init(SDL_init_flags) < 0) {
        std::cerr << "SDL could not be initialized:" << std::endl
            << SDL_GetError();
        exit(1);
    }

    std::cout << window_name << ": SDL is initialized" << std::endl;

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    SDL_DisplayMode MD;
    SDL_GetDesktopDisplayMode(0, &MD);

    this->width = int((float)MD.w * screen_occupation_percentage);
    this->height = int((float)MD.h * screen_occupation_percentage);
    
    this->window = SDL_CreateWindow(
        window_name.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        this->width,
        this->height,
        window_flags
    );

    if (NULL == this->window) {
        std::cerr << window_name << ": failed to create window" << endline
            << SDL_GetError();
        exit(1);
    }

    std::cout << window_name << ": window is initialized" << endline;

    this->gl_context = SDL_GL_CreateContext(this->window);
    std::cout << window_name << ": GL context is initialized" << endline;

    if (GLEW_OK != glewInit()) {
        std::cerr << "failed to initialized GLEW" << endline;
        exit(1);
    }

    std::cout << window_name << ": GLEW is initialized" << endline << endline;

    std::cout << "GPU Vendor: " << glGetString(GL_VENDOR) << endline;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << endline;
    std::cout << "Version: " << glGetString(GL_VERSION) << endline;
    std::cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endline << endline;
}

Window::~Window() {
    using namespace IOChars;
    SDL_GL_DeleteContext(this->gl_context);
    std::cout << this->name << ": destroyed GL context" << endline;

    SDL_DestroyWindow(window);
    std::cout << this->name << ": destroyed window" << endline;
}

SDL_Window* Window::get_window() {
    return this->window;
}

SDL_GLContext Window::get_gl_context() {
    return this->gl_context;
}

void Window::swap_buffers() {
    SDL_GL_SwapWindow(this->window);
}

void Window::clear(const GLclampf r, const GLclampf g, const GLclampf b, const GLclampf a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::clear(glm::vec4 color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

float Window::wh_ratio() {
    return (float) this->width / (float) this->height;
}