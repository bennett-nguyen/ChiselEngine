#ifndef CHISEL_HPP
#define CHISEL_HPP

#include <memory>
#include <string>
#include <iostream>
#include <queue>

#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

#include <glm/vec4.hpp>

#include "engine_constants.hpp"
#include "debug_output.hpp"

namespace chisel {
    struct Destroyer {
        void operator()(SDL_Window* p) const noexcept {
            std::clog << "LOG :: GL window is destroyed\n";
            SDL_DestroyWindow(p);
        }

        void operator()(SDL_GLContext p) const noexcept {
            std::clog << "LOG :: GL context is destroyed\n";
            SDL_GL_DestroyContext(p);
        }
    };

    using WindowPtr = std::unique_ptr<SDL_Window, Destroyer>;
    using GLContextPtr = std::unique_ptr<SDL_GLContextState, Destroyer>;

    class Exception : std::exception {
        std::string error_msg {};

    public:
        explicit Exception(const std::string_view error_msg)
            : error_msg { error_msg } {}

        [[nodiscard]] const std::string& getErrorMsg() const { return error_msg; }
    };

    class System {
        static std::queue<std::pair<SDL_GLAttr, int>> saved_attrs;
        explicit System(SDL_InitFlags flags);
    public:
        ~System();

        static System& initialize(SDL_InitFlags flags);
        static void setGLWindowAttribute(SDL_GLAttr attr, int value);
        static void recheckGLAttrs();

        System(const System&)            = delete;
        System& operator=(const System&) = delete;
        System(System&&)                 = delete;
        System& operator=(System&&)      = delete;
    };

    class GUISystem {
    public:
        explicit GUISystem(const WindowPtr& p_window, const GLContextPtr& p_gl_context);
        ~GUISystem();

        GUISystem(const GUISystem&)            = delete;
        GUISystem& operator=(const GUISystem&) = delete;
        GUISystem(GUISystem&&)                 = delete;
        GUISystem& operator=(GUISystem&&)      = delete;
    };

    [[nodiscard]] WindowPtr makeGLWindow(const std::string& title);
    [[nodiscard]] GLContextPtr makeGLContext(const WindowPtr& p_window);

    void swapBuffers(const WindowPtr& p_window);
    void clearWindow(glm::vec4 color);
    void clearWindow(GLfloat r, GLfloat g, GLfloat b, GLfloat a);

    void enableVsync();
    void disableVsync();

    void lockMouseToWindow(const WindowPtr& p_window);
    void unlockMouseFromWindow(const WindowPtr& p_window);
};

#endif