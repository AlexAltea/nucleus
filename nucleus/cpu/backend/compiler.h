/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/block.h"
#include "nucleus/cpu/hir/function.h"
#include "nucleus/cpu/hir/module.h"

namespace cpu {
namespace backend {

class Compiler {
public:
    virtual bool compile(hir::Block* block) = 0;
    virtual bool compile(hir::Function* function) = 0;
    virtual bool compile(hir::Module* module) = 0;
};

}  // namespace backend
}  // namespace cpu
