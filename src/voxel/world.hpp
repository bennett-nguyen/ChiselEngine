#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <unordered_set>
#include "chunk.hpp"
#include "camera.hpp"
#include "glm/gtx/hash.hpp"

class World {
public:
    World(Camera *p_camera, ShaderProgram *p_chunk_shader);
    ~World();

    void update();
    void render();
    void load_chunks();
    void remove_chunks();
    void rebuild_chunks();
    glm::ivec3 get_chunk_coords_from_camera();
    unsigned* get_chunk_neighbor_pvoxels(glm::ivec3 coord);

private:
    Camera *m_pcamera;
    ShaderProgram *m_pchunk_shader;
    std::unordered_map<glm::ivec3, Chunk*> m_chunk_map;

    glm::ivec3 m_prev_player_chunk_pos;
};

#endif