/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/function.h"

namespace cpu {
namespace backend {

class Compiler {
public:
    virtual bool compile(hir::Block* function) = 0;
    virtual bool compile(hir::Function* function) = 0;
    virtual bool compile(hir::Module* function) = 0;
};

}  // namespace backend
}  // namespace cpu
