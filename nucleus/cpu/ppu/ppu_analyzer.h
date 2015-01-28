/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

//#include "llvm/IR/BasicBlock.h"
//#include "llvm/IR/Function.h"

#include <map>
#include <string>
#include <vector>

namespace cpu {
namespace ppu {

// Function type
enum FunctionTypeIn {
    FUNCTION_IN_UNKNOWN = 0,
    FUNCTION_IN_INTEGER,      // The u64 argument is passed on r3 to r10
    FUNCTION_IN_FLOAT,        // The f64 argument is passed on f1 to f13
};

enum FunctionTypeOut {
    FUNCTION_OUT_UNKNOWN = 0,
    FUNCTION_OUT_INTEGER,     // The u64 argument is returned on r3 (to r6?)
    FUNCTION_OUT_FLOAT,       // The f64 argument is returned on f1 to f4
};

class Block
{
    //llvm::BasicBlock* block;

public:
    u32 address = 0; // Starting address in the EA space
    u32 size = 0;    // Number of bytes covered

    // Entry block with no predecessors
    bool initial = true;

    // Branching
    u32 branch_a = 0; // Conditional-True or Unconditional branching address
    u32 branch_b = 0; // Conditional-False branching address
};

class Function
{
    //llvm::Function* function;

public:
    u32 address = 0; // Starting address in the EA space
    u32 size = 0;    // Number of bytes covered (sum of basic block sizes)

    // Control Flow Graph
    std::map<u32, Block> blocks;

    // Arguments/return type
    std::vector<FunctionTypeIn> type_in;
    std::vector<FunctionTypeOut> type_out;

    // Name extracted from the DWARF symbols if available
    std::string name;

    Function(u32 address) : address(address) {}

    // Analyze function relative to a specific segment:
    // Determine CFG and arguments/return types and return if analysis succeeded.
    // Unsuccessful analysis are caused by branches outside the segment.
    bool analyze(u32 segAddress, u32 segSize);

    // Recompile function
    void recompile();
};

class Segment
{
public:
    u32 address = 0; // Starting address in the EA space
    u32 size = 0;    // Number of bytes covered

    // Functions contained
    std::vector<Function> functions;

    Segment(u32 address, u32 size) : address(address), size(size) {}

    // Generate a list of functions and analyze them
    void analyze();

    // Recompile each of the functions
    void recompile();
};

}  // namespace ppu
}  // namespace cpu
