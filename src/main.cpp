#include <SDL2/SDL.h>
#include "window.hpp"
#include "camera.hpp"
#include "colors.hpp"
#include "shader_program.hpp"
#include "constant.hpp"
#include "world.hpp"
#include "debug_window.hpp"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

#include <stb_image.h>

Uint64 now = SDL_GetPerformanceCounter();
Uint64 last = 0;
float delta_time = 0;

GLuint crosshair_vbo;
GLuint crosshair_vao;

int main(int argc, char** argv) {
    Window window("Chisel", Constant::SCREEN_OCCUPATION_RATIO, SDL_INIT_VIDEO, SDL_WINDOW_OPENGL);
    ShaderProgram crosshair_shader("resources/shaders/crosshair.vert", "resources/shaders/crosshair.frag");
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
    glm::mat4 ortho_projection = glm::ortho(0.0f, (float)window.getWidth(), (float)window.getHeight(), 0.0f, -1.0f, 1.0f); 

    GLfloat vertices[] = { 
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 
    
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    float border_color[] = {
        0, 0, 0, 1.0f
    };

    glGenBuffers(1, &crosshair_vbo);
    glGenVertexArrays(1, &crosshair_vao);

    glBindBuffer(GL_ARRAY_BUFFER, crosshair_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(crosshair_vao);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    int width, height, numCh;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* bytes = stbi_load("resources/imgs/crosshair.png", &width, &height, &numCh, 0);

    GLuint crosshair_texture;
    glGenTextures(1, &crosshair_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, crosshair_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER, border_color);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(bytes);

    float crosshair_scale = 25.0f;
    while (true) {
        window.clear(Colors::SKY_BLUE);

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

        glm::mat4 crosshair_model;
        
        crosshair_model = glm::translate(glm::mat4(1.0f), glm::vec3(((float)window.getWidth() - crosshair_scale) * 0.5f, ((float)window.getHeight() - crosshair_scale) * 0.5f, 0.0f));
        crosshair_model = glm::scale(crosshair_model, glm::vec3(crosshair_scale));

        crosshair_shader.activate();
        glBindVertexArray(crosshair_vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, crosshair_texture);
        crosshair_shader.uniformMat4f("model", 1, GL_FALSE, crosshair_model);
        crosshair_shader.uniformMat4f("ortho_projection", 1, GL_FALSE, ortho_projection);
        crosshair_shader.uniformInt("tex", 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        debug_window.show();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        window.swapBuffers();
    }

    return 0;
}