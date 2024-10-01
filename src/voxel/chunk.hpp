#ifndef CHUNK_HPP
#define CHUNK_CPP

#include "block.hpp"

class Chunk {
public:
    Chunk();
    ~Chunk();

    void update(float dt);
    void render();

    static const int CHUNK_SIZE = 16;

private:
    Block ***pBlocks;
};

#endif