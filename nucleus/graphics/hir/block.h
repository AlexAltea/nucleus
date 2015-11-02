/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <list>
#include <string>

namespace gfx {
namespace hir {

// Forward declarations
class Function;
class Instruction;

enum BlockFlags {
    BLOCK_IS_ENTRY = (1 << 0),  // Block is the entry point of its parent function
};

class Block {
    // Value ID used in human-readable HIR representations
    S32 id = -1;

public:
    Function* parent;

    std::list<Instruction*> instructions;

    U32 flags;

    // Constructor
    Block(Function* parent);
    ~Block();

    // Get ID of this block
    S32 getId();

    /**
     * Save a human-readable version of this HIR block
     * @return String containing the readable version of this HIR block
     */
    std::string dump();
};

}  // namespace hir
}  // namespace gfx
