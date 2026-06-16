#include "chisel.hpp"

void chisel::System::setGLWindowAttribute(const SDL_GLAttr attr, const int value) {
    SDL_GL_SetAttribute(attr, value);

    if constexpr (IS_DEBUGGING_ENABLE) {
        int actual_value {};
        SDL_GL_GetAttribute(attr, &actual_value);
        std::clog << "LOG :: Requested value " << value << " for " << attr << " attribute" << value << '\n';
        std::clog << "LOG :: Got " << actual_value << '\n';
    }
}

chisel::System::System(const SDL_InitFlags flags) {
     if (not SDL_Init(flags)) {
         const std::string sdl_error_msg { SDL_GetError() };
         throw Exception("ERROR :: SDL3 cannot be initialized:\n" + sdl_error_msg);
     }

    std::clog << "LOG :: SDL3 is initialized\n";

    if constexpr (IS_DEBUGGING_ENABLE) {
        setGLWindowAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    }
}

chisel::System::~System() {
    std::clog << "LOG :: SDL3 is destroyed\n";
    SDL_Quit();
}

chisel::GUISystem::GUISystem(const WindowPtr &p_window, const GLContextPtr &p_gl_context) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplSDL3_InitForOpenGL(p_window.get(), p_gl_context.get());
    ImGui_ImplOpenGL3_Init();

    std::clog << "LOG :: GUI system is initialized\n";
}

chisel::GUISystem::~GUISystem() {
    std::clog << "LOG :: GUI system is shutdown\n";

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

chisel::WindowPtr chisel::makeGLWindow(const std::string &title) {
    System::setGLWindowAttribute(SDL_GL_RED_SIZE, 8);
    System::setGLWindowAttribute(SDL_GL_GREEN_SIZE, 8);
    System::setGLWindowAttribute(SDL_GL_BLUE_SIZE, 8);
    System::setGLWindowAttribute(SDL_GL_ALPHA_SIZE, 8);

    System::setGLWindowAttribute(SDL_GL_BUFFER_SIZE, 32);
    System::setGLWindowAttribute(SDL_GL_DOUBLEBUFFER, 1);

    System::setGLWindowAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    System::setGLWindowAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    const SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, title.c_str());
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 1728);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 972);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED);
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);

    SDL_Window* raw_window = SDL_CreateWindowWithProperties(props);

    SDL_DestroyProperties(props);

    if (nullptr == raw_window) {
        const std::string sdl_error_msg { SDL_GetError() };
        throw Exception("ERROR :: GL window creation error:\n" + sdl_error_msg);
    }

    std::clog << "LOG :: GL window is initialized" << '\n';
    return WindowPtr(raw_window);
}

chisel::GLContextPtr chisel::makeGLContext(const WindowPtr &p_window) {
    SDL_GLContext raw_gl_context = SDL_GL_CreateContext(p_window.get());
    if (nullptr == raw_gl_context) {
        const std::string sdl_error_msg { SDL_GetError() };
        throw Exception("ERROR :: GL context creation error:\n" + sdl_error_msg);
    }

    std::clog << "LOG :: GL context is initialized" << '\n';

    const int version = gladLoadGL(SDL_GL_GetProcAddress);
    std::clog << "LOG :: GLAD is initialized: OpenGL "
        << GLAD_VERSION_MAJOR(version) << '.' << GLAD_VERSION_MINOR(version) << '\n';

    if constexpr (IS_DEBUGGING_ENABLE) {
        initDebugOutput();
    }

    return GLContextPtr(raw_gl_context);
}

void chisel::swapBuffers(const WindowPtr &p_window) {
    SDL_GL_SwapWindow(p_window.get());
}

void chisel::clearWindow(const GLfloat r, const GLfloat g, const GLfloat b, const GLfloat a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void chisel::clearWindow(const glm::vec4 color) {
    clearWindow(color.r, color.g, color.b, color.a);
}

void chisel::enableVsync() {
    if (not SDL_GL_SetSwapInterval(1)) {
        throw Exception(SDL_GetError());
    }
}

void chisel::disableVsync() {
    if (not SDL_GL_SetSwapInterval(0)) {
        throw Exception(SDL_GetError());
    }
}

void chisel::lockMouseToWindow(const WindowPtr &p_window) {
    SDL_SetWindowRelativeMouseMode(p_window.get(), true);
}

void chisel::unlockMouseFromWindow(const WindowPtr &p_window) {
    SDL_SetWindowRelativeMouseMode(p_window.get(), false);
}
