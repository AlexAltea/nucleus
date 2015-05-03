/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/block.h"
#include "nucleus/cpu/hir/function.h"
#include "nucleus/cpu/frontend/frontend_block.h"

#include <unordered_map>

namespace cpu {
namespace frontend {

template<typename TAddr>
class Function : public hir::Function
{
public:
    // Starting address of the entry block
    TAddr address = 0;

    // Number of bytes covered by all CFG blocks
    TAddr size = 0;

    // Control Flow Graph
    std::unordered_map<TAddr, Block<TAddr>> blocks;
    hir::Block<TAddr> prolog;
    hir::Block<TAddr> epilog;

    // Check whether an address is inside any CFG block
    bool contains(TAddr addr) {
        for (auto& item : blocks) {
            auto& block = block.second;
            if (block.contains(addr)) {
                return true;
            }
        }
        return false;
    }
};

}  // namespace frontend
}  // namespace cpu
