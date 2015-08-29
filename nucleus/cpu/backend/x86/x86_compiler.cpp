/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "x86_compiler.h"
#include "nucleus/logger/logger.h"

#include <queue>

namespace cpu {
namespace backend {
namespace x86 {

X86Compiler::X86Compiler() {
    emitter = std::make_unique<X86Emitter>();
#if defined(NUCLEUS_ARCH_X86_32BITS)
    emitter->mode = X86_MODE_32BITS;
#elif defined(NUCLEUS_ARCH_X86_64BITS)
    emitter->mode = X86_MODE_64BITS;
#else
    logger.error(LOG_CPU, "Unsupported variant of the x86 architecture");
#endif
}

bool X86Compiler::compile(hir::Function* function) {
    // Function blocks to compile
    std::queue<hir::Block*> blocks({ function->entry });
    while (!blocks.empty()) {
        auto block = blocks.front();
        blocks.pop();
    }

    return true;
}

}  // namespace x86
}  // namespace backend
}  // namespace cpu
