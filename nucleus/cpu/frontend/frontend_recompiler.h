/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/block.h"
#include "nucleus/cpu/frontend/frontend_function.h"

#include <map>

namespace cpu {
namespace frontend {

class IRecompiler {
protected:
    // Function to be generated
    Function* function;

public:
    // HIR builder
    hir::Builder builder;

    // Control Flow Graph
    hir::Block* prolog = nullptr;
    hir::Block* epilog = nullptr;
    std::map<U64, hir::Block*> blocks;

    // Constructor
    IRecompiler(Function* f) : function(f) {
        //prolog = hir::Block::Create("prolog", function->function);
        //epilog = hir::Block::Create("epilog", function->function);
    }
};

}  // namespace frontend
}  // namespace cpu
