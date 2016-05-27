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
 *  - Constant-Formation Instructions (Chapter 4)
 *  - Integer and Logical Instructions (Chapter 5)
 *  - Shift and Rotate Instructions (Chapter 6)
 */

// Constant-Formation Instructions (Chapter 4)
void Translator::fsmbi(Instruction code)
{
    V128 value;
    value.u8[0x0] = (code.i16 >> 0x0) & 1 ? 0xFF : 00;
    value.u8[0x1] = (code.i16 >> 0x1) & 1 ? 0xFF : 00;
    value.u8[0x2] = (code.i16 >> 0x2) & 1 ? 0xFF : 00;
    value.u8[0x3] = (code.i16 >> 0x3) & 1 ? 0xFF : 00;
    value.u8[0x4] = (code.i16 >> 0x4) & 1 ? 0xFF : 00;
    value.u8[0x5] = (code.i16 >> 0x5) & 1 ? 0xFF : 00;
    value.u8[0x6] = (code.i16 >> 0x6) & 1 ? 0xFF : 00;
    value.u8[0x7] = (code.i16 >> 0x7) & 1 ? 0xFF : 00;
    value.u8[0x8] = (code.i16 >> 0x8) & 1 ? 0xFF : 00;
    value.u8[0x9] = (code.i16 >> 0x9) & 1 ? 0xFF : 00;
    value.u8[0xA] = (code.i16 >> 0xA) & 1 ? 0xFF : 00;
    value.u8[0xB] = (code.i16 >> 0xB) & 1 ? 0xFF : 00;
    value.u8[0xC] = (code.i16 >> 0xC) & 1 ? 0xFF : 00;
    value.u8[0xD] = (code.i16 >> 0xD) & 1 ? 0xFF : 00;
    value.u8[0xE] = (code.i16 >> 0xE) & 1 ? 0xFF : 00;
    value.u8[0xF] = (code.i16 >> 0xF) & 1 ? 0xFF : 00;
    Value* rt = builder.getConstantV128(value);

    setGPR(code.rt, rt);
}

void Translator::il(Instruction code)
{
    V128 value;
    value.s32[0] = code.i16;
    value.s32[1] = code.i16;
    value.s32[2] = code.i16;
    value.s32[3] = code.i16;
    Value* rt = builder.getConstantV128(value);

    setGPR(code.rt, rt);
}

void Translator::ila(Instruction code)
{
    V128 value;
    value.u32[0] = code.i18 & 0x3FFFF;
    value.u32[1] = code.i18 & 0x3FFFF;
    value.u32[2] = code.i18 & 0x3FFFF;
    value.u32[3] = code.i18 & 0x3FFFF;
    Value* rt = builder.getConstantV128(value);

    setGPR(code.rt, rt);
}

void Translator::ilh(Instruction code)
{
    V128 value;
    value.u16[0] = code.i16;
    value.u16[1] = code.i16;
    value.u16[2] = code.i16;
    value.u16[3] = code.i16;
    value.u16[4] = code.i16;
    value.u16[5] = code.i16;
    value.u16[6] = code.i16;
    value.u16[7] = code.i16;
    Value* rt = builder.getConstantV128(value);

    setGPR(code.rt, rt);
}

void Translator::ilhu(Instruction code)
{
    V128 value;
    value.u32[0] = code.i16 << 16;
    value.u32[1] = code.i16 << 16;
    value.u32[2] = code.i16 << 16;
    value.u32[3] = code.i16 << 16;
    Value* rt = builder.getConstantV128(value);

    setGPR(code.rt, rt);
}

void Translator::iohl(Instruction code)
{
    Value* constant;
    Value* rt = getGPR(code.rt);

    V128 value;
    value.u32[0] = code.i16 & 0xFFFF;
    value.u32[1] = code.i16 & 0xFFFF;
    value.u32[2] = code.i16 & 0xFFFF;
    value.u32[3] = code.i16 & 0xFFFF;
    constant = builder.getConstantV128(value);
    rt = builder.createOr(rt, constant);

    setGPR(code.rt, rt);
}


