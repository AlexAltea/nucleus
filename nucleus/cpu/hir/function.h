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

public:
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
    const void* nativeAddress;

    // Constructor
    Function(Module* parent, TypeOut tOut, TypeIn tIn);
    ~Function();

    /**
     * Call native function
     */
    template <typename... Ts>
    U64 call(Ts... args) {
        if (flags & FUNCTION_IS_COMPILED) {
            return ((U64(*)(Ts...))nativeAddress)(args...);
        } else {
            logger.error(LOG_CPU, "Function is not ready");
            return 0;
        }
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
