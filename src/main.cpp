#include <SDL2/SDL.h>
#include "coreutils.hpp"
#include "blocktype_enum.hpp"
#include "chunk.hpp"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

int main() {
    Window window(
        "Terrain Generation",
        Constant::SCREEN_OCCUPATION_RATIO,
        SDL_INIT_VIDEO,
        SDL_WINDOW_OPENGL
    );

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplSDL2_InitForOpenGL(window.get_window(), window.get_gl_context());
    ImGui_ImplOpenGL3_Init();

    glEnable(GL_DEPTH_TEST);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    Camera camera(glm::vec3(0, 0, -3.0f), glm::radians(45.0f), 0.1f, 500.0f, window.wh_ratio());
    ShaderProgram default_shader_program("resources/shaders/default.vert", "resources/shaders/default.frag");

    SDL_Event event;
    glm::mat4 model(1.0f);

    std::vector<Chunk*> chunks;

    for (unsigned x = 0; x < 25; x++) {
        for (unsigned z = 0; z < 25; z++) {
            Chunk* pchunk = new Chunk(glm::vec3(x*Chunk::CHUNK_SIZE, 0, z*Chunk::CHUNK_SIZE));
            pchunk->load_chunk();
            pchunk->setup_chunk();
            chunks.push_back(pchunk);
        }
    }

    while (true) {
        window.clear(Colors::DARK_SLATE_GRAY);

        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            camera.pan(event);

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
                }
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        glm::vec3 camera_pos = camera.get_camera_position();
        glm::vec3 camera_front = camera.get_camera_front();
        ImGui::Begin("Debug", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ImGui::SetWindowPos(ImVec2(0, 0), 0);
        ImGui::Text("Position: %f, %f, %f", camera_pos.x, camera_pos.y, camera_pos.z);
        ImGui::Text("Direction: %f, %f, %f", camera_front.x, camera_front.y, camera_front.z);
        ImGui::Text("Facing: %s", camera_front.y >= 0 ? "Up" : "Down");
        ImGui::End();

        camera.move();
        camera.update_view();

        default_shader_program.activate();
        default_shader_program.uniform_mat4f("model", 1, GL_FALSE, glm::mat4(1.0f));
        default_shader_program.uniform_mat4f("view", 1, GL_FALSE, camera.get_view_mat());
        default_shader_program.uniform_mat4f("projection", 1, GL_FALSE, camera.get_projection_mat());

        for (Chunk* pchunk : chunks) {
            pchunk->render(default_shader_program, Colors::WHITE, Colors::BLACK);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        window.swap_buffers();
    }

    return 0;
}
