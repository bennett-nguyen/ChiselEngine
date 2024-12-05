#include "world.hpp"

#define FPS_INTERVAL 1.0 //seconds.

Uint32 fps_lasttime = SDL_GetTicks(); //the last recorded time.
Uint32 fps_current; //the current FPS.
Uint32 fps_frames = 0; //frames passed since the last recorded fps.

World::World(float wh_ratio)
    : m_voxel_handler(&m_chunk_map) {
    m_chunk_shader = ShaderProgram("resources/shaders/chunk.vert", "resources/shaders/chunk.frag");
    m_player.init_camera(wh_ratio);
    m_prev_player_chunk_pos = m_player.get_player_chunk_coords();
    load_chunks();
}

World::~World() {
    for (auto &chunk : m_chunk_map) {
        delete chunk.second;
    }

    m_chunk_map.clear();
}

void World::build_chunk(glm::ivec3 chunk_coords) {
    if (1 != m_chunk_map.count(chunk_coords)) return;
    glm::ivec3 north_coord(chunk_coords.x+1, chunk_coords.y, chunk_coords.z);
    glm::ivec3 south_coord(chunk_coords.x-1, chunk_coords.y, chunk_coords.z);
    glm::ivec3 east_coord(chunk_coords.x, chunk_coords.y, chunk_coords.z+1);
    glm::ivec3 west_coord(chunk_coords.x, chunk_coords.y, chunk_coords.z-1);

    unsigned *north_neighbor_pvoxels = get_chunk_neighbor_pvoxels(north_coord);
    unsigned *south_neighbor_pvoxels = get_chunk_neighbor_pvoxels(south_coord);
    unsigned *east_neighbor_pvoxels = get_chunk_neighbor_pvoxels(east_coord);
    unsigned *west_neighbor_pvoxels = get_chunk_neighbor_pvoxels(west_coord);

    m_chunk_map[chunk_coords]->build_mesh(north_neighbor_pvoxels, south_neighbor_pvoxels, east_neighbor_pvoxels, west_neighbor_pvoxels);
}

void World::rebuild_chunk(glm::ivec3 chunk_coords) {
    if (1 != m_chunk_map.count(chunk_coords)) return;
    m_chunk_map[chunk_coords]->destroy_mesh();
    build_chunk(chunk_coords);
}

void World::update() {
    m_player.move();
    m_player.m_camera.update_view();
    glm::ivec3 current_player_chunk_pos = m_player.get_player_chunk_coords();

    if (m_prev_player_chunk_pos.x != current_player_chunk_pos.x
            || m_prev_player_chunk_pos.z != current_player_chunk_pos.z) {
        remove_chunks();
        load_chunks();
        rebuild_chunks();

        m_prev_player_chunk_pos = current_player_chunk_pos;
    }

    Camera camera = m_player.m_camera;
    m_voxel_handler.ray_cast(camera.get_camera_position(), camera.get_camera_front());
    break_block();

    render();

    if (m_is_render_cube_mesh) {
        m_cubemesh.render(camera.get_view_mat(),
            camera.get_projection_mat(),
                glm::vec3(get_voxel_world_coords(m_voxel_handler.get_detected_voxel(),
                    m_voxel_handler.get_chunk_coords_of_detected_voxel())));
    }

    m_is_break_block = false;
}

void World::break_block() {
    m_is_render_cube_mesh = false;

    if (m_voxel_handler.get_is_detected_voxel()) {
        m_is_render_cube_mesh = true;

        if (!m_is_break_block) return;

        m_is_render_cube_mesh = false;
        glm::ivec3 voxel_local_coords = m_voxel_handler.get_detected_voxel();
        glm::ivec3 chunk_coords = m_voxel_handler.get_chunk_coords_of_detected_voxel();
        unsigned voxel_idx = m_voxel_handler.get_detected_voxel_idx();

        m_chunk_map[chunk_coords]->set_voxel_id(voxel_idx, 0);
        rebuild_chunk(chunk_coords);

        if (0 == voxel_local_coords.x) {
            rebuild_chunk(glm::ivec3(chunk_coords.x-1, chunk_coords.y, chunk_coords.z));
        } else if (Constant::CHUNK_SIZE-1 == voxel_local_coords.x) {
            rebuild_chunk(glm::ivec3(chunk_coords.x+1, chunk_coords.y, chunk_coords.z));
        }

        if (0 == voxel_local_coords.y) {
            rebuild_chunk(glm::ivec3(chunk_coords.x, chunk_coords.y-1, chunk_coords.z));
        } else if (Constant::CHUNK_HEIGHT-1 == voxel_local_coords.y) {
            rebuild_chunk(glm::ivec3(chunk_coords.x, chunk_coords.y+1, chunk_coords.z));
        }

        if (0 == voxel_local_coords.z) {
            rebuild_chunk(glm::ivec3(chunk_coords.x, chunk_coords.y, chunk_coords.z-1));
        } else if (Constant::CHUNK_SIZE-1 == voxel_local_coords.z) {
            rebuild_chunk(glm::ivec3(chunk_coords.x, chunk_coords.y, chunk_coords.z+1));
        }
    }
}

void World::render() {
    m_chunk_shader.activate();
    m_chunk_shader.uniform_mat4f("view", 1, GL_FALSE, m_player.m_camera.get_view_mat());
    m_chunk_shader.uniform_mat4f("projection", 1, GL_FALSE, m_player.m_camera.get_projection_mat());

    for (auto &chunk : m_chunk_map) {
        m_chunk_shader.uniform_mat4f("model", 1, GL_FALSE, chunk.second->get_chunk_model());
        chunk.second->render();
    }
}

