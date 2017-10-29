/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/frontend/frontend_block.h"
#include "nucleus/cpu/frontend/frontend_module.h"

#include <map>
#include <string>

namespace cpu {
namespace frontend {

// Forward declarations
class Module;

class Function {
public:
    // Parent segment
    Module* parent;

    // HIR Function
    hir::Function* hirFunction;

    // Starting address of the entry block
    U64 address = 0;

    // Number of bytes covered by all CFG blocks
    U64 size = 0;

    // Name of this function
    std::string name;

    // Control Flow Graph
    std::map<U64, Block*> blocks;

    // Check whether an address is inside any CFG block
    bool contains(U64 addr) const {
        for (const auto& item : blocks) {
            const auto& block = item.second;
            if (block->contains(addr)) {
                return true;
            }
        }
        return false;
    }
};

}  // namespace frontend
}  // namespace cpu
