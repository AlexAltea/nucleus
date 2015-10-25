/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
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
template <typename TAddr>
class Module;

template <typename TAddr>
class Function {
public:
    // Parent segment
    Module<TAddr>* parent;

    // HIR Function
    hir::Function* hirFunction;

    // Starting address of the entry block
    TAddr address = 0;

    // Number of bytes covered by all CFG blocks
    TAddr size = 0;

    // Name of this function
    std::string name;

    // Control Flow Graph
    std::map<TAddr, Block<TAddr>*> blocks;

    // Check whether an address is inside any CFG block
    bool contains(TAddr addr) const {
        for (const auto& item : blocks) {
            const auto& block = block.second;
            if (block.contains(addr)) {
                return true;
            }
        }
        return false;
    }
};

}  // namespace frontend
}  // namespace cpu
