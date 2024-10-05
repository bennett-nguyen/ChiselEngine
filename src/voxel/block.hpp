#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "blocktype_enum.hpp"
#include <stdexcept>

enum Faces {
    _BEGIN = 0,
    X_NEG = _BEGIN, X_POS,
    Y_NEG, Y_POS,
    Z_NEG, Z_POS,
    _END
};

class Block {
public:
    static const float BLOCK_RENDER_SIZE;

    Block();
    ~Block();

    bool is_active();
    void set_active(bool active);
private:
    bool active = false;
    BlockType block_type;
};

#endif