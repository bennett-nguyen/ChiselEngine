#ifndef CHUNK_MANAGER_HPP
#define CHUNK_MANAGER_HPP

#include <vector>
#include <chunk.hpp>
#include <glm/vec3.hpp>

class ChunkManager {
public:
    ChunkManager();
    void update(float dt, glm::vec3 camera_pos, glm::vec3 camera_view);

    static const unsigned ASYNC_NUM_CHUNKS_PER_FRAME = 4;

private:
    glm::vec3 camera_pos;
    glm::vec3 camera_view;

    std::vector<Chunk*> chunks;
};

#endif