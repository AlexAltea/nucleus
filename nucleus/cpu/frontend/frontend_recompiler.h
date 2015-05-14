/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/block.h"

#include <map>

namespace cpu {
namespace frontend {

template <typename TAddr>
class IRecompiler
{
protected:
    // Function to be generated and its parent module
    hir::Function hir_function;
    hir::Module hir_module; 

public:
    // HIR builder
    hir::Builder builder;

    // Control Flow Graph
    hir::Block prolog;
    hir::Block epilog;
    std::map<TAddr, hir::Block> blocks;

    // Constructor
    IRecompiler(hir::Function f) : function(f) {
        prolog = hir::Block::Create("prolog", function);
        epilog = hir::Block::Create("epilog", function);
        module = function.getParent();
    }
};

}  // namespace frontend
}  // namespace cpu
