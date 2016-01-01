/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/type.h"
#include "nucleus/cpu/hir/value.h"

#include <vector>

namespace cpu {
namespace hir {

// Forward declarations
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

    // Value ID used in human-readable HIR representations
    S32 id = -1;

public:
    Module* parent;

    // Function flags
    U32 flags;

    // Return and argument types
    TypeOut typeOut;
    TypeIn typeIn;

    // Blocks
    std::vector<Block*> blocks;

    // Arguments
    std::vector<Value*> args;

    // Pointer to the compiled function
    void* nativeAddress;
    U64 nativeSize;

    // Constructor
    Function(Module* parent, TypeOut tOut, TypeIn tIn = {});
    ~Function();

    // Get ID of this function (dumping related)
    S32 getId();

    // Generate IDs for child blocks and values
    S32 blockIdCounter = 0;
    S32 valueIdCounter = 0;

    /**
     * Reset the function to its original declared state, removing its definition and compiled result
     */
    void reset();

    /**
     * Save a human-readable version of this HIR function
     * @return           String containing the readable version of this HIR function
     */
    std::string dump();
};

}  // namespace hir
}  // namespace cpu