// Integer and Logical Instructions (Chapter 5)
void Translator::a(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createVAdd(ra, rb, COMPONENT_I32);

    setGPR(code.rt, rt);
}

void Translator::absdb(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createVSub(ra, rb, COMPONENT_I8);
    rt = builder.createVAbs(rt, COMPONENT_I8);

    setGPR(code.rt, rt);
}

void Translator::addx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt = getGPR(code.rt);

    Value* mask = builder.getConstantV128(V128::from_u32(1, 1, 1, 1));
    rt = builder.createAnd(rt, mask);
    rt = builder.createVAdd(rt, ra, COMPONENT_I32);
    rt = builder.createVAdd(rt, rb, COMPONENT_I32);

    setGPR(code.rt, rt);
}

void Translator::ah(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createVAdd(ra, rb, COMPONENT_I16);

    setGPR(code.rt, rt);
}

void Translator::ahi(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    V128 value;
    value.u16[0] = code.i10;
    value.u16[1] = code.i10;
    value.u16[2] = code.i10;
    value.u16[3] = code.i10;
    value.u16[4] = code.i10;
    value.u16[5] = code.i10;
    value.u16[6] = code.i10;
    value.u16[7] = code.i10;
    rt = builder.getConstantV128(value);
    rt = builder.createVAdd(rt, ra, COMPONENT_I16);

    setGPR(code.rt, rt);
}

void Translator::ai(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    V128 value;
    value.u32[0] = code.i10;
    value.u32[1] = code.i10;
    value.u32[2] = code.i10;
    value.u32[3] = code.i10;
    rt = builder.getConstantV128(value);
    rt = builder.createVAdd(rt, ra, COMPONENT_I32);

    setGPR(code.rt, rt);
}

void Translator::and_(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createAnd(ra, rb);

    setGPR(code.rt, rt);
}

void Translator::andc(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rb = builder.createNot(rb);
    rt = builder.createAnd(ra, rb);

    setGPR(code.rt, rt);
}

void Translator::andbi(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    V128 value;
    for (Size i = 0; i < sizeof(V128) / sizeof(U08); i++) {
        value.u8[i] = code.i10;
    }
    rt = builder.createAnd(ra, builder.getConstantV128(value));

    setGPR(code.rt, rt);
}

void Translator::andhi(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    V128 value;
    for (Size i = 0; i < sizeof(V128) / sizeof(U16); i++) {
        value.u16[i] = code.i10;
    }
    rt = builder.createAnd(ra, builder.getConstantV128(value));

    setGPR(code.rt, rt);
}

void Translator::andi(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    V128 value;
    for (Size i = 0; i < sizeof(V128) / sizeof(U32); i++) {
        value.u32[i] = code.i10;
    }
    rt = builder.createAnd(ra, builder.getConstantV128(value));

    setGPR(code.rt, rt);
}

void Translator::avgb(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createVAvg(ra, rb, COMPONENT_I8 | ARITHMETIC_UNSIGNED);

    setGPR(code.rt, rt);
}

void Translator::bg(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    auto result = builder.createVCmpUGT(ra, rb, COMPONENT_I32);
    rt = builder.createZExt(result, TYPE_I32);

    setGPR(code.rt, rt);
}

void Translator::bgx(Instruction code)
{
    INTERPRET({
        S64 r;
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        for (Size i = 0; i < 4; i++) {
            r = (U64)rb.u32[i] - (U64)ra.u32[i] - (U64)(1 - (state.r[o.rt].u32[i] & 1));
            state.r[o.rt].u32[i] = (r < 0) ? 0 : 1;
        }
    });
}

void Translator::cg(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].u32[0] = ((ra.u32[0] + rb.u32[0]) < ra.u32[0]) ? 1 : 0;
        }
    });
}

void Translator::cgx(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].u32[i] = ((U64)ra.u32[i] + (U64)rb.u32[i] + (U64)(state.r[o.rt].u32[i] & 1)) >> 32;
        }
    });
}

void Translator::clz(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::cntb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::eqv(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rb = builder.createNot(rb);
    rt = builder.createXor(ra, rb);

    setGPR(code.rt, rt);
}

void Translator::fsm(Instruction code)
{
    INTERPRET({
        const U32 ps = state.r[o.ra].u32[3];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].u32[i] = (ps & (1 << i)) ? ~0 : 0;
        }
    });
}

