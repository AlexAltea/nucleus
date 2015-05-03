/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include "llvm/IR/BasicBlock.h"

namespace cpu {
namespace hir {

class Block
{
public:
    llvm::BasicBlock* bb = nullptr;

    llvm::BasicBlock::iterator begin() {
        bb->begin();
    }
    llvm::BasicBlock::iterator end() {
        bb->end();
    }
};

}  // namespace hir
}  // namespace cpu
