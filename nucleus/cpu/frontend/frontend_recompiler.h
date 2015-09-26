/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/block.h"
#include "nucleus/cpu/frontend/frontend_function.h"

#include <map>

namespace cpu {
namespace frontend {

template <typename TAddr>
class IRecompiler {
protected:
    // Function to be generated
    Function<TAddr>* function;

public:
    // HIR builder
    hir::Builder builder;

    // Control Flow Graph
    hir::Block* prolog;
    hir::Block* epilog;
    std::map<TAddr, hir::Block*> blocks;

    // Constructor
    IRecompiler(Function<TAddr>* f) : function(f) {
        /*prolog = hir::Block::Create("prolog", function->function);
        epilog = hir::Block::Create("epilog", function->function);*/
    }
};

}  // namespace frontend
}  // namespace cpu
