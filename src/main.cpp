#include <SDL2/SDL.h>
#include "coreutils.hpp"
#include "blocktype_enum.hpp"

int main() {
    Window window(
        "Terrain Generation",
        Constant::SCREEN_OCCUPATION_RATIO,
        SDL_INIT_VIDEO,
        SDL_WINDOW_OPENGL
    );

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    Camera camera(glm::vec3(0, 0, -3.0f), glm::radians(45.0f), 0.1f, 50.0f, window.wh_ratio());
    ShaderProgram default_shader_program("resources/shaders/default.vert", "resources/shaders/default.frag");

    GLfloat cube[] = {
        // positions
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };

    VBO cube_vbo;
    VAO cube_vao;

    cube_vao.bind();
    cube_vbo.bind();

    cube_vbo.buffer_data(sizeof(cube), cube, GL_STATIC_DRAW);
    cube_vbo.attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    cube_vbo.enable_attrib_array(0);

    SDL_Event event;

    glm::mat4 model(1.0f);

    while (true) {
        window.clear(Colors::DARK_SLATE_GRAY);

        while (SDL_PollEvent(&event)) {
            camera.pan(event);

            if (SDL_QUIT == event.type) {
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

        camera.move();
        camera.update_view();

        default_shader_program.activate();
        default_shader_program.uniform_vec4("color", Colors::ORANGE_CHOCOLATE);
        default_shader_program.uniform_mat4f("view", 1, GL_FALSE, camera.get_view_mat());
        default_shader_program.uniform_mat4f("projection", 1, GL_FALSE, camera.get_projection_mat());
        cube_vao.bind();

        for (int y = 0; y <= 5; y++) {
            for (int x = 0; x <= 5; x++) {
                for (int z = 0; z <= 5; z++) {
                    model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
                    default_shader_program.uniform_mat4f("model", 1, GL_FALSE, model);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }

        window.swap_buffers();
    }

    return 0;
}