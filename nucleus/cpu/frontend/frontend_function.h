/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/frontend/frontend_block.h"
#include "nucleus/cpu/frontend/frontend_segment.h"

#include <string>
#include <unordered_map>

namespace cpu {
namespace frontend {

// Class declarations
template <typename TAddr>
class ISegment;

template <typename TAddr>
class IFunction
{
public:
    hir::Function function;

    // Starting address of the entry block
    TAddr address = 0;

    // Number of bytes covered by all CFG blocks
    TAddr size = 0;

    // Name of this function
    std::string name;

    // Parent segment
    ISegment<TAddr>* parent;

    // Control Flow Graph
    std::unordered_map<TAddr, IBlock<TAddr>*> blocks;

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
