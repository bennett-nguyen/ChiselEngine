#ifndef WORLD_HPP
#define WORLD_HPP

#include <unordered_map>
#include <unordered_set>
#include <glm/gtx/hash.hpp>

#include "chunk.hpp"
#include "camera.hpp"
#include "shader.hpp"

struct World {
    Camera camera;
    ShaderProgramID chunk_shader_program;
    std::unordered_map<glm::ivec3, Chunk*> chunk_map;

    glm::ivec3 prev_player_chunk_pos;
};

Chunk* getChunkPointer(World &world, glm::ivec3 chunk_position);
// void initWorld(World &world);
void initChunkShader(World &world, const std::string &vshader_path, const std::string &fshader_path);
void buildChunk(World &world, glm::ivec3 chunk_position);
void loadChunks(World &world);
void removeChunks(World &world);
void rebuildChunks(World &world);
void renderWorld(const World &world);
bool isChunkExist(const World &world, glm::ivec3 chunk_position);
void breakBlock(World &world, glm::ivec3 voxel_position);
void placeBlock(World &world, glm::ivec3 adjacent_voxel_position);

#endif