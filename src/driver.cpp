#include "constants.hpp"
#include "window_loader.hpp"
#include "shader_loader.hpp"
#include "camera.hpp"
// this is test branch

int main() {
    Window window("Graphics", Constant::WIDTH, Constant::HEIGHT,
        SDL_INIT_VIDEO, SDL_WINDOW_OPENGL);
    window.apply_default_settings();

    Shader shader("./resources/shader/default.vs", "./resources/shader/default.fs");
    Camera camera(glm::radians(45.0f), 0.1f, 100.0f);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    unsigned VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindVertexArray(VAO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    bool running = true;
    SDL_Event event;

    while (running) {
        window.clear(0.2f, 0.3f, 0.3f, 1.0f);

        shader.activate();
        shader.uniform_mat4f("model", 1, GL_FALSE, camera.get_model_mat());
        shader.uniform_mat4f("view", 1, GL_FALSE, camera.get_view_mat());
        shader.uniform_mat4f("projection", 1, GL_FALSE, camera.get_projection_mat());

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        while (SDL_PollEvent(&event)) {
            if (SDL_QUIT == event.type) {
                running = false;
                SDL_Quit();
            } if (SDL_KEYDOWN == event.type) {
                if (SDL_SCANCODE_TAB == event.key.keysym.scancode) {
                    if (SDL_GetRelativeMouseMode()) {
                        SDL_SetRelativeMouseMode(SDL_FALSE);
                    } else {
                        SDL_SetRelativeMouseMode(SDL_TRUE);
                    }
                }
            }

            camera.pan(event);
        }

        camera.move();
        camera.update_view();
        window.swap_buffers();
    }

    return 0;
}