void Translator::fsmb(Instruction code)
{
    INTERPRET({
        const U32 ps = state.r[o.ra].u32[3];
        for (Size i = 0; i < 16; i++) {
            state.r[o.rt].u8[i] = (ps & (1 << i)) ? ~0 : 0;
        }
    });
}

void Translator::fsmh(Instruction code)
{
    INTERPRET({
        const U32 ps = state.r[o.ra].u32[3];
        for (Size i = 0; i < 8; i++) {
            state.r[o.rt].u16[i] = (ps & (1 << i)) ? ~0 : 0;
        }
    });
}

void Translator::gb(Instruction code)
{
    INTERPRET({
        U32 temp = 0;
        for (Size i = 0; i < 4; i++) {
            temp |= (state.r[o.ra].u32[i] & 1) << i;
        }
        state.r[o.rt].u32[3] = temp;
        state.r[o.rt].u32[2] = 0;
        state.r[o.rt].u32[1] = 0;
        state.r[o.rt].u64[0] = 0;
    });
}

void Translator::gbb(Instruction code)
{
    INTERPRET({
        U32 temp = 0;
        for (Size i = 0; i < 16; i++) {
            temp |= (state.r[o.ra].u8[i] & 1) << i;
        }
        state.r[o.rt].u32[3] = temp;
        state.r[o.rt].u32[2] = 0;
        state.r[o.rt].u32[1] = 0;
        state.r[o.rt].u64[0] = 0;
    });
}

void Translator::gbh(Instruction code)
{
    INTERPRET({
        U32 temp = 0;
        for (Size i = 0; i < 8; i++) {
            temp |= (state.r[o.ra].u16[i] & 1) << i;
        }
        state.r[o.rt].u32[3] = temp;
        state.r[o.rt].u32[2] = 0;
        state.r[o.rt].u32[1] = 0;
        state.r[o.rt].u64[0] = 0;
    });
}

void Translator::mpy(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].s32[i] = ra.s16[i*2] * rb.s16[i*2];
        }
    });
}

void Translator::mpya(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        const auto& rc = state.r[o.rc];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].s32[i] = ra.s16[i*2] * rb.s16[i*2] + rc.s32[i];
        }
    });
}

void Translator::mpyh(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].s32[i] = (ra.s16[i*2 + 1] * rb.s16[i*2]) << 16;
        }
    });
}

void Translator::mpyhh(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].s32[i] = ra.s16[i*2 + 1] * rb.s16[i*2 + 1];
        }
    });
}

void Translator::mpyhha(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].s32[i] += ra.s16[i*2 + 1] * rb.s16[i*2 + 1];
        }
    });
}

void Translator::mpyhhau(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].u32[i] += ra.u16[i*2 + 1] * rb.u16[i*2 + 1];
        }
    });
}

void Translator::mpyhhu(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].u32[i] = ra.u16[i*2 + 1] * rb.u16[i*2 + 1];
        }
    });
}

void Translator::mpyi(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].s32[i] = ra.s16[i*2] * (S32)(o.i10);
        }
    });
}

void Translator::mpys(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].s32[i] = (ra.s16[i*2] * rb.s16[i*2]) >> 16;
        }
    });
}

void Translator::mpyu(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].u32[i] = ra.u16[i*2] * rb.u16[i*2];
        }
    });
}

void Translator::mpyui(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].u32[i] = ra.u16[i*2] * (U16)(o.i10 & 0xFFFF);
        }
    });
}

void Translator::nand(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createAnd(ra, rb);
    rt = builder.createNot(rt);

    setGPR(code.rt, rt);
}

void Translator::nor(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createOr(ra, rb);
    rt = builder.createNot(rt);

    setGPR(code.rt, rt);
}

void Translator::or_(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createOr(ra, rb);

    setGPR(code.rt, rt);
}

void Translator::orbi(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    V128 value;
    for (Size i = 0; i < sizeof(V128) / sizeof(U08); i++) {
        value.u8[i] = code.i10;
    }
    rt = builder.createOr(ra, builder.getConstantV128(value));

    setGPR(code.rt, rt);
}

