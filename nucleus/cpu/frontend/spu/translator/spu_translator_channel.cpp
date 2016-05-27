/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "spu_translator.h"
#include "nucleus/assert.h"
#include "nucleus/cpu/frontend/spu/spu_state.h"
#include "nucleus/cpu/frontend/spu/spu_thread.h"

#define INTERPRET(func) \
    builder.createCall(builder.getExternFunction( \
        reinterpret_cast<void*>( \
        reinterpret_cast<uintptr_t>( \
        static_cast<void(*)(Instruction)>([](Instruction i) { \
            auto& thread = *static_cast<frontend::spu::SPUThread*>(CPU::getCurrentThread()); \
            auto& state = *thread.state.get(); \
            func \
        }))), \
    TYPE_VOID, { TYPE_I32 }), { builder.getConstantI32(code.value) });

namespace cpu {
namespace frontend {
namespace spu {

using namespace cpu::hir;

/**
 * SPU Instructions:
 *  - Channel Instructions (Chapter 11)
 */

// Channel Instructions (Chapter 11)
void Translator::rchcnt(Instruction code)
{
    INTERPRET({
        assert_always("Unimplemented");
    });
}

void Translator::rdch(Instruction code)
{
    INTERPRET({
        U32& result = state.r[i.rt].u32[3];
        switch (i.ca) {
        case MFC_LSA:
            result = state.mfc.lsa;
            break;
        case MFC_EAH:
            result = state.mfc.eah;
            break;
        case MFC_EAL:
            result = state.mfc.eal;
            break;
        case MFC_Size:
            result = state.mfc.size;
            break;
        default:
            assert_always("Unimplemented");
        }
    });
}

void Translator::wrch(Instruction code)
{
    INTERPRET({
        U32 value = state.r[i.rt].u32[3];
        switch (i.ca) {
        case MFC_LSA:
            assert_true(value < 0x40000);
            state.mfc.lsa = value;
            break;
        case MFC_EAH:
            state.mfc.eah = value;
            break;
        case MFC_EAL:
            state.mfc.eal = value;
            break;
        case MFC_Size:
            assert_true(value <= 16_KB);
            state.mfc.size = value;
            break;
        case MFC_TagID:
            state.mfc.tag = value;
            break;
        case MFC_Cmd:
            thread.mfcCommand(value);
            break;
        case MFC_WrTagMask:
        case MFC_WrTagUpdate:
        default:
            assert_always("Unimplemented");
        }
    });
}

}  // namespace spu
}  // namespace frontend
}  // namespace cpu
