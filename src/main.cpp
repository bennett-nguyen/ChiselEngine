#include <SDL2/SDL.h>
#include "window.hpp"
#include "camera.hpp"
#include "colors.hpp"
#include "shader_program.hpp"
#include "constant.hpp"
#include "world.hpp"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

int main() {
    Window window("OpenGL Window", Constant::SCREEN_OCCUPATION_RATIO, SDL_INIT_VIDEO, SDL_WINDOW_OPENGL);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplSDL2_InitForOpenGL(window.get_window(), window.get_gl_context());
    ImGui_ImplOpenGL3_Init();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    World world(window.wh_ratio());
    SDL_Event event;

    // int width, height, nchannels;
    // unsigned char *bytes = stbi_load("resources/imgs/texture_array.png", &width, &height, &nchannels, 0);
    // int num_layers = height / 32;

    // GLuint texture_array;
    // glGenTextures(1, &texture_array);
    // glBindTexture(GL_TEXTURE_2D_ARRAY, texture_array);
    // glTexStorage3D(GL_TEXTURE_2D_ARRAY, )

    while (true) {
        window.clear(Colors::DARK_SLATE_GRAY);

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

            world.poll_event(event);
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        world.debug_window();
        world.update();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        window.swap_buffers();
    }

    return 0;
}