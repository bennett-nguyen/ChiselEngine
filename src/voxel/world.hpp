#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include "chunk.hpp"
#include "camera.hpp"
#include "glm/gtx/hash.hpp"

class World {
public:
    World(Camera *p_camera, ShaderProgram *p_chunk_shader);
    ~World();

    void render();
    void load_chunks();
    void remove_chunks();
    glm::ivec3 get_chunk_coords_from_camera();
private:
    Camera *m_pcamera;
    ShaderProgram *m_pchunk_shader;
    std::unordered_map<glm::ivec3, Chunk*> m_chunk_map;
};

#endif