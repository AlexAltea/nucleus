/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/format.h"

#include "llvm/IR/Module.h"
#include "llvm/PassManager.h"

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
    FUNCTION_IN_VECTOR,       // The u128 arguement is passed on v2 to v13
};

enum FunctionTypeOut {
    FUNCTION_OUT_UNKNOWN = 0,
    FUNCTION_OUT_INTEGER,     // The u64 argument is returned on r3
    FUNCTION_OUT_FLOAT,       // The f64 argument is returned on f1
    FUNCTION_OUT_FLOAT_X2,    // The f64 argument is returned on f1:f2
    FUNCTION_OUT_FLOAT_X3,    // The f64 argument is returned on f1:f3
    FUNCTION_OUT_FLOAT_X4,    // The f64 argument is returned on f1:f4
    FUNCTION_OUT_VECTOR,      // The u128 arguement is returned on v2
    FUNCTION_OUT_VOID,        // Nothing is returned
};

class Block
{
public:
    u32 address = 0; // Starting address in the EA space
    u32 size = 0;    // Number of bytes covered

    // Entry block with no predecessors
    bool initial = true;

    // Branching
    u32 branch_a = 0; // Conditional-True or Unconditional branching address
    u32 branch_b = 0; // Conditional-False branching address

    // Determines whether the specified address is part of this block
    bool contains(u32 addr) const;
};

class Function
{
    // Analyzer auxiliary method: Determine function arguments/return types
    void get_type();

public:
    u32 address = 0; // Starting address in the EA space
    u32 size = 0;    // Number of bytes covered (sum of basic block sizes)

    // Control Flow Graph
    std::map<u32, Block> blocks;

    // Return/Arguments type
    FunctionTypeOut type_out;
    std::vector<FunctionTypeIn> type_in;

    // Name extracted from the DWARF symbols if available
    std::string name;

    Function(u32 address) : address(address) {
        name = format("func_%X", address);
    }

    // Analyze function relative to a specific segment:
    // Generate CFG and return if analysis succeeded (branching addresses stay inside the segment)
    bool analyze(u32 segAddress, u32 segSize);

    // Recompile function inside a LLVM module
    llvm::Function* recompile(llvm::Module* module);
};

class Segment
{
    llvm::Module* module = nullptr;
    llvm::FunctionPassManager* fpm = nullptr;

public:
    u32 address = 0; // Starting address in the EA space
    u32 size = 0;    // Number of bytes covered

    // Functions contained
    std::vector<Function> functions;

    std::string name;

    Segment(u32 address, u32 size) : address(address), size(size) {
        name = format("seg_%X", address);
    }

    ~Segment() {
        delete module;
    }

    // Generate a list of functions and analyze them
    void analyze();

    // Recompile each of the functions
    void recompile();

    // Determines whether the specified address is part of this segment
    bool contains(u32 addr) const;
};

}  // namespace ppu
}  // namespace cpu
