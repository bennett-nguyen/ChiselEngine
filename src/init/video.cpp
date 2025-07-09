#include "video.hpp"

void initVideo() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Video could not be initialized:" << '\n'
            << SDL_GetError();
        exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Enable 4x MSAA
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, Constant::MULTISAMPLE_LEVEL);
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    std::cout << "Video is initialized" << '\n';
}

void initWindow(Window &window, const std::string& window_name, const float screen_occupation_percentage) {
    window.window_name = window_name;

    SDL_DisplayMode MD;
    SDL_GetDesktopDisplayMode(0, &MD);

    const int width = static_cast<int>(static_cast<float>(MD.w) * screen_occupation_percentage);
    const int height = static_cast<int>(static_cast<float>(MD.h) * screen_occupation_percentage);

    window.ptr_window = SDL_CreateWindow(
        window_name.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_OPENGL
    );

    if (nullptr == window.ptr_window) {
        std::cerr << "Failed to initialize window" << '\n'
            << SDL_GetError();
        exit(1);
    }

    std::cout << "Window is initialized" << '\n';

    window.gl_context = SDL_GL_CreateContext(window.ptr_window);
    std::cout << "GL context is initialized" << '\n';

    gladLoadGLLoader(SDL_GL_GetProcAddress);
    std::cout << "GLAD is initialized" << '\n' << '\n';

    std::cout << "GPU Vendor: " << glGetString(GL_VENDOR) << '\n';
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';
    std::cout << "Version: " << glGetString(GL_VERSION) << '\n';
    std::cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n' << '\n';
}

void destroyWindow(const Window &window) {
    SDL_GL_DeleteContext(window.gl_context);
    std::cout << "Destroyed GL context" << '\n';

    SDL_DestroyWindow(window.ptr_window);
    std::cout << "Destroyed window" << '\n';
}

void swapBuffers(Window &window) {
    SDL_GL_SwapWindow(window.ptr_window);
}

void clearWindow(const GLclampf r, const GLclampf g, const GLclampf b, const GLclampf a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void clearWindow(const glm::vec4 color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

float computeAspectRatio(const Window &window) {
    int width, height;
    SDL_GetWindowSize(window.ptr_window, &width, &height);

    return static_cast<float>(width) / static_cast<float>(height);
}