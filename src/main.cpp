#include <SDL2/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <stb_image.h>

#include "video.hpp"
#include "world.hpp"
#include "constant.hpp"
#include "ray_casting.hpp"
#include "debug_output.hpp"

int main(int argc, char** argv) {
    Window window;

    initVideo();
    requestDebugContext();
    initWindow(window, "Chisel", Constant::SCREEN_OCCUPATION_RATIO);
    initDebugOutput();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplSDL2_InitForOpenGL(window.ptr_window, window.gl_context);
    ImGui_ImplOpenGL3_Init();

    SDL_SetRelativeMouseMode(SDL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    World world;
    RayCastResult ray_cast_result;

    world.camera = initCamera(glm::radians(60.0f), 0.1f, 150.0f, computeAspectRatio(window));
    world.camera.position = glm::vec3(0, 0, -1.0f);
    world.prev_player_chunk_pos = world.camera.position;

    // initWorld(world);
    initChunkShader(world, "resources/shaders/chunk.vert", "resources/shaders/chunk.frag");
    loadChunks(world);

    bool enable_break_block = false;
    bool enable_place_block = false;
    bool wireframe = false;
    bool running = true;
    SDL_Event event;

    const GLubyte* vendor   = glGetString(GL_VENDOR);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version  = glGetString(GL_VERSION);
    const GLubyte* shading  = glGetString(GL_SHADING_LANGUAGE_VERSION);

    const ShaderID crosshair_vshader = makeShader("resources/shaders/crosshair.vert", GL_VERTEX_SHADER);
    const ShaderID crosshair_fshader = makeShader("resources/shaders/crosshair.frag", GL_FRAGMENT_SHADER);
    const ShaderProgramID crosshair_shader_program = glCreateProgram();
    glAttachShader(crosshair_shader_program, crosshair_vshader);
    glAttachShader(crosshair_shader_program, crosshair_fshader);
    glLinkProgram(crosshair_shader_program);
    programLinkingCheck(crosshair_shader_program);
    glDeleteShader(crosshair_vshader);
    glDeleteShader(crosshair_fshader);

    GLfloat crosshair_vertices[] = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    GLuint empty_vao, ssbo_crosshair_vertices;
    constexpr auto VERTEX_BUFFER_SIZE = static_cast<GLsizeiptr>(sizeof(crosshair_vertices));
    const auto VERTEX_DATA = reinterpret_cast<void*>(crosshair_vertices);

    glCreateBuffers(1, &ssbo_crosshair_vertices);
    glCreateVertexArrays(1, &empty_vao);
    glNamedBufferStorage(ssbo_crosshair_vertices, VERTEX_BUFFER_SIZE, VERTEX_DATA, 0);

    int width, height, numCh;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* bytes = stbi_load("resources/imgs/crosshair.png", &width, &height, &numCh, 0);

    GLuint crosshair_texture;
    glCreateTextures(GL_TEXTURE_2D, 1, &crosshair_texture);
    glBindTextureUnit(0, crosshair_texture);

    glTextureParameteri(crosshair_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(crosshair_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(crosshair_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(crosshair_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTextureStorage2D(crosshair_texture, 1, GL_RGBA8, width, height);
    glTextureSubImage2D(crosshair_texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    glGenerateTextureMipmap(crosshair_texture);
    stbi_image_free(bytes);

    int window_width, window_height;
    SDL_GetWindowSize(window.ptr_window, &window_width, &window_height);

    float crosshair_scale = 25.0f;
    glm::mat4 ortho_projection = glm::ortho(0.0f, (float)window_width, (float)window_height, 0.0f, -1.0f, 1.0f);
    auto crosshair_model = glm::translate(glm::mat4(1.0f), glm::vec3(((float)window_width - crosshair_scale) * 0.5f, ((float)window_height - crosshair_scale) * 0.5f, 0.0f));
    crosshair_model = glm::scale(crosshair_model, glm::vec3(crosshair_scale));

    while (running) {
        clearWindow(0.45490f, 0.70196f, 1.0f, 1.0f);

        enable_break_block = false;
        enable_place_block = false;

        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (SDL_QUIT == event.type) {
                running = false;
            } 

            if (SDL_KEYDOWN == event.type) {
                if (SDL_SCANCODE_TAB == event.key.keysym.scancode) {
                    if (SDL_GetRelativeMouseMode()) {
                        SDL_SetRelativeMouseMode(SDL_FALSE);
                    } else {
                        SDL_SetRelativeMouseMode(SDL_TRUE);
                    }
                } else if (SDL_SCANCODE_Q == event.key.keysym.scancode) {
                    wireframe = !wireframe;

                    if (wireframe) {
                        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    } else {
                        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    }
                }
            }

            if (SDL_MOUSEBUTTONDOWN == event.type) {
                if (SDL_BUTTON_LEFT == event.button.button) {
                    enable_break_block = true;
                } else if (SDL_BUTTON_RIGHT == event.button.button) {
                    enable_place_block = true;
                }
            }

            pan(world.camera, event);
        }

        move(world.camera);

        auto current_player_chunk_pos = Conversion::toChunk(world.camera.position);

        if (world.prev_player_chunk_pos.x != current_player_chunk_pos.x
                || world.prev_player_chunk_pos.z != current_player_chunk_pos.z) {
            removeChunks(world);
            loadChunks(world);
            rebuildChunks(world);
            world.prev_player_chunk_pos = current_player_chunk_pos;
        }

        rayCast(ray_cast_result, world, world.camera.position, world.camera.front);

        if (ray_cast_result.is_detected_voxel) {
            if (enable_break_block) {
                breakBlock(world, ray_cast_result.detected_voxel_world_pos);
            } else if (enable_place_block) {
                placeBlock(world, getAdjacentVoxel(ray_cast_result));
            }
        }

        renderWorld(world);

        activateShaderProgram(crosshair_shader_program);
        glBindVertexArray(empty_vao);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_crosshair_vertices);
        uniformMat4f(crosshair_shader_program, "model", 1, GL_FALSE, crosshair_model);
        uniformMat4f(crosshair_shader_program, "ortho_projection", 1, GL_FALSE, ortho_projection);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Debug", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SetWindowPos(ImVec2(0, 0), 0);

        ImGui::Text("Coordinates: %f, %f, %f", world.camera.position.x, world.camera.position.y, world.camera.position.z);
        ImGui::Text("Cardinal Direction: %s", getCardinalDirections(world.camera).c_str());

        ImGui::NewLine();

        ImGui::Text("GPU Vendor: %s",               vendor   ? reinterpret_cast<const char*>(vendor)   : "Unknown");
        ImGui::Text("Version: %s",                  version  ? reinterpret_cast<const char*>(version)  : "Unknown");
        ImGui::Text("Renderer: %s",                 renderer ? reinterpret_cast<const char*>(renderer) : "Unknown");
        ImGui::Text("Shading Language Version: %s", shading  ? reinterpret_cast<const char*>(shading)  : "Unknown");

        ImGui::End();

        updateView(world.camera);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        swapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    destroyWindow(window);
    SDL_Quit();

    return 0;
}