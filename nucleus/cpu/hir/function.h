/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/logger/logger.h"
#include "nucleus/cpu/hir/type.h"
#include "nucleus/cpu/hir/value.h"

#include <vector>

namespace cpu {
namespace hir {

class Block;
class Module;

enum FunctionFlags {
    FUNCTION_IS_EXTERN      = (1 << 0),  // Function is defined outside its parent module
    FUNCTION_IS_DECLARED    = (1 << 1),  // Function is referenced in its parent module
    FUNCTION_IS_DEFINING    = (1 << 2),  // Function is being analyzed and HIR emitted
    FUNCTION_IS_DEFINED     = (1 << 3),  // Function has been analyzed and HIR emitted
    FUNCTION_IS_COMPILABLE  = (1 << 4),  // Function can be compiled
    FUNCTION_IS_COMPILING   = (1 << 5),  // Function is being compiled
    FUNCTION_IS_COMPILED    = (1 << 6),  // Function has been compiled
    FUNCTION_IS_CALLABLE    = (1 << 7),  // Function can be called
};

class Function {
    using TypeOut = Type;
    using TypeIn = std::vector<Type>;

public:
    Module* parent;

    // Function flags
    U32 flags;

    // Return and argument types
    TypeOut typeOut;
    TypeIn typeIn;

    // Entry point
    Block* entry;
    std::vector<Block*> blocks;

    // Arguments
    std::vector<Value*> args;

    // Pointer to the compiled function
    void* nativeAddress;
    U64 nativeSize;

    // Constructor
    Function(Module* parent, TypeOut tOut, TypeIn tIn);
    ~Function();

    /**
     * Call native function
     * @param[in]  args  Arguments as an array of constant values
     * @return           Return as a constant value
     */
    Value* call(const std::vector<Value*>& args = {});
};

}  // namespace hir
}  // namespace cpu