void Translator::orc(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rb = builder.createNot(rb);
    rt = builder.createOr(ra, rb);

    setGPR(code.rt, rt);
}

void Translator::orhi(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    V128 value;
    for (Size i = 0; i < sizeof(V128) / sizeof(U16); i++) {
        value.u16[i] = code.i10;
    }
    rt = builder.createOr(ra, builder.getConstantV128(value));

    setGPR(code.rt, rt);
}

void Translator::ori(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    V128 value;
    for (Size i = 0; i < sizeof(V128) / sizeof(U32); i++) {
        value.u32[i] = code.i10;
    }
    rt = builder.createOr(ra, builder.getConstantV128(value));

    setGPR(code.rt, rt);
}

void Translator::orx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::selb(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rc = getGPR(code.rc);
    Value* rt;

    Value* selA = builder.createAnd(ra, builder.createNot(rc));
    Value* selB = builder.createAnd(rb, rc);
    rt = builder.createOr(selA, selB);

    setGPR(code.rt, rt);
}

void Translator::sf(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createVSub(rb, ra, COMPONENT_I32);

    setGPR(code.rt, rt);
}

void Translator::sfh(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createVSub(rb, ra, COMPONENT_I16);

    setGPR(code.rt, rt);
}

void Translator::sfhi(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    V128 value;
    value.u16[0] = code.i10;
    value.u16[1] = code.i10;
    value.u16[2] = code.i10;
    value.u16[3] = code.i10;
    value.u16[4] = code.i10;
    value.u16[5] = code.i10;
    value.u16[6] = code.i10;
    value.u16[7] = code.i10;
    rt = builder.getConstantV128(value);
    rt = builder.createVSub(rt, ra, COMPONENT_I16);

    setGPR(code.rt, rt);
}

void Translator::sfi(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    V128 value;
    value.u32[0] = code.i10;
    value.u32[1] = code.i10;
    value.u32[2] = code.i10;
    value.u32[3] = code.i10;
    rt = builder.getConstantV128(value);
    rt = builder.createVSub(rt, ra, COMPONENT_I32);

    setGPR(code.rt, rt);
}

void Translator::sfx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::shufb(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rc = getGPR(code.rc);
    Value* rt;

    //Value* zero = builder.getConstantV128(V128::from_u8(0x00));
    //Value* mask = builder.createVCmpSGT(zero, rc, COMPONENT_I8);
    rt = builder.createShuffle(rc, ra, rb);
    //rt = builder.createAnd(rt, builder.createNot(mask));
    //// TODO: Cases 10X, 110X, 111X are wrongly translated as 0x00.

    setGPR(code.rt_, rt);
}

void Translator::sumb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::xor_(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createXor(ra, rb);

    setGPR(code.rt, rt);
}

void Translator::xorbi(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    V128 value;
    for (Size i = 0; i < sizeof(V128) / sizeof(U08); i++) {
        value.u8[i] = code.i10;
    }
    rt = builder.createXor(ra, builder.getConstantV128(value));

    setGPR(code.rt, rt);
}

void Translator::xorhi(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    V128 value;
    for (Size i = 0; i < sizeof(V128) / sizeof(U16); i++) {
        value.u16[i] = code.i10;
    }
    rt = builder.createXor(ra, builder.getConstantV128(value));

    setGPR(code.rt, rt);
}

void Translator::xori(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    V128 value;
    for (Size i = 0; i < sizeof(V128) / sizeof(U32); i++) {
        value.u32[i] = code.i10;
    }
    rt = builder.createXor(ra, builder.getConstantV128(value));

    setGPR(code.rt, rt);
}

void Translator::xsbh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::xshw(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::xswd(Instruction code)
{
    assert_always("Unimplemented");
}


// Shift and Rotate Instructions (Chapter 6)
void Translator::shl(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].u32[i] = (rb.u32[i] & 0x3F) > 31 ? 0 : ra.u32[i] << (rb.u32[i] & 0x3F);
        }
    });
}

