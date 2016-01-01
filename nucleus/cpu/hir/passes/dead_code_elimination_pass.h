/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/pass.h"

namespace cpu {
namespace hir {
namespace passes {

class DeadCodeEliminationPass : public Pass {
public:
    // Constructor
    DeadCodeEliminationPass();

    // Get the name of this pass
    const char* name() override {
        return "Dead Code Elimination";
    }

    // Apply this pass on a function
    bool run(Function* function) override;
};

}  // namespace passes
}  // namespace hir
}  // namespace cpu
