/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/block.h"

namespace cpu {
namespace frontend {

template<typename TAddr>
class Block : public hir::Block
{
public:
    // Starting address
    TAddr address = 0;

    // Number of bytes covered by this block
    TAddr size = 0;

    // Check whether an address is inside this block
    bool contains(TAddr addr) {
        const TAddr from = address;
        const TAddr to = address + size;
        return from <= addr && addr < to;
    }
};

}  // namespace frontend
}  // namespace cpu
