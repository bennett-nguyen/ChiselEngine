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
    ShaderProgram chunk_shader("resources/shaders/chunk.vert", "resources/shaders/chunk.frag");
    Camera camera(glm::vec3(0, 0, -3.0f), glm::radians(45.0f), 0.1f, 200.0f, window.wh_ratio());

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

    World world(&camera, &chunk_shader);
    SDL_Event event;

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

            camera.pan(event);
        }

        if (SDL_GetRelativeMouseMode()) camera.move();
        camera.update_view();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        glm::vec3 camera_pos = camera.get_camera_position();
        glm::vec3 camera_front = camera.get_camera_front();

        std::string x_direction = camera_front.x >= 0 ? "North" : "South";
        std::string y_direction = camera_front.y >= 0 ? "Up" : "Down";
        std::string z_direction = camera_front.z >= 0 ? "East" : "West";

        ImGui::Begin("Debug", 0, ImGuiWindowFlags_NoMove);
        ImGui::SetWindowPos(ImVec2(0, 0), 0);
        ImGui::Text("Position: %f, %f, %f", camera_pos.x, camera_pos.y, camera_pos.z);
        ImGui::Text("Cardinal Direction: %s", camera.get_cardinal_directions().c_str());
        ImGui::Text("XYZ Direction: %s", camera.get_xyz_directions().c_str());
        ImGui::End();

        world.update();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        window.swap_buffers();
    }

    return 0;
}