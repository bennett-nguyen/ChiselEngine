#include "window.hpp"
#include "iochars.hpp"

Window::Window(const std::string &window_name, float screen_occupation_percentage, Uint32 SDL_init_flags, Uint32 window_flags)
    : m_name(window_name) {
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

    m_width = int((float)MD.w * screen_occupation_percentage);
    m_height = int((float)MD.h * screen_occupation_percentage);
    
    m_pwindow = SDL_CreateWindow(
        window_name.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        m_width,
        m_height,
        window_flags
    );

    if (NULL == m_pwindow) {
        std::cerr << window_name << ": failed to create window" << endline
            << SDL_GetError();
        exit(1);
    }

    std::cout << window_name << ": window is initialized" << endline;

    m_gl_context = SDL_GL_CreateContext(m_pwindow);
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
    SDL_GL_DeleteContext(m_gl_context);
    std::cout << m_name << ": destroyed GL context" << endline;

    SDL_DestroyWindow(m_pwindow);
    std::cout << m_name << ": destroyed window" << endline;
}

SDL_Window* Window::getWindowPointer() {
    return m_pwindow;
}

SDL_GLContext Window::getGLContext() {
    return m_gl_context;
}

void Window::swapBuffers() {
    SDL_GL_SwapWindow(m_pwindow);
}

void Window::clear(const GLclampf r, const GLclampf g, const GLclampf b, const GLclampf a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::clear(glm::vec4 color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

float Window::getWidthHeightRatio() {
    return (float) m_width / (float) m_height;
}

int Window::getWidth() {
    return m_width;
}


int Window::getHeight() {
    return m_height;
}