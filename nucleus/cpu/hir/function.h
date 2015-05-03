/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/block.h"

#include "llvm/IR/Function.h"

namespace cpu {
namespace hir {

class Function
{
public:
    llvm::Function* function = nullptr;

    Block getEntryBlock();
};

}  // namespace hir
}  // namespace cpu
