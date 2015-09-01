/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/type.h"
#include "nucleus/cpu/hir/value.h"

#include <vector>

namespace cpu {
namespace hir {

class Block;
class Module;

enum FunctionFlags {
    FUNCTION_IS_DECLARED   = (1 << 0),  // Function is referenced in its parent module
    FUNCTION_IS_DEFINING   = (1 << 1),  // Function is being analyzed and IR emitted
    FUNCTION_IS_DEFINED    = (1 << 1),  // Function has been analyzed and IR emitted
    FUNCTION_IS_COMPILING  = (1 << 2),  // Function is being compiled
    FUNCTION_IS_COMPILED   = (1 << 2),  // Function has been compiled
};

class Function {
    using TypeOut = Type;
    using TypeIn = std::vector<Type>;

    Module* parent;

    // Pointer to the compiled function
    void* emittedFunction;

public:
    // Function flags
    U32 flags;

    // Return and argument types
    TypeOut typeOut;
    TypeIn typeIn;

    // Entry point
    Block* entry;
    std::vector<Block*> blocks;

    std::vector<Value*> args;

    // Constructor
    Function(Module* parent, TypeOut tOut, TypeIn tIn);
    ~Function();

    template <typename... Ts>
    U64 call(Ts... args) {
        return ((U64(*)(Ts...))emittedFunction)(args...);
    }
    
    // Frontend methods

    /**
     * Analyze this function emitting the IR
     * @return           True on success
     */
    bool analyze();


    // Backend methods

    /**
     * Compile this function
     * @return           True on success
     */
    bool compile();
};

}  // namespace hir
}  // namespace cpu
