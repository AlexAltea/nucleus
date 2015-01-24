/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <string>

namespace cpu {
namespace ppu {

struct Block {
    // Starting address of this block in the EA space
    u32 address = 0;

    // Number of bytes covered by this block
    u32 size = 0;

    // Entry block with no predecessors
    bool initial = true;

    // Branching
    Block* branch_a = nullptr; // Conditional-True or Unconditional branching address
    Block* branch_b = nullptr; // Conditional-False branching address
};

struct Function {
    // Initial block that starts the Control Flow Graph
    Block block;

    // Name extracted from the DWARF symbols if available
    std::string name;
};

void analyze_segment(u32 address, u32 size);

}  // namespace ppu
}  // namespace cpu
