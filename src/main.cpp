#include <SDL2/SDL.h>
#include "window.hpp"
#include "camera.hpp"
#include "colors.hpp"
#include "shader_program.hpp"
#include "constant.hpp"
#include "world.hpp"
#include "debug_window.hpp"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

Uint64 now = SDL_GetPerformanceCounter();
Uint64 last = 0;
float delta_time = 0;

int main() {
    Window window("OpenGL Window", Constant::SCREEN_OCCUPATION_RATIO, SDL_INIT_VIDEO, SDL_WINDOW_OPENGL);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplSDL2_InitForOpenGL(window.getWindowPointer(), window.getGLContext());
    ImGui_ImplOpenGL3_Init();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    World world(window.getWidthHeightRatio());
    SDL_Event event;

    DebugWindow debug_window(world.getPlayerPointer());

    while (true) {
        window.clear(Colors::DARK_SLATE_GRAY);

        last = now;
        now = SDL_GetPerformanceCounter();
        delta_time = (float)((now - last)*1000 / (float)SDL_GetPerformanceFrequency() ) * 0.001f;

        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (SDL_QUIT == event.type) {
                ImGui_ImplOpenGL3_Shutdown();
                ImGui_ImplSDL2_Shutdown();
                ImGui::DestroyContext();
                SDL_Quit();
                return 0;
            } else if (SDL_KEYDOWN == event.type) {
                if (SDL_SCANCODE_TAB == event.key.keysym.scancode) {
                    if (SDL_GetRelativeMouseMode()) {
                        SDL_SetRelativeMouseMode(SDL_FALSE);
                    } else {
                        SDL_SetRelativeMouseMode(SDL_TRUE);
                    }
                } else if (SDL_SCANCODE_Q == event.key.keysym.scancode) {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glLineWidth(1.4f);
                } else if (SDL_SCANCODE_E == event.key.keysym.scancode) {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }
            }

            world.pollEvent(event, delta_time);
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();


        world.update(delta_time);

        debug_window.show();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        window.swapBuffers();
    }

    return 0;
}