#include "block.hpp"

Block::Block() {}
Block::~Block() {}

bool Block::is_active() {
    return this->active;
}

void Block::set_active(bool active) {
    this->active = active;
}