void Translator::shlh(Instruction code)
{
    INTERPRET({
        const auto& ra = state.r[o.ra];
        const auto& rb = state.r[o.rb];
        for (Size i = 0; i < 8; i++) {
            state.r[o.rt].u16[i] = (rb.u16[i] & 0x1F) > 15 ? 0 : ra.u16[i] << (rb.u16[i] & 0x1F);
        }
    });
}

void Translator::shlhi(Instruction code)
{
    INTERPRET({
        const auto s = (o.i7 & 0x1F);
        const auto& ra = state.r[o.ra];
        for (Size i = 0; i < 8; i++) {
            state.r[o.rt].u16[i] = (s > 15) ? 0 : (ra.u16[i] << s);
        }
    });
}

void Translator::shli(Instruction code)
{
    INTERPRET({
        const auto s = (o.i7 & 0x3F);
        const auto& ra = state.r[o.ra];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].u32[i] = (s > 31) ? 0 : (ra.u32[i] << s);
        }
    });
}

void Translator::shlqbi(Instruction code)
{
    INTERPRET({
        const auto s = state.r[o.rb].u32[3] & 0x7;
        if (s) {
            const V128 temp = state.r[o.ra];
            state.r[o.rt].u32[0] = (temp.u32[0] << s);
            state.r[o.rt].u32[1] = (temp.u32[1] << s) | (temp.u32[0] >> (32 - s));
            state.r[o.rt].u32[2] = (temp.u32[2] << s) | (temp.u32[1] >> (32 - s));
            state.r[o.rt].u32[3] = (temp.u32[3] << s) | (temp.u32[2] >> (32 - s));
        } else {
            state.r[o.rt] = state.r[o.ra];
        }
    });
}

void Translator::shlqbii(Instruction code)
{
    INTERPRET({
        const auto s = o.i7 & 0x7;
        if (s) {
            const V128 temp = state.r[o.ra];
            state.r[o.rt].u32[0] = (temp.u32[0] << s);
            state.r[o.rt].u32[1] = (temp.u32[1] << s) | (temp.u32[0] >> (32 - s));
            state.r[o.rt].u32[2] = (temp.u32[2] << s) | (temp.u32[1] >> (32 - s));
            state.r[o.rt].u32[3] = (temp.u32[3] << s) | (temp.u32[2] >> (32 - s));
        } else {
            state.r[o.rt] = state.r[o.ra];
        }
    });
}

void Translator::shlqby(Instruction code)
{
    INTERPRET({
        const auto s = o.i7 & 0x1F;
        const V128 temp = state.r[o.ra];
        state.r[o.rt] = V128{};
        for (Size i = s; i < 16; i++) {
            state.r[o.rt].u8[i] = temp.u8[i - s];
        }
    });
}

void Translator::shlqbybi(Instruction code)
{
    INTERPRET({
        const auto s = (state.r[o.rb].u32[3] >> 3) & 0x1F;
        const V128 temp = state.r[o.ra];
        state.r[o.rt] = V128{};
        for (Size i = s; i < 16; i++) {
            state.r[o.rt].u8[i] = temp.u8[i - s];
        }
    });
}

void Translator::shlqbyi(Instruction code)
{
    INTERPRET({
        const auto s = o.i7 & 0x1F;
        const V128 temp = state.r[o.ra];
        state.r[o.rt] = V128{};
        for (Size i = s; i < 16; i++) {
            state.r[o.rt].u8[i] = temp.u8[i - s];
        }
    });
}

void Translator::rot(Instruction code)
{
    INTERPRET({
        const auto& a = state.r[o.ra];
        const auto& b = state.r[o.rb];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].u32[i] = (a.u32[i] << b.s32[i]) | (a.u32[i] >> (32 - b.s32[i]));
        }
    });
}

void Translator::roth(Instruction code)
{
    INTERPRET({
        const auto& a = state.r[o.ra];
        const auto& b = state.r[o.rb];
        for (Size i = 0; i < 8; i++) {
            state.r[o.rt].u16[i] = (a.u16[i] << b.s16[i]) | (a.u16[i] >> (16 - b.s16[i]));
        }
    });
}

