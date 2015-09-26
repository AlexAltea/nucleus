/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/format.h"
#include "nucleus/cpu/hir/module.h"
#include "nucleus/cpu/hir/type.h"
#include "nucleus/cpu/hir/value.h"
#include "nucleus/cpu/frontend/frontend_block.h"
#include "nucleus/cpu/frontend/frontend_function.h"
#include "nucleus/cpu/frontend/frontend_module.h"
#include "nucleus/cpu/frontend/ppu/analyzer/ppu_analyzer.h"

#include <map>
#include <string>
#include <vector>

namespace cpu {
namespace frontend {
namespace ppu {

// Class declarations
class Block;
class Function;
class Module;

// Function type
enum FunctionTypeIn {
    FUNCTION_IN_UNKNOWN = 0,
    FUNCTION_IN_INTEGER,      // The U64 argument is passed on r3 to r10
    FUNCTION_IN_FLOAT,        // The F64 argument is passed on f1 to f13
    FUNCTION_IN_VECTOR,       // The U128 arguement is passed on v2 to v13
};

enum FunctionTypeOut {
    FUNCTION_OUT_UNKNOWN = 0,
    FUNCTION_OUT_INTEGER,     // The U64 argument is returned on r3
    FUNCTION_OUT_FLOAT,       // The F64 argument is returned on f1
    FUNCTION_OUT_FLOAT_X2,    // The F64 argument is returned on f1:f2
    FUNCTION_OUT_FLOAT_X3,    // The F64 argument is returned on f1:f3
    FUNCTION_OUT_FLOAT_X4,    // The F64 argument is returned on f1:f4
    FUNCTION_OUT_VECTOR,      // The U128 arguement is returned on v2
    FUNCTION_OUT_VOID,        // Nothing is returned
};

class Block : public frontend::Block<U32> {
public:
    bool initial;                   // Is this a function entry block?
    bool jump_destination = false;  // Is this a target of a bx/bcx instruction?
    bool call_destination = false;  // Is this a target of a bl instruction

    // Constructors
    Block() {}
    Block(frontend::Block<U32>& block) : frontend::Block<U32>(block) {}

    // Determines whether an extra branch is required to connect this with the immediate block after
    bool is_split() const;
};

class Function : public frontend::Function<U32> {
    // Analyzer auxiliary method: Determine register read/writes
    void do_register_analysis(Analyzer* status);

public:
    // Return/Arguments type
    FunctionTypeOut type_out;
    std::vector<FunctionTypeIn> type_in;

    Function(Module* seg) {
        parent = reinterpret_cast<frontend::Module<U32>*>(seg);
    }

    // Analysis
    bool analyze_cfg();  // Generate CFG (and return if branching addresses stay inside the parent segment)
    void analyze_type(); // Determine function arguments/return types

    // Declare function inside the parent segment
    void declare();

    // Recompile function
    void recompile();
};

class Module : public frontend::Module<U32> {
public:
    Function* getFunction(U32 addr);

    // Generate a list of functions and analyze them
    void analyze();

    // Recompile each of the functions
    void recompile();
};

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
