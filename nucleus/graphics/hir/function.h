/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/hir/type.h"
#include "nucleus/graphics/hir/value.h"

#include <vector>

namespace gfx {
namespace hir {

// Forward declarations
class Block;
class Module;

enum FunctionFlags {
    FUNCTION_IS_DECLARED    = (1 << 0),  // Function is referenced in its parent module
    FUNCTION_IS_DEFINING    = (1 << 1),  // Function is being analyzed and HIR emitted
    FUNCTION_IS_DEFINED     = (1 << 2),  // Function has been analyzed and HIR emitted
    FUNCTION_IS_CALLABLE    = (1 << 3),  // Function can be called
    FUNCTION_IS_ENTRY       = (1 << 4),  // Function is the module entry point
};

class Function {
    using TypeOut = Type;
    using TypeIn = std::vector<Type>;

    // Value ID used in human-readable HIR representations
    S32 id = -1;

public:
    std::vector<Block*> blocks;
    std::vector<Value*> args;

    Module* parent;

    // Function flags
    U32 flags;

    // Return and argument types
    TypeOut typeOut;
    TypeIn typeIn;

    // Constructor
    Function(Module* parent, TypeOut tOut, TypeIn tIn = {});
    ~Function();

    // Get ID of this function (dumping related)
    S32 getId();

    // Generate IDs for child blocks and values
    S32 blockIdCounter = 0;
    S32 valueIdCounter = 0;

    /**
     * Save a human-readable version of this HIR function
     * @return           String containing the readable version of this HIR function
     */
    std::string dump();
};

}  // namespace hir
}  // namespace gfx
