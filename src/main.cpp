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

    GLuint empty_vao;
    glCreateVertexArrays(1, &empty_vao);

    int window_width, window_height;
    SDL_GetWindowSize(window.ptr_window, &window_width, &window_height);

    const ShaderID screen_vshader = makeShader("resources/shaders/screen.vert", GL_VERTEX_SHADER);
    const ShaderID screen_fshader = makeShader("resources/shaders/screen.frag", GL_FRAGMENT_SHADER);
    const ShaderProgramID screen_shader_program = glCreateProgram();
    glAttachShader(screen_shader_program, screen_vshader);
    glAttachShader(screen_shader_program, screen_fshader);
    glLinkProgram(screen_shader_program);
    programLinkingCheck(screen_shader_program);
    glDeleteShader(screen_vshader);
    glDeleteShader(screen_fshader);

    GLuint multisampled_texture_color_buffer, intermediate_texture_color_buffer;
    glCreateTextures(GL_TEXTURE_2D, 1, &intermediate_texture_color_buffer);
    glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &multisampled_texture_color_buffer);
    glTextureStorage2DMultisample(multisampled_texture_color_buffer, Constant::MULTISAMPLE_LEVEL, GL_RGB8, window_width, window_height, GL_TRUE);

    glBindTexture(GL_TEXTURE_2D, intermediate_texture_color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTextureParameteri(intermediate_texture_color_buffer, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(intermediate_texture_color_buffer, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLuint multisampled_renderbuffer, intermediate_renderbuffer;
    glCreateRenderbuffers(1, &multisampled_renderbuffer);
    glCreateRenderbuffers(1, &intermediate_renderbuffer);
    glNamedRenderbufferStorage(intermediate_renderbuffer, GL_DEPTH_COMPONENT24, window_width, window_height);
    glNamedRenderbufferStorageMultisample(multisampled_renderbuffer, Constant::MULTISAMPLE_LEVEL, GL_DEPTH_COMPONENT24, window_width, window_height);

    GLuint multisampled_framebuffer, intermediate_framebuffer;
    glCreateFramebuffers(1, &multisampled_framebuffer);
    glCreateFramebuffers(1, &intermediate_framebuffer);

    glNamedFramebufferTexture(multisampled_framebuffer, GL_COLOR_ATTACHMENT0, multisampled_texture_color_buffer, 0);
    glNamedFramebufferRenderbuffer(multisampled_framebuffer, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, multisampled_renderbuffer);

    glNamedFramebufferTexture(intermediate_framebuffer, GL_COLOR_ATTACHMENT0, intermediate_texture_color_buffer, 0);
    glNamedFramebufferRenderbuffer(intermediate_framebuffer, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, intermediate_renderbuffer);

    if (glCheckNamedFramebufferStatus(multisampled_framebuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || glCheckNamedFramebufferStatus(intermediate_framebuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << '\n';
        exit(1);
    }

    float screen_quad_vertices[] = {
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
    glNamedBufferStorage(screen_ssbo, static_cast<GLsizeiptr>(sizeof(screen_quad_vertices)), screen_quad_vertices, 0);

    GLuint ubo_view_projection;
    glCreateBuffers(1, &ubo_view_projection);
    glNamedBufferStorage(ubo_view_projection, static_cast<GLsizeiptr>(2 * sizeof(glm::mat4)), nullptr, GL_DYNAMIC_STORAGE_BIT);

    World world;
    RayCastResult ray_cast_result;

    bool is_using_cinematic_camera = false;
    bool is_switching_controls = false;
    Camera cinematic_camera = initCamera(glm::radians(60.0f), 20.0f, 500.0f, computeAspectRatio(window));
    cinematic_camera.position = glm::vec3(0, 80.0f, -10.0f);

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

    GLuint ssbo_crosshair_vertices;
    constexpr auto VERTEX_BUFFER_SIZE = static_cast<GLsizeiptr>(sizeof(crosshair_vertices));
    const auto VERTEX_DATA = reinterpret_cast<void*>(crosshair_vertices);

    glCreateBuffers(1, &ssbo_crosshair_vertices);
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

    float crosshair_scale = 25.0f;
    glm::mat4 ortho_projection = glm::ortho(0.0f, (float)window_width, (float)window_height, 0.0f, -1.0f, 1.0f);
    auto crosshair_model = glm::translate(glm::mat4(1.0f), glm::vec3(((float)window_width - crosshair_scale) * 0.5f, ((float)window_height - crosshair_scale) * 0.5f, 0.0f));
    crosshair_model = glm::scale(crosshair_model, glm::vec3(crosshair_scale));

    GLuint texture_array;
    int arr_width, arr_height, arr_numCh;
    unsigned char* dirt_bytes = stbi_load("resources/imgs/block_textures/dirt.png", &arr_width, &arr_height, &arr_numCh, 0);
    unsigned char* grass_bytes = stbi_load("resources/imgs/block_textures/grass.png", &arr_width, &arr_height, &arr_numCh, 0);
    unsigned char* stone_bytes = stbi_load("resources/imgs/block_textures/stone.png", &arr_width, &arr_height, &arr_numCh, 0);
    unsigned char* test_bytes = stbi_load("resources/imgs/block_textures/test.png", &arr_width, &arr_height, &arr_numCh, 0);
    unsigned char* tnt_bytes = stbi_load("resources/imgs/block_textures/tnt.png", &arr_width, &arr_height, &arr_numCh, 0);
    unsigned char* cobblestone_bytes = stbi_load("resources/imgs/block_textures/cobblestone.png", &arr_width, &arr_height, &arr_numCh, 0);

    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texture_array);

    glTextureParameteri(texture_array, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(texture_array, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(texture_array, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture_array, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    const auto NUM_MIPS = static_cast<GLsizei>(std::floor(std::log2(std::max(arr_width, arr_height))));
    std::cout << NUM_MIPS << '\n';

    glTextureStorage3D(texture_array, NUM_MIPS, GL_RGBA8, arr_width, arr_height, 6);
    glTextureSubImage3D(texture_array, 0, 0, 0, 0, arr_width, arr_height, 1, GL_RGBA, GL_UNSIGNED_BYTE, test_bytes);
    glTextureSubImage3D(texture_array, 0, 0, 0, 1, arr_width, arr_height, 1, GL_RGBA, GL_UNSIGNED_BYTE, dirt_bytes);
    glTextureSubImage3D(texture_array, 0, 0, 0, 2, arr_width, arr_height, 1, GL_RGBA, GL_UNSIGNED_BYTE, grass_bytes);
    glTextureSubImage3D(texture_array, 0, 0, 0, 3, arr_width, arr_height, 1, GL_RGBA, GL_UNSIGNED_BYTE, stone_bytes);
    glTextureSubImage3D(texture_array, 0, 0, 0, 4, arr_width, arr_height, 1, GL_RGBA, GL_UNSIGNED_BYTE, cobblestone_bytes);
    glTextureSubImage3D(texture_array, 0, 0, 0, 5, arr_width, arr_height, 1, GL_RGBA, GL_UNSIGNED_BYTE, tnt_bytes);
    glGenerateTextureMipmap(texture_array);

    stbi_image_free(dirt_bytes);
    stbi_image_free(grass_bytes);
    stbi_image_free(stone_bytes);
    stbi_image_free(test_bytes);
    stbi_image_free(tnt_bytes);
    stbi_image_free(cobblestone_bytes);

    while (running) {
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
            if (!is_switching_controls) {
                pan(world.camera, event);
            } else {
                pan(cinematic_camera, event);
            }
        }

        if (!is_switching_controls) {
            move(world.camera);
        } else {
            move(cinematic_camera);
        }

        updateView(world.camera);
        updateView(cinematic_camera);

        glm::mat4 view, projection;
        if (!is_using_cinematic_camera) {
            view = world.camera.view_mat;
            projection = world.camera.projection_mat;
        } else {
            view = cinematic_camera.view_mat;
            projection = cinematic_camera.projection_mat;
        }

        glNamedBufferSubData(ubo_view_projection, 0, sizeof(glm::mat4), glm::value_ptr(view));
        glNamedBufferSubData(ubo_view_projection, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));

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

        glBindFramebuffer(GL_FRAMEBUFFER, multisampled_framebuffer);
        clearWindow(0.45490f, 0.70196f, 1.0f, 1.0f);
        glEnable(GL_DEPTH_TEST);

        activateShaderProgram(world.chunk_shader_program);
        glBindTextureUnit(0, texture_array);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_view_projection);

        std::vector<glm::vec4> frustum_planes = getFrustumPlanes(world.camera);

        for (const auto &[_, ptr_chunk] : world.chunk_map) {
            if (!isChunkVisible(ptr_chunk, frustum_planes)) continue;
            uniformMat4f(world.chunk_shader_program, "model", 1, GL_FALSE, getChunkModel(ptr_chunk));
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ptr_chunk->mesh.ssbo_vertices);
            renderChunk(ptr_chunk);
        }

        glBlitNamedFramebuffer(multisampled_framebuffer, intermediate_framebuffer, 0, 0, window_width, window_height, 0, 0, window_width, window_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(empty_vao);
        activateShaderProgram(screen_shader_program);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, screen_ssbo);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, intermediate_texture_color_buffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        activateShaderProgram(crosshair_shader_program);
        glBindVertexArray(empty_vao);
        glBindTextureUnit(0, crosshair_texture);
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