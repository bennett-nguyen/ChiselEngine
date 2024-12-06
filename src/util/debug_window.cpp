#include "debug_window.hpp"

constexpr float FPS_INTERVAL = 1.0f; //seconds.

Uint32 fps_lasttime = SDL_GetTicks(); //the last recorded time.
Uint32 fps_current; //the current FPS.
Uint32 fps_frames = 0; //frames passed since the last recorded fps.

void computeFPS() {
    fps_frames++;

    if (fps_lasttime < SDL_GetTicks() - FPS_INTERVAL*1000) {
        fps_lasttime = SDL_GetTicks();
        fps_current = fps_frames;
        fps_frames = 0;
    }
}

DebugWindow::DebugWindow(Player *p_player)
    : mp_player(p_player) {}

void DebugWindow::show() {
    computeFPS();
    Camera camera = mp_player->m_camera;
    glm::vec3 player_pos = mp_player->getPosition();
    glm::ivec3 chunk_coords = VoxelMath::getChunkCoordsFromPos(player_pos);

    ImGui::Begin("Debug", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetWindowPos(ImVec2(0, 0), 0);

    ImGui::Text("FPS: %u", fps_current);
    ImGui::Text("Coordinates: %f, %f, %f", player_pos.x, player_pos.y, player_pos.z);
    ImGui::Text("Chunk Coordinates: %d, %d, %d", chunk_coords.x, chunk_coords.y, chunk_coords.z);
    ImGui::Text("Cardinal Direction: %s", camera.getCardinalDirections().c_str());
    ImGui::Text("XYZ Direction: %s", camera.getXYZ_Directions().c_str());

    ImGui::NewLine();

    ImGui::Text("GPU Vendor: %s", glGetString(GL_VENDOR));
    ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
    ImGui::Text("Version: %s", glGetString(GL_VERSION));
    ImGui::Text("Shading Language Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    ImGui::End();
}