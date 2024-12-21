#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <unordered_set>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <SDL2/SDL.h>
#include <imgui.h>
#include "chunk.hpp"
#include "camera.hpp"
#include "voxel_handler.hpp"
#include "player.hpp"
#include "cubemesh.hpp"

class World {
public:
    World(float getWidthHeightRatio);
    ~World();

    void update(float delta_time);
    void render();
    void buildChunk(glm::ivec3 chunk_coords);
    void rebuildChunk(glm::ivec3 chunk_coords);
    void loadChunks();
    void removeChunks();
    void rebuildChunks();
    void breakBlock();
    void placeBlock();
    void pollEvent(const SDL_Event &event, float delta_time);
    unsigned* getChunkNeighborVoxelsPointer(glm::ivec3 coord);

    Player *getPlayerPointer();
private:
    Player m_player;
    ShaderProgram m_chunk_shader;
    ShaderProgram m_cubemesh_shader;
    std::unordered_map<glm::ivec3, Chunk*> m_chunk_map;
    VoxelHandler m_voxel_handler;
    CubeMesh m_cubemesh;

    glm::ivec3 m_prev_player_chunk_pos;
    bool m_is_block_interaction_enabled = false;
    bool m_is_render_cube_mesh = false;
    int m_block_interaction_mode = 0;
};

#endif