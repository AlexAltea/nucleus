/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "register_allocation_pass.h"

namespace cpu {
namespace hir {
namespace passes {

RegisterAllocationPass::RegisterAllocationPass(backend::TargetInfo* targetInfo) {
}

bool RegisterAllocationPass::run(Function* function) {
    // Check function flags
    if (!function || !(function->flags & FUNCTION_IS_DEFINED)) {
        return false;
    }

    function->flags |= FUNCTION_IS_CALLABLE;
    return true;
}

}  // namespace passes
}  // namespace hir
}  // namespace cpu
