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
    // Starting address of the function in the EA space
    u32 address;

    // Number of bytes covered by this block
    u32 size;
};

struct Function {
    // Initial block that starts the Control Flow Graph
    Block* block;

    // Name extracted from the DWARF symbols if available
    std::string name;
};

void analyze_segment(u32 address, u32 size);

}  // namespace ppu
}  // namespace cpu
