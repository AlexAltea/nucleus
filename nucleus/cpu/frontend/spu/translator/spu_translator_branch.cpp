/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "spu_translator.h"
#include "nucleus/core/config.h"
#include "nucleus/assert.h"
#include "nucleus/cpu/frontend/spu/spu_state.h"
#include "nucleus/cpu/frontend/spu/spu_thread.h"

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
 *  - Compare, Branch and Halt Instructions (Chapter 7)
 *  - Hint-for-Branch Instructions (Chapter 8)
 */

// Compare, Branch and Halt Instructions (Chapter 7)
void Translator::bi(Instruction code)
{
    builder.createRet();
}

void Translator::bihnz(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::bihz(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::binz(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::bisl(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::bisled(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::biz(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::br(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::bra(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::brasl(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::brhnz(Instruction code)
{
    const U32 targetAddr = currentAddress + (code.i16 << 2);
    const U32 nextAddr = currentAddress + 4;

    Value* ps = builder.createExtract(getGPR(code.rt), builder.getConstantI8(7), TYPE_I16);
    Value* zero = builder.getConstantI16(0);
    Value* cond = builder.createCmpNE(ps, zero);

    builder.createBrCond(cond, blocks.at(targetAddr), blocks.at(nextAddr));
}

void Translator::brhz(Instruction code)
{
    const U32 targetAddr = currentAddress + (code.i16 << 2);
    const U32 nextAddr = currentAddress + 4;

    Value* ps = builder.createExtract(getGPR(code.rt), builder.getConstantI8(7), TYPE_I16);
    Value* zero = builder.getConstantI16(0);
    Value* cond = builder.createCmpEQ(ps, zero);

    builder.createBrCond(cond, blocks.at(targetAddr), blocks.at(nextAddr));
}

void Translator::brnz(Instruction code)
{
    const U32 targetAddr = currentAddress + (code.i16 << 2);
    const U32 nextAddr = currentAddress + 4;

    Value* ps = builder.createExtract(getGPR(code.rt), builder.getConstantI8(3), TYPE_I32);
    Value* zero = builder.getConstantI32(0);
    Value* cond = builder.createCmpNE(ps, zero);

    builder.createBrCond(cond, blocks.at(targetAddr), blocks.at(nextAddr));
}

void Translator::brsl(Instruction code)
{
    const U32 targetAddr = currentAddress + (code.i16 << 2);

    Value* rt = builder.getConstantI32(currentAddress + 4);
    setGPR(code.rt, rt);

    Module* module = static_cast<Module*>(function->parent);
    if (config.spuTranslator & CPU_TRANSLATOR_IS_JIT) {
        module->addFunction(targetAddr);
    }
    auto& targetFunc = static_cast<Function&>(*module->functions.at(targetAddr));
    builder.createCall(targetFunc.hirFunction);
}

void Translator::brz(Instruction code)
{
    const U32 targetAddr = currentAddress + (code.i16 << 2);
    const U32 nextAddr = currentAddress + 4;

    Value* ps = builder.createExtract(getGPR(code.rt), builder.getConstantI8(3), TYPE_I32);
    Value* zero = builder.getConstantI32(0);
    Value* cond = builder.createCmpEQ(ps, zero);

    builder.createBrCond(cond, blocks.at(targetAddr), blocks.at(nextAddr));
}

void Translator::ceq(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].u32[i] = (ra.s32[i] == rb.s32[i]) ? 0xFFFFFFFF : 0;
        }
    });
}

void Translator::ceqb(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        for (Size i = 0; i < 16; i++) {
            state.r[o.rt].u8[i] = (ra.u8[i] == rb.u8[i]) ? 0xFF : 0;
        }
    });
}

void Translator::ceqbi(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        for (Size i = 0; i < 16; i++) {
            state.r[o.rt].s8[i] = (ra.s8[i] == (S08)(o.i10 & 0xFF)) ? 0xFF : 0;
        }
    });
}

void Translator::ceqh(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        for (Size i = 0; i < 8; i++) {
            state.r[o.rt].u16[i] = (ra.u16[i] == rb.u16[i]) ? 0xFFFF : 0;
        }
    });
}

