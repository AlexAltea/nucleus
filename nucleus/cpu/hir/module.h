/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include "llvm/IR/Module.h"

namespace cpu {

class Module
{
public:
    llvm::Module* value = nullptr;
};

}  // namespace cpu
