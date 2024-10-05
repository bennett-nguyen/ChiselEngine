#include "block.hpp"

const float Block::BLOCK_RENDER_SIZE = 1.0f;

Block::Block() {
    this->set_active(false);
}

Block::~Block() {}

bool Block::is_active() {
    return this->active;
}

void Block::set_active(bool active) {
    this->active = active;
}