void Translator::rothi(Instruction code)
{
    INTERPRET({
        const auto s = o.i7 & 0xF;
        const auto& a = state.r[o.ra];
        for (Size i = 0; i < 8; i++) {
            state.r[o.rt].u16[i] = (a.u16[i] << s) | (a.u16[i] >> (16 - s));
        }
    });
}

void Translator::rothm(Instruction code)
{
    INTERPRET({
        const auto& a = state.r[o.ra];
        const auto& b = state.r[o.rb];
        for (Size i = 0; i < 8; i++) {
            state.r[o.rt].u16[i] = (a.u16[i] >> (0 - b.u16[i]));
        }
    });
}

void Translator::rothmi(Instruction code)
{
    INTERPRET({
        const auto s = (0 - o.i7) & 0x1F;
        const auto& a = state.r[o.ra];
        for (Size i = 0; i < 8; i++) {
            state.r[o.rt].u16[i] = s < 16 ? a.u16[i] >> s : 0;
        }
    });
}

void Translator::roti(Instruction code)
{
    INTERPRET({
        const auto s = o.i7 & 0x1F;
        const auto& a = state.r[o.ra];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].u32[i] = (a.u32[i] << s) | (a.u32[i] >> (32 - s));
        }
    });
}

void Translator::rotm(Instruction code)
{
    INTERPRET({
        const auto& a = state.r[o.ra];
        const auto& b = state.r[o.rb];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].u32[i] = (a.u32[i] >> (0 - b.u32[i]));
        }
    });
}

void Translator::rotma(Instruction code)
{
    INTERPRET({
        const auto& a = state.r[o.ra];
        const auto& b = state.r[o.rb];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].s32[i] = (a.s32[i] >> (0 - b.u32[i]));
        }
    });
}

void Translator::rotmah(Instruction code)
{
    INTERPRET({
        const auto& a = state.r[o.ra];
        const auto& b = state.r[o.rb];
        for (Size i = 0; i < 8; i++) {
            state.r[o.rt].s16[i] = (a.s16[i] >> (0 - b.u16[i]));
        }
    });
}

void Translator::rotmahi(Instruction code)
{
    INTERPRET({
        const auto s = (0 - o.i7) & 0x1F;
        const auto& a = state.r[o.ra];
        for (Size i = 0; i < 8; i++) {
            state.r[o.rt].s16[i] = s < 16 ? a.s16[i] >> s : a.s16[i] >> 15;
        }
    });
}

void Translator::rotmai(Instruction code)
{
    INTERPRET({
        const auto s = (0 - o.i7) & 0x3F;
        const auto& a = state.r[o.ra];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].s32[i] = s < 32 ? a.s32[i] >> s : a.s32[i] >> 31;
        }
    });
}

void Translator::rotmi(Instruction code)
{
    INTERPRET({
        const auto s = (0 - o.i7) & 0x3F;
        const auto& a = state.r[o.ra];
        for (Size i = 0; i < 4; i++) {
            state.r[o.rt].u32[i] = s < 32 ? a.u32[i] >> s : 0;
        }
    });
}

void Translator::rotqbi(Instruction code)
{
    INTERPRET({
        const auto s = state.r[o.rb].u32[3] & 0x7;
        if (s) {
            const V128 temp = state.r[o.ra];
            state.r[o.rt].u32[0] = (temp.u32[0] << s) | (temp.u32[3] >> (32 - s));
            state.r[o.rt].u32[1] = (temp.u32[1] << s) | (temp.u32[0] >> (32 - s));
            state.r[o.rt].u32[2] = (temp.u32[2] << s) | (temp.u32[1] >> (32 - s));
            state.r[o.rt].u32[3] = (temp.u32[3] << s) | (temp.u32[2] >> (32 - s));
        } else {
            state.r[o.rt] = state.r[o.ra];
        }
    });
}

