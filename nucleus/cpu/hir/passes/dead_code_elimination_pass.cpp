/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "dead_code_elimination_pass.h"

namespace cpu {
namespace hir {
namespace passes {

bool DeadCodeEliminationPass::run(Function* function) {
    // Check function flags
    if (!function || !(function->flags & FUNCTION_IS_DEFINED)) {
        return false;
    }

    return true;
}

}  // namespace passes
}  // namespace hir
}  // namespace cpu
