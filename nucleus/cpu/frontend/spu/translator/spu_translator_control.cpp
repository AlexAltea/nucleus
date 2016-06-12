/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "spu_translator.h"
#include "nucleus/assert.h"
#include "nucleus/cpu/frontend/spu/spu_state.h"
#include "nucleus/cpu/frontend/spu/spu_thread.h"

#ifdef NUCLEUS_ARCH_X86
#ifdef NUCLEUS_COMPILER_MSVC
#include <intrin.h>
#else
#include <x86intrin.h>
#endif
#endif

#define INTERPRET(func) \
    builder.createCall(builder.getExternFunction( \
        reinterpret_cast<void*>( \
        reinterpret_cast<uintptr_t>( \
        static_cast<void(*)(Instruction)>([](Instruction o) { \
            auto& state = *static_cast<frontend::spu::SPUThread*>(CPU::getCurrentThread())->state.get(); \
            func \
        }))), \
    TYPE_VOID, { TYPE_I32 }), { builder.getConstantI32(code.value) });

namespace cpu {
namespace frontend {
namespace spu {

using namespace cpu::hir;

/**
 * SPU Instructions:
 *  - Control Instructions (Chapter 10)
 */

// Control Instructions (Chapter 10)
void Translator::dsync(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lnop(Instruction code)
{
    // NOTE: No need to implement this instruction.
}

void Translator::mfspr(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::mtspr(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::nop(Instruction code)
{
    // NOTE: No need to implement this instruction.
}

void Translator::stop(Instruction code)
{
    // TODO: Handle signal
    builder.createRet();
}

void Translator::stopd(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::sync(Instruction code)
{
#ifdef NUCLEUS_ARCH_X86
    INTERPRET({
        _mm_mfence();
    });
#else
    assert_always("Unimplemented");
#endif
}

}  // namespace spu
}  // namespace frontend
}  // namespace cpu