void Translator::rotqbii(Instruction code)
{
    INTERPRET({
        const auto s = o.i7 & 0x7;
        if (s) {
            const V128 temp = state.r[o.ra];
            state.r[o.rt].u32[0] = (temp.u32[0] << s) | (temp.u32[3] >> (32 - s));
            state.r[o.rt].u32[1] = (temp.u32[1] << s) | (temp.u32[0] >> (32 - s));
            state.r[o.rt].u32[2] = (temp.u32[2] << s) | (temp.u32[1] >> (32 - s));
            state.r[o.rt].u32[3] = (temp.u32[3] << s) | (temp.u32[2] >> (32 - s));
        } else {
            state.r[o.rt] = state.r[o.ra];
        }
    });
}

void Translator::rotqby(Instruction code)
{
    INTERPRET({
        const auto s = o.i7 & 0xF;
        const auto& ra = state.r[o.ra];
        for (Size i = 0; i < 16; i++) {
            state.r[o.rt].u8[i] = ra.u8[(i - s) & 0xF];
        }
    });
}

void Translator::rotqbybi(Instruction code)
{
    INTERPRET({
        const auto s = (state.r[o.rb].u32[3] >> 3) & 0xF;
        const V128 temp = state.r[o.ra];
        for (Size i = 0; i < 16; i++) {
            state.r[o.rt].u8[i] = temp.u8[(i - s) & 0xF];
        }
    });
}

void Translator::rotqbyi(Instruction code)
{
    INTERPRET({
        const auto s = o.i7 & 0xF;
        const V128 temp = state.r[o.ra];
        for (Size i = 0; i < 16; i++) {
            state.r[o.rt].u8[i] = temp.u8[(i - s) & 0xF];
        }
    });
}

void Translator::rotqmbi(Instruction code)
{
    INTERPRET({
        const auto s = (0 - state.r[o.rb].u32[3]) & 0x7;
        if (s) {
            const V128 temp = state.r[o.ra];
            state.r[o.rt].u32[0] = (temp.u32[0] >> s) | (temp.u32[1] << (32 - s));
            state.r[o.rt].u32[1] = (temp.u32[1] >> s) | (temp.u32[2] << (32 - s));
            state.r[o.rt].u32[2] = (temp.u32[2] >> s) | (temp.u32[3] << (32 - s));
            state.r[o.rt].u32[3] = (temp.u32[3] >> s);
        } else {
            state.r[o.rt] = state.r[o.ra];
        }
    });
}

void Translator::rotqmbii(Instruction code)
{
    INTERPRET({
        const auto s = (0 - o.i7) & 0x7;
        if (s) {
            const V128 temp = state.r[o.ra];
            state.r[o.rt].u32[0] = (temp.u32[0] >> s) | (temp.u32[1] << (32 - s));
            state.r[o.rt].u32[1] = (temp.u32[1] >> s) | (temp.u32[2] << (32 - s));
            state.r[o.rt].u32[2] = (temp.u32[2] >> s) | (temp.u32[3] << (32 - s));
            state.r[o.rt].u32[3] = (temp.u32[3] >> s);
        } else {
            state.r[o.rt] = state.r[o.ra];
        }
    });
}

void Translator::rotqmby(Instruction code)
{
    INTERPRET({
        const auto s = (0 - state.r[o.rb].u32[3]) & 0x1F;
        const V128 temp = state.r[o.ra];
        state.r[o.rt] = V128{};
        for (Size i = 0; i < 16 - s; i++) {
            state.r[o.rt].u8[i] = temp.u8[i + s];
        }
    });
}

void Translator::rotqmbybi(Instruction code)
{
    INTERPRET({
        const auto s = (0 - (state.r[o.rb].u32[3] >> 3)) & 0x1F;
        const V128 temp = state.r[o.ra];
        state.r[o.rt] = V128{};
        for (Size i = 0; i < 16 - s; i++) {
            state.r[o.rt].u8[i] = temp.u8[i + s];
        }
    });
}

void Translator::rotqmbyi(Instruction code)
{
    INTERPRET({
        const auto s = (0 - o.i7) & 0x1F;
        const V128 temp = state.r[o.ra];
        state.r[o.rt] = V128{};
        for (Size i = 0; i < 16 - s; i++) {
            state.r[o.rt].u8[i] = temp.u8[i + s];
        }
    });
}

}  // namespace spu
}  // namespace frontend
}  // namespace cpu
