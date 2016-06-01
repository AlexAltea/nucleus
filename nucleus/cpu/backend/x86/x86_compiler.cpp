/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "x86_compiler.h"
#include "nucleus/emulator.h"
#include "nucleus/logger/logger.h"
#include "nucleus/cpu/backend/x86/x86_sequences.h"

#ifdef NUCLEUS_ARCH_X86
#include <intrin.h>
#endif

#include <cstring>
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

void X86Compiler::setExtensionsHost() {
#ifdef NUCLEUS_ARCH_X86
    extensions = 0;
    int data[4];
    __cpuid(data, 0x00000001);
    extensions |= ((data[2] >>  9) & 1) ? X86Extension::SSSE3 : 0;
    extensions |= ((data[2] >> 28) & 1) ? X86Extension::AVX   : 0;
    extensions |= 0 ? X86Extension::AVX2 : 0;
    extensions |= 0 ? X86Extension::BMI2 : 0;
    __cpuid(data, 0x80000001);
    extensions |= ((data[2] >>  5) & 1) ? X86Extension::LZCNT : 0;
    extensions |= 0 ? X86Extension::MOVBE : 0;
#endif
}

void X86Compiler::init() {
    // Initialize sequences
    X86Sequences::init();

    // Set target information
    setExtensionsHost();
#if defined(NUCLEUS_TARGET_WINDOWS)
    targetInfo.regSets.resize(2);
    targetInfo.regSets[0].types = RegisterSet::TYPE_INT;
    targetInfo.regSets[0].valueIndex = {10, 11, 12, 13, 14, 15}; // {r10, r11, r12, r13, r14, r15}
    targetInfo.regSets[0].argIndex = {1, 2, 8, 9}; // {rcx, rdx, r8, r9}
    targetInfo.regSets[0].retIndex = 0; // rax
    targetInfo.regSets[1].types = RegisterSet::TYPE_FLOAT | RegisterSet::TYPE_VECTOR;
    targetInfo.regSets[1].valueIndex = {6, 7, 8, 9, 10, 11, 12, 13, 14, 15}; // {xmm6, ...,  xmm15}
    targetInfo.regSets[1].argIndex = {0, 1, 2, 3}; // {xmm0, ..., xmm3}
    targetInfo.regSets[1].retIndex = 0; // xmm0
#elif defined(NUCLEUS_TARGET_LINUX)
#elif defined(NUCLEUS_TARGET_OSX)
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
    X86Emitter e(this);
#if defined(NUCLEUS_ARCH_X86_32BITS)
    e.mode = X86_MODE_32BITS;
#elif defined(NUCLEUS_ARCH_X86_64BITS)
    e.mode = X86_MODE_64BITS;
#else
    logger.error(LOG_CPU, "Unsupported variant of the x86 architecture");
#endif

    // Prolog block
    e.L(e.labelProlog);
    e.sub(e.rsp, 0x28);
    if (!(function->blocks[0]->flags & BLOCK_IS_ENTRY)) {
        e.jmp(e.labelEntry, e.T_NEAR);
    }

    // Prepare labels
    for (const auto& block : function->blocks) {
        e.labels[block] = Xbyak::Label();
    }

    // Iterate over blocks
    for (const auto& block : function->blocks) {
        e.L(e.labels[block]);
        if (block->flags & BLOCK_IS_ENTRY) {
            e.L(e.labelEntry);
        }
        for (const auto& instr : block->instructions) {
            if (!X86Sequences::select(e, instr)) {
                logger.error(LOG_CPU, "Cannot compile block");
                return false;
            }
        }
    }

    // Epilog block
    e.L(e.labelEpilog);
    e.add(e.rsp, 0x28);
    e.ret();

    // Copy emitted code
    const auto codeSize = e.getSize();
    function->nativeSize = codeSize;
    function->nativeAddress = allocRWXMemory(codeSize);
    memcpy(function->nativeAddress, e.getCode(), codeSize);

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

bool X86Compiler::call(hir::Function* function, void* state, const std::vector<hir::Value*>& args) {
    if (!(function->flags & FUNCTION_IS_COMPILED)) {
        logger.error(LOG_CPU, "Function is not ready");
        return false;
    }

    // Generate code for caller
    X86Emitter e(this);
    e.push(e.rbx);
    e.push(e.r10);
    e.push(e.r11);
    e.push(e.r12);
    e.push(e.r13);
    e.push(e.r14);
    e.push(e.r15);
    e.mov(e.rbx, reinterpret_cast<size_t>(state));
    e.mov(e.rax, reinterpret_cast<size_t>(function->nativeAddress));
    e.call(e.rax);
    e.pop(e.r15);
    e.pop(e.r14);
    e.pop(e.r13);
    e.pop(e.r12);
    e.pop(e.r11);
    e.pop(e.r10);
    e.pop(e.rbx);
    e.ret();

    // Execute caller
    auto callerFunc = reinterpret_cast<void(*)()>(e.getCode());
    callerFunc();
    return true;
}

}  // namespace x86
}  // namespace backend
}  // namespace cpu
