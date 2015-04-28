/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include "llvm/IR/Value.h"

namespace cpu {

template<typename T>
class Value
{
public:
    llvm::Value* value = nullptr;
};

}  // namespace cpu
