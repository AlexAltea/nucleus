/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include "llvm/IR/Function.h"

namespace cpu {

template<typename TR, typename... TArgs>
class Function
{
public:
    llvm::Function* value = nullptr;
};

}  // namespace cpu
