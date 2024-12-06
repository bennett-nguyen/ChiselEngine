#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <unordered_set>
#include "chunk.hpp"
#include "camera.hpp"
#include <glm/gtx/hash.hpp>
#include "voxel_handler.hpp"
#include <SDL2/SDL.h>
#include "player.hpp"
#include "imgui.h"
#include "cubemesh.hpp"

class World {
public:
    World(float getWidthHeightRatio);
    ~World();

    void update();
    void render();
    void buildChunk(glm::ivec3 chunk_coords);
    void rebuildChunk(glm::ivec3 chunk_coords);
    void loadChunks();
    void removeChunks();
    void rebuildChunks();
    void breakBlock();
    void pollEvent(const SDL_Event &event);
    unsigned* getChunkNeighborPVoxels(glm::ivec3 coord);

    Player *getPlayerPointer();
private:
    Player m_player;
    ShaderProgram m_chunk_shader;
    std::unordered_map<glm::ivec3, Chunk*> m_chunk_map;
    VoxelHandler m_voxel_handler;
    CubeMesh m_cubemesh;

    glm::ivec3 m_prev_player_chunk_pos;
    bool m_is_break_block = false;
    bool m_is_render_cube_mesh = false;
};

#endif