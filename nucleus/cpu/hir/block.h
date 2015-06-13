/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/function.h"

#include "llvm/IR/BasicBlock.h"

namespace cpu {
namespace hir {

class Function;

class Block
{
public:
    llvm::BasicBlock* bb;

    // Constructor
    Block(llvm::BasicBlock* block = nullptr) : bb(block) {}

    llvm::BasicBlock::iterator begin() {
        return bb->begin();
    }
    llvm::BasicBlock::iterator end() {
        return bb->end();
    }

    static Block Create(const std::string& name, Function function);
};

}  // namespace hir
}  // namespace cpu
