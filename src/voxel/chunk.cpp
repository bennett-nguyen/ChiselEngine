#include "chunk.hpp"

Chunk::Chunk() {
    this->pBlocks = new Block **[Chunk::CHUNK_SIZE];

    for (int i = 0; i < Chunk::CHUNK_SIZE; i++) {
        this->pBlocks[i] = new Block *[Chunk::CHUNK_SIZE];

        for (int j = 0; j < Chunk::CHUNK_SIZE; j++) {
            this->pBlocks[i][j] = new Block[Chunk::CHUNK_SIZE];
        }
    }
}

Chunk::~Chunk() {
    for (int i = 0; i < Chunk::CHUNK_SIZE; i++) {
        for (int j = 0; j < Chunk::CHUNK_SIZE; j++) {
            delete this->pBlocks[i][j];
        }

        delete this->pBlocks[i];
    }

    delete this->pBlocks;
}