void World::load_chunks() {
    glm::ivec3 player_chunk_coords = m_player.get_player_chunk_coords();
    std::vector<glm::ivec3> chunks_to_load;

    for (int x = -(int)Constant::LOAD_DISTANCE; x <= (int)Constant::LOAD_DISTANCE; x++) {
        for (int z = -(int)Constant::LOAD_DISTANCE; z <= (int)Constant::LOAD_DISTANCE; z++) {
            glm::ivec3 chunk_coord(x + player_chunk_coords.x, 0, z + player_chunk_coords.z);
            if (1 == m_chunk_map.count(chunk_coord)) continue;
            chunks_to_load.push_back(chunk_coord);
            Chunk *pchunk = new Chunk(chunk_coord);
            pchunk->build_voxels();
            m_chunk_map[chunk_coord] = pchunk;
        }
    }

    for (auto const &coords : chunks_to_load) {
        build_chunk(coords);
    }
}

void World::debug_window() {
    fps_frames++;

    if (fps_lasttime < SDL_GetTicks() - FPS_INTERVAL*1000) {
        fps_lasttime = SDL_GetTicks();
        fps_current = fps_frames;
        fps_frames = 0;
    }

    Camera camera = m_player.m_camera;
    glm::vec3 camera_pos = camera.get_camera_position();
    ImGui::Begin("Debug", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetWindowPos(ImVec2(0, 0), 0);

    ImGui::Text("FPS: %u", fps_current);
    ImGui::Text("Position: %f, %f, %f", camera_pos.x, camera_pos.y, camera_pos.z);
    ImGui::Text("Cardinal Direction: %s", camera.get_cardinal_directions().c_str());
    ImGui::Text("XYZ Direction: %s", camera.get_xyz_directions().c_str());

    ImGui::NewLine();

    ImGui::Text("GPU Vendor: %s", glGetString(GL_VENDOR));
    ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
    ImGui::Text("Version: %s", glGetString(GL_VERSION));
    ImGui::Text("Shading Language Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    ImGui::End();
}

unsigned* World::get_chunk_neighbor_pvoxels(glm::ivec3 coord) {
    unsigned *neighbor_pvoxels = nullptr;

    if (1 == m_chunk_map.count(coord)) {
        neighbor_pvoxels = m_chunk_map[coord]->get_pvoxels();
    }

    return neighbor_pvoxels;
}

void World::remove_chunks() {
    glm::ivec3 player_chunk_coords = m_player.get_player_chunk_coords();
    std::vector<glm::ivec3> chunks_to_remove;

    int x_min = -(int)Constant::LOAD_DISTANCE + player_chunk_coords.x;
    int x_max = (int)Constant::LOAD_DISTANCE + player_chunk_coords.x;
    int z_min = -(int)Constant::LOAD_DISTANCE + player_chunk_coords.z;
    int z_max = (int)Constant::LOAD_DISTANCE + player_chunk_coords.z;

    for (auto const &chunk : m_chunk_map) {
        glm::ivec3 chunk_coord = chunk.first;
        if (x_min <= chunk_coord.x && chunk_coord.x <= x_max && 
                z_min <= chunk_coord.z && chunk_coord.z <= z_max) continue;

        chunks_to_remove.push_back(chunk_coord);
    }

    for (auto const &chunk_coord : chunks_to_remove) {
        delete m_chunk_map[chunk_coord];
        m_chunk_map.erase(chunk_coord);
    }
}

void World::rebuild_chunks() {
    glm::ivec3 current_player_chunk_pos = m_player.get_player_chunk_coords();
    std::unordered_set<glm::ivec3> chunks_to_rebuild;

    int old_x_min = m_prev_player_chunk_pos.x - Constant::LOAD_DISTANCE;
    int old_x_max = m_prev_player_chunk_pos.x + Constant::LOAD_DISTANCE;
    int old_z_min = m_prev_player_chunk_pos.z - Constant::LOAD_DISTANCE;
    int old_z_max = m_prev_player_chunk_pos.z + Constant::LOAD_DISTANCE;

    int new_x_min = current_player_chunk_pos.x - Constant::LOAD_DISTANCE;
    int new_x_max = current_player_chunk_pos.x + Constant::LOAD_DISTANCE;
    int new_z_min = current_player_chunk_pos.z - Constant::LOAD_DISTANCE;
    int new_z_max = current_player_chunk_pos.z + Constant::LOAD_DISTANCE;

    for (int x = new_x_min; x <= new_x_max; x++) {
        for (int z = new_z_min; z <= new_z_max; z++) {
            if (new_x_min < x && x < new_x_max && new_z_min < z && z < new_z_max) continue;
            
            if (old_x_min <= x && x <= old_x_max && old_z_min <= z && z <= old_z_max) {
                chunks_to_rebuild.insert(glm::ivec3(x, 0, z));
            }
        }
    }

    for (int x = old_x_min; x <= old_x_max; x++) {
        for (int z = old_z_min; z <= old_z_max; z++) {
            if (old_x_min < x && x < old_x_max && old_z_min < z && z < old_z_max) continue;
            
            if (new_x_min <= x && x <= new_x_max && new_z_min <= z && z <= new_z_max) {
                chunks_to_rebuild.insert(glm::ivec3(x, 0, z));
            }
        }
    }

    for (auto const &coords : chunks_to_rebuild) {
        rebuild_chunk(coords);
    }
}

void World::poll_event(const SDL_Event &event) {
    m_player.m_camera.pan(event);

    if (SDL_KEYDOWN == event.type) {
        if (SDL_SCANCODE_B == event.key.keysym.scancode) {
            m_is_break_block = true;
        }
    }

    if (SDL_MOUSEBUTTONDOWN == event.type) {
        if (SDL_BUTTON_LEFT == event.button.button) {
            m_is_break_block = true;
        }
    }
}
