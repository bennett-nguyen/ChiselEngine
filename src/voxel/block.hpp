#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "blocktype_enum.hpp"

class Block {
public:
    Block();
    ~Block();

    bool is_active();
    void set_active(bool active);
private:
    bool active;
    BlockType block_type;
};

#endif