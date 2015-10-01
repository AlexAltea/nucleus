/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "x86_compiler.h"
#include "nucleus/emulator.h"
#include "nucleus/logger/logger.h"
#include "nucleus/cpu/backend/x86/x86_sequences.h"

#include <queue>

namespace cpu {
namespace backend {
namespace x86 {

using namespace cpu::hir;

X86Compiler::X86Compiler() : Compiler() {
    init();
}

X86Compiler::X86Compiler(const Settings& settings) : Compiler(settings) {
    init();
}

void X86Compiler::init() {
    // Initialize sequences
    X86Sequences::init();

    // Set target information
#if defined(NUCLEUS_PLATFORM_WINDOWS)
    targetInfo.regSets[0].count = 5;
    targetInfo.regSets[0].types = RegisterSet::TYPE_INT;
    targetInfo.regSets[0].retIndex = 0; // rax
    targetInfo.regSets[0].argIndex = {1, 2, 8, 9}; // {rcx, rdx, r8, r9};
    targetInfo.regSets[1].count = 10;
    targetInfo.regSets[1].types = RegisterSet::TYPE_FLOAT | RegisterSet::TYPE_VECTOR;
    targetInfo.regSets[1].retIndex = 0; // xmm0
    targetInfo.regSets[1].argIndex = {0, 1, 2, 3}; // {xmm0, xmm1, xmm2, xmm3};
#elif defined(NUCLEUS_PLATFORM_LINUX)
#elif defined(NUCLEUS_PLATFORM_OSX)
#endif
}

bool X86Compiler::compile(Block* block) {
    // TODO
    logger.warning(LOG_CPU, "Unimplemented compiler method");
    return false;
}

bool X86Compiler::compile(Function* function) {
    // Set flags
    function->flags |= FUNCTION_IS_COMPILING;

    // Run compiler passes
    optimize(function);

    // Initialize emitter
    X86Emitter emitter(settings, function->nativeAddress, function->nativeSize);
#if defined(NUCLEUS_ARCH_X86_32BITS)
    emitter.mode = X86_MODE_32BITS;
#elif defined(NUCLEUS_ARCH_X86_64BITS)
    emitter.mode = X86_MODE_64BITS;
#else
    logger.error(LOG_CPU, "Unsupported variant of the x86 architecture");
#endif

    // Iterate over blocks
    std::queue<Block*> blocks({ function->entry });
    while (!blocks.empty()) {
        auto& block = blocks.front();

        // Compile block
        auto& instructions = block->instructions;
        for (auto it = instructions.begin(); it != instructions.end(); it++) {
            if (!X86Sequences::select(emitter, *it)) {
                logger.error(LOG_CPU, "Cannot compile block");
                return false;
            }
        }
        blocks.pop();
    }

    function->flags |= FUNCTION_IS_COMPILED;
    return true;
}

bool X86Compiler::compile(Module* module) {
    for (auto function : module->functions) {
        if (!compile(function)) {
            logger.error(LOG_CPU, "Cannot compile function");
            return false;
        }
    }
    return true;
}

void X86Compiler::translationEnter() {
    // Get current state address
    void* state = static_cast<frontend::ppu::Thread*>(nucleus.cell.getCurrentThread())->state.get();

    // Generate code
    X86Emitter e(settings);
    e.mov(e.rbx, reinterpret_cast<size_t>(state));
    e.ret();
   
    // Execute real translation-enter function
    auto function = reinterpret_cast<void(*)()>(e.getCode());
    function();
}

void X86Compiler::translationExit() {
}

}  // namespace x86
}  // namespace backend
}  // namespace cpu
