/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <list>

namespace cpu {
namespace hir {

class Function;
class Instruction;

class Block {
    // Value ID used in human-readable HIR representations
    S32 id = -1;

public:
    Function* parent;

    std::list<Instruction*> instructions;

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
}  // namespace cpu
