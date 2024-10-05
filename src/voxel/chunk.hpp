#ifndef CHUNK_HPP
#define CHUNK_CPP

#include "mesh.hpp"
#include "block.hpp"
#include "vertex.hpp"
#include <array>
#include <algorithm>
#include <glm/gtc/noise.hpp>

class Chunk {
public:
    Chunk(glm::vec3 position);
    ~Chunk();

    void update(float dt);
    void render(ShaderProgram &shader_program, glm::vec4 voxel_fill, glm::vec4 voxel_outline);

    void create_mesh();
    void rebuild_mesh();
    void create_cube(unsigned x, unsigned y, unsigned z, std::array<bool, 6> faces);

    void load_chunk();
    void setup_chunk();
    void unload_chunk();

    bool is_setup();
    bool is_loaded();
    Block* get_pblock(size_t idx);
    Block* get_pblock(unsigned x, unsigned y, unsigned z);
    size_t get_block_idx(unsigned x, unsigned y, unsigned z);
    glm::vec3 get_chunk_pos();

    static const unsigned CHUNK_SIZE;
    static const unsigned CHUNK_AREA;
    static const unsigned CHUNK_HEIGHT;

private:
    Mesh chunk_mesh;
    Block *pBlocks;
    bool _is_setup = false;
    bool _is_loaded = false;
    glm::vec3 chunk_position;
};

#endif