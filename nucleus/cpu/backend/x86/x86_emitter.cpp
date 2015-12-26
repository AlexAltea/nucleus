/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "x86_emitter.h"
#include "nucleus/cpu/backend/x86/x86_compiler.h"

namespace cpu {
namespace backend {
namespace x86 {

X86Emitter::X86Emitter(const X86Compiler* compiler) :
    X86Assembler(1 * 1024 * 1024),
    compiler(compiler) {
}

X86Emitter::X86Emitter(const X86Compiler* compiler, void* address, U64 size) :
    X86Assembler(size, address),
    compiler(compiler) {
}

bool X86Emitter::isExtensionAvailable(U32 queriedExtension) const {
    return (compiler->extensions & queriedExtension);
}

const Settings& X86Emitter::settings() const {
    return compiler->settings;
}

}  // namespace x86
}  // namespace backend
}  // namespace cpu
