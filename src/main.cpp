#include <iostream>

#include <SDL2/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <stb_image.h>

#define GLM_FORCE_CXX17
#define GLM_FORCE_EXPLICIT_CTOR

#include "video.hpp"
#include "shader.hpp"
#include "constant.hpp"
#include "camera.hpp"
#include "ray_casting.hpp"
#include "framebuffer.hpp"
#include "ray_casting.hpp"
#include "ubo_view_projection.hpp"

void loadWorld(const ChunkPosition player_position) {
    const int X_MIN = -static_cast<int>(Constant::LOAD_DISTANCE) + player_position.x;
    const int X_MAX =  static_cast<int>(Constant::LOAD_DISTANCE) + player_position.x;
    const int Z_MIN = -static_cast<int>(Constant::LOAD_DISTANCE) + player_position.z;
    const int Z_MAX =  static_cast<int>(Constant::LOAD_DISTANCE) + player_position.z;

    const ChunkPosition CORNER_1 { X_MIN, 0, Z_MIN };
    const ChunkPosition CORNER_2 { X_MAX, 0, Z_MIN };
    const ChunkPosition CORNER_3 { X_MIN, 0, Z_MAX };
    const ChunkPosition CORNER_4 { X_MAX, 0, Z_MAX };
    const ChunkPosition CENTER   { player_position.x, 0, player_position.z };

    const std::array starting_points = { CORNER_1, CORNER_2, CORNER_3, CORNER_4, CENTER };
    constexpr std::array directions = { Direction::North, Direction::South, Direction::East, Direction::West };

    std::queue<ChunkPosition> build_queue;

    for (auto const &point : starting_points) {
        if (ChunkPool::isChunkUsed(point)) continue;
        ChunkPool::use(point);
        ChunkPool::enqueueForBuilding(point);
        build_queue.push(point);
    }

    while (!build_queue.empty()) {
        ChunkPosition position = build_queue.front();
        build_queue.pop();

        for (auto const &direction : directions) {
            const ChunkPosition neighbor = position + CHUNK_NEIGHBORS_DIRECTION.at(direction);
            if (ChunkPool::isChunkUsed(neighbor)) {
                if (!ChunkPool::isBuilt(neighbor)) continue;
                ChunkPool::enqueueForRebuilding(neighbor);
                continue;
            }

            if (X_MIN > neighbor.x || neighbor.x > X_MAX || Z_MIN > neighbor.z || neighbor.z > Z_MAX) continue;

            build_queue.push(neighbor);
            ChunkPool::use(neighbor);
            ChunkPool::enqueueForBuilding(neighbor);
        }
    }
}

void removeChunks(const ChunkPosition player_position) {
    const int X_MIN = -static_cast<int>(Constant::LOAD_DISTANCE) + player_position.x;
    const int X_MAX =  static_cast<int>(Constant::LOAD_DISTANCE) + player_position.x;
    const int Z_MIN = -static_cast<int>(Constant::LOAD_DISTANCE) + player_position.z;
    const int Z_MAX =  static_cast<int>(Constant::LOAD_DISTANCE) + player_position.z;

    const auto&& used_chunks_positions = ChunkPool::getUsedChunksPositions();
    for (const auto &position : used_chunks_positions) {
        if (X_MIN <= position.x && position.x <= X_MAX &&
                Z_MIN <= position.z && position.z <= Z_MAX) continue;

        ChunkPool::free(position);
    }
}

