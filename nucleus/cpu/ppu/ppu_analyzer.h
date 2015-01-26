/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

//#include "llvm/IR/BasicBlock.h"
//#include "llvm/IR/Function.h"

#include <string>
#include <vector>

namespace cpu {
namespace ppu {

class Block
{
    //llvm::BasicBlock* block;

public:
    u32 address = 0; // Starting address of this block in the EA space
    u32 size = 0;    // Number of bytes covered by this block

    // Entry block with no predecessors
    bool initial = true;

    // Branching
    Block* branch_a = nullptr; // Conditional-True or Unconditional branching address
    Block* branch_b = nullptr; // Conditional-False branching address
};

class Function
{
    //llvm::Function* function;

public:
    u32 address = 0; // Starting address of this function in the EA space
    u32 size = 0;    // Number of bytes covered by this function (sum of basic block sizes)

    // Initial block that starts the Control Flow Graph
    Block block;

    // Name extracted from the DWARF symbols if available
    std::string name;

    Function(u32 address) : address(address) {}

    // Analyze function: Generate graph of basic blocks
    void analyze();
};

class Segment
{
public:
    u32 address = 0; // Starting address of this segment in the EA space
    u32 size = 0;    // Number of bytes covered by this segment

    // Functions contained in this segment
    std::vector<Function> functions;

    Segment(u32 address, u32 size) : address(address), size(size) {}

    // Analyze segment: Get list of functions and analyze them
    void analyze();
};

}  // namespace ppu
}  // namespace cpu
