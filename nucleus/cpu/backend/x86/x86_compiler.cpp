/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "x86_compiler.h"
#include "nucleus/logger/logger.h"
#include "nucleus/cpu/backend/x86/x86_sequences.h"

#include <queue>

namespace cpu {
namespace backend {
namespace x86 {

using namespace cpu::hir;

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

bool X86Compiler::compile(Block* block) {
    auto& instructions = block->instructions;
    for (auto it = instructions.begin(); it != instructions.end(); it++) {
        X86Sequences::select(emitter.get(), *it);
    }

    return true;
}

bool X86Compiler::compile(Function* function) {
    // Function blocks to compile
    std::queue<Block*> blocks({ function->entry });
    while (!blocks.empty()) {
        auto block = blocks.front();
        if (!compile(block)) {
            logger.error(LOG_CPU, "Cannot compile block");
            return false;
        }
        blocks.pop();
    }

    return true;
}

bool X86Compiler::compile(Module* module) {
    for (auto function : module->functions) {
        compile(function);
    }
    return true;
}

}  // namespace x86
}  // namespace backend
}  // namespace cpu
