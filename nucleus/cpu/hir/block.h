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
};

}  // namespace hir
}  // namespace cpu