int main(int argc, char** argv) {
    // Program Init
    Window window;

    initVideo();
    initWindow(window, "Chisel", Constant::SCREEN_OCCUPATION_RATIO);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplSDL2_InitForOpenGL(window.ptr_window, window.gl_context);
    ImGui_ImplOpenGL3_Init();

    SDL_SetRelativeMouseMode(SDL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    stbi_set_flip_vertically_on_load(true);
    setupUBOViewProjection();

    GLuint empty_vao;
    glCreateVertexArrays(1, &empty_vao);

    auto [window_width, window_height] = getWindowWidthHeight(window);

    // Screen Init
    const ShaderProgramID screen_shader_program = glCreateProgram();
    attachShader("resources/shaders/screen.vert", screen_shader_program);
    attachShader("resources/shaders/screen.frag", screen_shader_program);
    linkProgram(screen_shader_program);

    Framebuffer multisample_framebuffer, intermediate_framebuffer;

    intermediate_framebuffer.init();
    multisample_framebuffer.initMultiSample(Constant::MULTISAMPLE_LEVEL);

    intermediate_framebuffer.setSize(window_width, window_height);
    multisample_framebuffer.setSize(window_width, window_height);

    intermediate_framebuffer.setSizedInternalFormat(GL_DEPTH_COMPONENT24);
    multisample_framebuffer.setSizedInternalFormat(GL_DEPTH_COMPONENT24);

    intermediate_framebuffer.setUp();
    multisample_framebuffer.setUp();

    std::array<GLfloat, 24> SCREEN_QUAD_VERTICES = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    GLuint screen_ssbo;
    glCreateBuffers(1, &screen_ssbo);
    glNamedBufferStorage(screen_ssbo, static_cast<GLsizeiptr>(sizeof(SCREEN_QUAD_VERTICES)), SCREEN_QUAD_VERTICES.data(), 0);

    // Crosshair Init
    std::array<GLfloat, 24> CROSSHAIR_VERTICES = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    GLuint ssbo_crosshair_vertices;
    constexpr auto VERTEX_BUFFER_SIZE = static_cast<GLsizeiptr>(sizeof(CROSSHAIR_VERTICES));
    const auto VERTEX_DATA = reinterpret_cast<void*>(CROSSHAIR_VERTICES.data());

    glCreateBuffers(1, &ssbo_crosshair_vertices);
    glNamedBufferStorage(ssbo_crosshair_vertices, VERTEX_BUFFER_SIZE, VERTEX_DATA, 0);

    int width, height, numCh;
    unsigned char* bytes = stbi_load("resources/imgs/crosshair.png", &width, &height, &numCh, 0);

    GLuint crosshair_texture;
    glCreateTextures(GL_TEXTURE_2D, 1, &crosshair_texture);
    glTextureParameteri(crosshair_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(crosshair_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(crosshair_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(crosshair_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTextureStorage2D(crosshair_texture, 1, GL_RGBA8, width, height);
    glTextureSubImage2D(crosshair_texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    glGenerateTextureMipmap(crosshair_texture);
    stbi_image_free(bytes);

    constexpr float crosshair_scale = 25.0f;
    const glm::mat4 ortho_projection = glm::ortho(0.0f, static_cast<float>(window_width), static_cast<float>(window_height), 0.0f, -1.0f, 1.0f);
    auto crosshair_model = glm::translate(glm::mat4(1.0f), glm::vec3((static_cast<float>(window_width) - crosshair_scale) * 0.5f, (static_cast<float>(window_height) - crosshair_scale) * 0.5f, 0.0f));
    crosshair_model = glm::scale(crosshair_model, glm::vec3(crosshair_scale));

    const ShaderProgramID crosshair_shader_program = glCreateProgram();
    attachShader("resources/shaders/crosshair.vert", crosshair_shader_program);
    attachShader("resources/shaders/crosshair.frag", crosshair_shader_program);
    linkProgram(crosshair_shader_program);

    // World Init
    const ShaderProgramID chunk_shader_program = glCreateProgram();
    attachShader("resources/shaders/chunk.vert", chunk_shader_program);
    attachShader("resources/shaders/chunk.frag", chunk_shader_program);
    linkProgram(chunk_shader_program);

    Camera player_camera;
    RayCastResult ray_cast_result;
    ChunkPosition prev_player_position, current_player_position;

    bool is_using_cinematic_camera = false;
    bool is_switching_controls = false;
    Camera cinematic_camera = initCamera(glm::radians(60.0f), 20.0f, 500.0f, computeAspectRatio(window));
    cinematic_camera.position = glm::vec3(0, 80.0f, -10.0f);

    player_camera = initCamera(glm::radians(60.0f), 0.1f, 500.0f, computeAspectRatio(window));
    player_camera.position = glm::vec3(0.0f, 0.0f, 0.0f);
    prev_player_position = Conversion::toChunk(player_camera.position);
    current_player_position = Conversion::toChunk(player_camera.position);

    ChunkPool::init();
    loadWorld(current_player_position);

    // Texture Array
    GLuint texture_array;

    int arr_width, arr_height, arr_numCh;
    unsigned char* dirt_bytes = stbi_load("resources/imgs/block_textures/dirt.png", &arr_width, &arr_height, &arr_numCh, 0);
    unsigned char* grass_bytes = stbi_load("resources/imgs/block_textures/grass.png", &arr_width, &arr_height, &arr_numCh, 0);
    unsigned char* stone_bytes = stbi_load("resources/imgs/block_textures/stone.png", &arr_width, &arr_height, &arr_numCh, 0);
    unsigned char* test_bytes = stbi_load("resources/imgs/block_textures/test.png", &arr_width, &arr_height, &arr_numCh, 0);
    unsigned char* tnt_bytes = stbi_load("resources/imgs/block_textures/tnt.png", &arr_width, &arr_height, &arr_numCh, 0);
    unsigned char* cobblestone_bytes = stbi_load("resources/imgs/block_textures/cobblestone.png", &arr_width, &arr_height, &arr_numCh, 0);
    unsigned char* sand_bytes = stbi_load("resources/imgs/block_textures/sand.png", &arr_width, &arr_height, &arr_numCh, 0);

    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texture_array);

    glTextureParameterf(texture_array, GL_TEXTURE_MAX_ANISOTROPY, 8.0f);
    glTextureParameteri(texture_array, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(texture_array, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture_array, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage3D(texture_array, 5, GL_RGBA8, arr_width, arr_height, 7);
    glTextureSubImage3D(texture_array, 0, 0, 0, 0, arr_width, arr_height, 1, GL_RGBA, GL_UNSIGNED_BYTE, test_bytes);
    glTextureSubImage3D(texture_array, 0, 0, 0, 1, arr_width, arr_height, 1, GL_RGBA, GL_UNSIGNED_BYTE, dirt_bytes);
    glTextureSubImage3D(texture_array, 0, 0, 0, 2, arr_width, arr_height, 1, GL_RGBA, GL_UNSIGNED_BYTE, grass_bytes);
    glTextureSubImage3D(texture_array, 0, 0, 0, 3, arr_width, arr_height, 1, GL_RGBA, GL_UNSIGNED_BYTE, stone_bytes);
    glTextureSubImage3D(texture_array, 0, 0, 0, 4, arr_width, arr_height, 1, GL_RGBA, GL_UNSIGNED_BYTE, cobblestone_bytes);
    glTextureSubImage3D(texture_array, 0, 0, 0, 5, arr_width, arr_height, 1, GL_RGBA, GL_UNSIGNED_BYTE, tnt_bytes);
    glTextureSubImage3D(texture_array, 0, 0, 0, 6, arr_width, arr_height, 1, GL_RGBA, GL_UNSIGNED_BYTE, sand_bytes);

    glGenerateTextureMipmap(texture_array);

    stbi_image_free(dirt_bytes);
    stbi_image_free(grass_bytes);
    stbi_image_free(stone_bytes);
    stbi_image_free(test_bytes);
    stbi_image_free(tnt_bytes);
    stbi_image_free(cobblestone_bytes);

    // Game State
    bool enable_break_block = false;
    bool enable_place_block = false;
    bool wireframe = false;
    bool running = true;
    SDL_Event event;

    // Graphics Specs
    const GLubyte* vendor   = glGetString(GL_VENDOR);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version  = glGetString(GL_VERSION);
    const GLubyte* shading  = glGetString(GL_SHADING_LANGUAGE_VERSION);

    // Build type
    const std::string build_type = IS_DEBUGGING_ENABLE ? "Debug" : "Release";

    uint32_t now = SDL_GetTicks();
    uint32_t last = 0;
    float delta_time = 0;

    double elapsed_time = 0;
    double average_elapsed_time = 0;
    int update_counter = 0;
    constexpr int UPDATE_FREQUENCY = 5;

    uint64_t start, end;

    while (running) {
        start = SDL_GetPerformanceCounter();

        last = now;
        now = SDL_GetTicks();
        delta_time = (static_cast<float>(now) - static_cast<float>(last)) / 1000.0f;

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
                    wireframe = not wireframe;
                } else if (SDL_SCANCODE_E == event.key.keysym.scancode) {
                    is_using_cinematic_camera = !is_using_cinematic_camera;
                } else if (SDL_SCANCODE_R == event.key.keysym.scancode) {
                    is_switching_controls = !is_switching_controls;
                }
            }

            if (SDL_MOUSEBUTTONDOWN == event.type) {
                if (SDL_BUTTON_LEFT == event.button.button) {
                    enable_break_block = true;
                } else if (SDL_BUTTON_RIGHT == event.button.button) {
                    enable_place_block = true;
                }
            }
            if (not is_switching_controls) {
                pan(player_camera, event);
            } else {
                pan(cinematic_camera, event);
            }
        }

        if (not is_switching_controls) {
            move(player_camera, delta_time);
        } else {
            move(cinematic_camera, delta_time);
        }

        updateView(player_camera);
        updateView(cinematic_camera);

        if (not is_using_cinematic_camera) {
            setView(player_camera.view_mat);
            setProjection(player_camera.projection_mat);
        } else {
            setView(cinematic_camera.view_mat);
            setProjection(cinematic_camera.projection_mat);
        }

        if (enable_break_block or enable_place_block) {
            rayCast(ray_cast_result, player_camera.position, player_camera.front);

            if (ray_cast_result.is_detected_voxel) {
                if (enable_break_block) {
                    breakBlock(ray_cast_result.detected_voxel_position);
                } else {
                    placeBlock(getAdjacentVoxel(ray_cast_result));
                }
            }
        }

        current_player_position = Conversion::toChunk(player_camera.position);

        if (prev_player_position.x != current_player_position.x
            or prev_player_position.z != current_player_position.z) {
            removeChunks(current_player_position);
            loadWorld(current_player_position);
            prev_player_position = current_player_position;
        }

        ChunkPool::buildQueuedChunks();
        ChunkPool::rebuildQueuedChunks();

        multisample_framebuffer.bind();
        clearWindow({ 0.45490f, 0.70196f, 1.0f, 1.0f });
        glEnable(GL_DEPTH_TEST);

        activateShaderProgram(chunk_shader_program);
        glBindTextureUnit(0, texture_array);
        bindUBOViewProjection();

        if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        const auto&& frustum_planes = getFrustumPlanes(player_camera);
        const auto&& used_chunks_positions = ChunkPool::getUsedChunksPositions();

        for (const auto position : used_chunks_positions) {
            if (not ChunkPool::isVisible(position, frustum_planes)) continue;
            uniformMat4f(chunk_shader_program, "model", 1, GL_FALSE, Conversion::toChunkModel(position));
            ChunkPool::render(position);
        }

        multisample_framebuffer.blitTo(intermediate_framebuffer);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(empty_vao);
        activateShaderProgram(screen_shader_program);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, screen_ssbo);
        glDisable(GL_DEPTH_TEST);
        glBindTextureUnit(0, intermediate_framebuffer.getTextureName());
        glDrawArrays(GL_TRIANGLES, 0, 6);

        activateShaderProgram(crosshair_shader_program);
        glBindVertexArray(empty_vao);
        glBindTextureUnit(0, crosshair_texture);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_crosshair_vertices);
        uniformMat4f(crosshair_shader_program, "model", 1, GL_FALSE, crosshair_model);
        uniformMat4f(crosshair_shader_program, "ortho_projection", 1, GL_FALSE, ortho_projection);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        end = SDL_GetPerformanceCounter();
        elapsed_time += static_cast<double>(end - start) / static_cast<double>(SDL_GetPerformanceFrequency());
        update_counter++;

        if (UPDATE_FREQUENCY-1 == update_counter) {
            average_elapsed_time = elapsed_time / static_cast<double>(UPDATE_FREQUENCY);
            update_counter = 0;
            elapsed_time = 0;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Debug", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SetWindowPos(ImVec2(0, 0), 0);

        ImGui::Text("Avg. Frame Generation Time - %d frame(s): %.3lf ms", UPDATE_FREQUENCY, average_elapsed_time * 1000.0f);
        ImGui::Text("Coordinates: %f, %f, %f", player_camera.position.x, player_camera.position.y, player_camera.position.z);
        ImGui::Text("Cardinal Direction: %s", getCardinalDirections(player_camera).c_str());

        ImGui::NewLine();

        ImGui::Text("Chisel Build: %s %s", Constant::VERSION.c_str(), build_type.c_str());
        ImGui::Text("GPU Vendor: %s",               vendor   ? reinterpret_cast<const char*>(vendor)   : "Unknown");
        ImGui::Text("Version: %s",                  version  ? reinterpret_cast<const char*>(version)  : "Unknown");
        ImGui::Text("Renderer: %s",                 renderer ? reinterpret_cast<const char*>(renderer) : "Unknown");
        ImGui::Text("Shading Language Version: %s", shading  ? reinterpret_cast<const char*>(shading)  : "Unknown");

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        swapBuffers(window);
    }

    ChunkPool::destroy();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    destroyWindow(window);
    SDL_Quit();

    return 0;
}