void Translator::ceqhi(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        for (Size i = 0; i < 8; i++)
            state.r[o.rt].u16[i] = (ra.s16[i] == (S16)(o.i10)) ? 0xFFFF : 0;
    
    });
}

void Translator::ceqi(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].u32[i] = (ra.s32[i] == (S32)(o.i10)) ? 0xFFFFFFFF : 0;
        }
    });
}

void Translator::cgt(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].u32[i] = (ra.s32[i] > rb.s32[i]) ? 0xFFFFFFFF : 0;
        }
    });
}

void Translator::cgtb(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        for (Size i = 0; i < 16; i++) {
            state.r[o.rt].u8[i] = (ra.s8[i] > rb.s8[i]) ? 0xFF : 0;
        }
    });
}

void Translator::cgtbi(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        for (Size i = 0; i < 16; i++) {
            state.r[o.rt].u8[i] = (ra.s8[i] > (S08)(o.i10 & 0xFF)) ? 0xFF : 0;
        }
    });
}

void Translator::cgth(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        for (Size i = 0; i < 8; i++) {
            state.r[o.rt].u16[i] = (ra.s16[i] > rb.s16[i]) ? 0xFFFF : 0;
        }
    });
}

void Translator::cgthi(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        for (Size i = 0; i < 8; i++) {
            state.r[o.rt].u16[i] = (ra.s16[i] > (S16)(o.i10)) ? 0xFFFF : 0;
        }
    });
}

void Translator::cgti(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].u32[i] = (ra.s32[i] > (S32)(o.i10)) ? 0xFFFFFFFF : 0;
        }
    });
}

void Translator::clgt(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].u32[i] = (ra.u32[i] > rb.u32[i]) ? 0xFFFFFFFF : 0;
        }
    });
}

void Translator::clgtb(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        for (Size i = 0; i < 16; i++) {
            state.r[o.rt].u8[i] = (ra.u8[i] > rb.u8[i]) ? 0xFF : 0;
        }
    });
}

void Translator::clgtbi(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        for (Size i = 0; i < 16; i++) {
            state.r[o.rt].u8[i] = (ra.u8[i] > (U08)(o.i10 & 0xFF)) ? 0xFF : 0;
        }
    });
}

void Translator::clgth(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        for (Size i = 0; i < 8; i++) {
            state.r[o.rt].u16[i] = (ra.u16[i] > rb.u16[i]) ? 0xFFFF : 0;
        }
    });
}

void Translator::clgthi(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        for (Size i = 0; i < 8; i++) {
			state.r[o.rt].u16[i] = (ra.u16[i] > (U16)(o.i10)) ? 0xFFFF : 0;
		}
    });
}

void Translator::clgti(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].u32[i] = (ra.u32[i] > (U32)(o.i10)) ? 0xFFFFFFFF : 0;
        }
    });
}

void Translator::heq(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::heqi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::hgt(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::hgti(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::hlgt(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::hlgti(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::iret(Instruction code)
{
    assert_always("Unimplemented");
}


// Hint-for-Branch Instructions (Chapter 8)
void Translator::hbr(Instruction code)
{
    // NOTE: Not implemented.
    // Although there is no need to translate these branch hints,
    // its translation could result in performance improvements.
}

void Translator::hbra(Instruction code)
{
    // NOTE: Not implemented.
    // Although there is no need to translate these branch hints,
    // its translation could result in performance improvements.
}

void Translator::hbrr(Instruction code)
{
    // NOTE: Not implemented.
    // Although there is no need to translate these branch hints,
    // its translation could result in performance improvements.
}

}  // namespace spu
}  // namespace frontend
}  // namespace cpu
