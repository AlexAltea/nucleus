/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace cpu {
namespace frontend {

// Forward declarations
class Function;

class Block {
public:
    Function* parent;

    // Starting address
    U64 address = 0;

    // Number of bytes covered by this block
    U64 size = 0;

    // Branching
    U64 branch_a = 0; // Conditional-True or Unconditional branching address
    U64 branch_b = 0; // Conditional-False branching address

    // Check whether an address is inside this block
    bool contains(U64 addr) const {
        const U64 from = address;
        const U64 to = address + size;
        return from <= addr && addr < to;
    }

    // Cut this block and return the remaining part as a new object
    Block split(U64 cut) {
        // Configure new block
        Block new_block{};
        new_block.parent = parent;
        new_block.address = cut;
        new_block.size = size - (cut - address);
        new_block.branch_a = branch_a;
        new_block.branch_b = branch_b;

        // Update this block
        size = cut - address;
        branch_a = cut;
        branch_b = 0;

        return new_block;
    }
};

}  // namespace frontend
}  // namespace cpu
