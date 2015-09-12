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

X86Compiler::X86Compiler() : Compiler() {
    // Initialize sequences
    X86Sequences::init();

    // Initialize emitter
    emitter = std::make_unique<X86Emitter>();
#if defined(NUCLEUS_ARCH_X86_32BITS)
    emitter->mode = X86_MODE_32BITS;
#elif defined(NUCLEUS_ARCH_X86_64BITS)
    emitter->mode = X86_MODE_64BITS;
#else
    logger.error(LOG_CPU, "Unsupported variant of the x86 architecture");
#endif

    // Set target information
#if defined(NUCLEUS_PLATFORM_WINDOWS)
    targetInfo.regSet[0].count = 4;
    targetInfo.regSet[0].types = RegisterSet::TYPE_INT;
    targetInfo.regSet[0].retIndex = emitter->rax.getIdx();
    targetInfo.regSet[0].argIndex = {
        emitter->rcx.getIdx(),
        emitter->rdx.getIdx(),
        emitter->r8.getIdx(),
        emitter->r9.getIdx(),
    };
    targetInfo.regSet[1].count = 10;
    targetInfo.regSet[1].types = RegisterSet::TYPE_FLOAT | RegisterSet::TYPE_VECTOR;
    targetInfo.regSet[1].retIndex = emitter->xmm0.getIdx();
    targetInfo.regSet[1].argIndex = {
        emitter->xmm0.getIdx(),
        emitter->xmm1.getIdx(),
        emitter->xmm2.getIdx(),
        emitter->xmm3.getIdx(),
    };
#elif defined(NUCLEUS_PLATFORM_LINUX)
#elif defined(NUCLEUS_PLATFORM_OSX)
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
    // Set flags
    function->flags |= FUNCTION_IS_COMPILING;

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

    function->nativeAddress = reinterpret_cast<const void*>(emitter->getCode());
    function->flags |= FUNCTION_IS_COMPILED;
    return true;
}

bool X86Compiler::compile(Module* module) {
    for (auto function : module->functions) {
        optimize(function);
        compile(function);
    }
    return true;
}

}  // namespace x86
}  // namespace backend
}  // namespace cpu
