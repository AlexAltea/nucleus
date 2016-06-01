/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_translator.h"
#include "nucleus/assert.h"
#include "nucleus/cpu/frontend/ppu/ppu_state.h"
#include "nucleus/cpu/frontend/ppu/ppu_thread.h"

// PowerPC Interpreter helper
#include <algorithm>

namespace cpu {
namespace frontend {
namespace ppu {

#define INTERPRET(func) \
    builder.createCall(builder.getExternFunction( \
        reinterpret_cast<void*>( \
        reinterpret_cast<uintptr_t>( \
        static_cast<void(*)(Instruction)>([](Instruction o) { \
            auto& state = *static_cast<frontend::ppu::PPUThread*>(CPU::getCurrentThread())->state.get(); \
            func \
        }))), \
    TYPE_VOID, { TYPE_I32 }), { builder.getConstantI32(code.value) });

// PowerPC Rotation-related functions
inline U64 rotl64(const U64 x, const U08 n) { return (x << n) | (x >> (64 - n)); }
inline U64 rotl32(const U32 x, const U08 n) { return rotl64((U64)x | ((U64)x << 32), n); }
inline U16 rotl16(const U16 x, const U08 n) { return (x << n) | (x >> (16 - n)); }
inline U08 rotl8 (const U08 x, const U08 n) { return (x << n) | (x >> (8 - n)); }

using namespace cpu::hir;

/**
 * PPC64 Vector/SIMD Instructions (aka AltiVec):
 *  - Vector UISA Instructions (Section: 4.2.x)
 *  - Vector VEA Instructions (Section: 4.3.x)
 */

void Translator::dss(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::dst(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::dstst(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lvebx(Instruction code)
{
    INTERPRET({
    });
}

void Translator::lvehx(Instruction code)
{
    INTERPRET({
    });
}

void Translator::lvewx(Instruction code)
{
    INTERPRET({
    });
}

void Translator::lvlx(Instruction code)
{
    INTERPRET({
    });
}

void Translator::lvlxl(Instruction code)
{
    INTERPRET({
    });
}

void Translator::lvrx(Instruction code)
{
    INTERPRET({
    });
}

void Translator::lvrxl(Instruction code)
{
    INTERPRET({
    });
}

void Translator::lvsl(Instruction code)
{
    INTERPRET({
    });
}

void Translator::lvsr(Instruction code)
{
    INTERPRET({
    });
}

void Translator::lvx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* vd;

    Value* addr = rb;
    if (code.ra) {
        addr = builder.createAdd(addr, ra);
    }

    vd = readMemory(addr, TYPE_V128);
    setVR(code.vd, vd);
}

void Translator::lvxl(Instruction code)
{
    INTERPRET({
    });
}

void Translator::mfvscr(Instruction code)
{
    INTERPRET({
    });
}

void Translator::mtvscr(Instruction code)
{
    INTERPRET({
    });
}

void Translator::stvebx(Instruction code)
{
    INTERPRET({
    });
}

void Translator::stvehx(Instruction code)
{
    INTERPRET({
    });
}

void Translator::stvewx(Instruction code)
{
    INTERPRET({
    });
}

void Translator::stvlx(Instruction code)
{
    INTERPRET({
    });
}

void Translator::stvlxl(Instruction code)
{
    INTERPRET({
    });
}

void Translator::stvrx(Instruction code)
{
    INTERPRET({
    });
}

void Translator::stvrxl(Instruction code)
{
    INTERPRET({
    });
}

void Translator::stvx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* vs = getGPR(code.vs);

    Value* addr = rb;
    if (code.ra) {
        addr = builder.createAdd(addr, ra);
    }

    writeMemory(addr, vs);
}

void Translator::stvxl(Instruction code)
{
}

void Translator::vaddcuw(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int w = 0; w < 4; w++) {
            vd.u32[w] = ~va.u32[w] < vb.u32[w];
        }
    });
}

void Translator::vaddfp(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAdd(va, vb, COMPONENT_F32);

    setVR(code.vd, vd);
}

void Translator::vaddsbs(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd = va; // TODO: This is wrong. Just a temporary fix

    // TODO: ? I8
    assert_always("Unimplemented");

    setVR(code.vd, vd);
}

void Translator::vaddshs(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd = va; // TODO: This is wrong. Just a temporary fix

    // TODO: ? I16
    assert_always("Unimplemented");

    setVR(code.vd, vd);
}

void Translator::vaddsws(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd = va; // TODO: This is wrong. Just a temporary fix

    // TODO: ? I32
    assert_always("Unimplemented");

    setVR(code.vd, vd);
}

void Translator::vaddubm(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAdd(va, vb, COMPONENT_I8);

    setVR(code.vd, vd);
}

void Translator::vaddubs(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAdd(va, vb, COMPONENT_I8 | ARITHMETIC_UNSIGNED | ARITHMETIC_SATURATE);
    assert_always("Unimplemented");
    // TODO: ? I8

    setVR(code.vd, vd);
}

void Translator::vadduhm(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAdd(va, vb, COMPONENT_I16);

    setVR(code.vd, vd);
}

void Translator::vadduhs(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd = va; // TODO: This is wrong. Just a temporary fix

    // TODO: ? I16
    assert_always("Unimplemented");

    setVR(code.vd, vd);
}

void Translator::vadduwm(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAdd(va, vb, COMPONENT_I32);

    setVR(code.vd, vd);
}

void Translator::vadduws(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd = va; // TODO: This is wrong. Just a temporary fix

    // TODO: ? I32
    assert_always("Unimplemented");

    setVR(code.vd, vd);
}

void Translator::vand(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createAnd(va, vb);

    setVR(code.vd, vd);
}

void Translator::vandc(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vb = builder.createNot(vb);
    vd = builder.createAnd(va, vb);

    setVR(code.vd, vd);
}

void Translator::vavgsb(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAvg(va, vb, COMPONENT_I8);

    setVR(code.vd, vd);
}

void Translator::vavgsh(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAvg(va, vb, COMPONENT_I16);

    setVR(code.vd, vd);
}

void Translator::vavgsw(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAvg(va, vb, COMPONENT_I32);

    setVR(code.vd, vd);
}

void Translator::vavgub(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAvg(va, vb, COMPONENT_I8 | ARITHMETIC_UNSIGNED);

    setVR(code.vd, vd);
}

void Translator::vavguh(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAvg(va, vb, COMPONENT_I16 | ARITHMETIC_UNSIGNED);

    setVR(code.vd, vd);
}

void Translator::vavguw(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAvg(va, vb, COMPONENT_I32 | ARITHMETIC_UNSIGNED);

    setVR(code.vd, vd);
}

void Translator::vcfsx(Instruction code)
{
    INTERPRET({
        const U32 scale = 1 << o.vuimm;
        for (int w = 0; w < 4; w++) {
            state.v[o.vd].f32[w] = ((F32)state.v[o.vb].s32[w]) / scale;
        }
    });
}

void Translator::vcfux(Instruction code)
{
    INTERPRET({
        const U32 scale = 1 << o.vuimm;
        for (int w = 0; w < 4; w++) {
            state.v[o.vd].f32[w] = ((F32)state.v[o.vb].u32[w]) / scale;
        }
    });
}

void Translator::vcmpbfp(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vcmpbfp_(Instruction code)
{
    vcmpbfp(code);
    // TODO: CR6 update
}

void Translator::vcmpeqfp(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVCmp(va, vb, COMPARE_EQ | COMPONENT_F32);

    setVR(code.vd, vd);
}

void Translator::vcmpeqfp_(Instruction code)
{
    vcmpeqfp(code);
    // TODO: CR6 update
}

void Translator::vcmpequb(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVCmp(va, vb, COMPARE_EQ | COMPONENT_I8);

    setVR(code.vd, vd);
}

void Translator::vcmpequb_(Instruction code)
{
    vcmpequb(code);
    // TODO: CR6 update
}

void Translator::vcmpequh(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVCmp(va, vb, COMPARE_EQ | COMPONENT_I16);

    setVR(code.vd, vd);
}

void Translator::vcmpequh_(Instruction code)
{
    vcmpequh(code);
    // TODO: CR6 update
}

void Translator::vcmpequw(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVCmp(va, vb, COMPARE_EQ | COMPONENT_I32);

    setVR(code.vd, vd);
}

void Translator::vcmpequw_(Instruction code)
{
    vcmpequw(code);
    // TODO: CR6 update
}

void Translator::vcmpgefp(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVCmp(va, vb, COMPARE_SGE | COMPONENT_F32);

    setVR(code.vd, vd);
}

void Translator::vcmpgefp_(Instruction code)
{
    vcmpgefp(code);
    // TODO: CR6 update
}

void Translator::vcmpgtfp(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVCmp(va, vb, COMPARE_SGT | COMPONENT_F32);

    setVR(code.vd, vd);
}

void Translator::vcmpgtfp_(Instruction code)
{
    vcmpgtfp(code);
    // TODO: CR6 update
}

void Translator::vcmpgtsb(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVCmp(va, vb, COMPARE_SGT | COMPONENT_I8);

    setVR(code.vd, vd);
}

void Translator::vcmpgtsb_(Instruction code)
{
    vcmpgtsb(code);
    // TODO: CR6 update
}

void Translator::vcmpgtsh(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVCmp(va, vb, COMPARE_SGT | COMPONENT_I16);

    setVR(code.vd, vd);
}

void Translator::vcmpgtsh_(Instruction code)
{
    vcmpgtsh(code);
    // TODO: CR6 update
}

void Translator::vcmpgtsw(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVCmp(va, vb, COMPARE_SGT | COMPONENT_I32);

    setVR(code.vd, vd);
}

void Translator::vcmpgtsw_(Instruction code)
{
    vcmpgtsw(code);
    // TODO: CR6 update
}

void Translator::vcmpgtub(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVCmp(va, vb, COMPARE_UGT | COMPONENT_I8);

    setVR(code.vd, vd);
}

void Translator::vcmpgtub_(Instruction code)
{
    vcmpgtub(code);
    // TODO: CR6 update
}

void Translator::vcmpgtuh(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVCmp(va, vb, COMPARE_UGT | COMPONENT_I16);

    setVR(code.vd, vd);
}

void Translator::vcmpgtuh_(Instruction code)
{
    vcmpgtuh(code);
    // TODO: CR6 update
}

void Translator::vcmpgtuw(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVCmp(va, vb, COMPARE_UGT | COMPONENT_I32);

    setVR(code.vd, vd);
}

void Translator::vcmpgtuw_(Instruction code)
{
    vcmpgtuw(code);
    // TODO: CR6 update
}

void Translator::vctsxs(Instruction code)
{
    INTERPRET({
        int nScale = 1 << o.vuimm;
        for (int w = 0; w < 4; w++) {
            F32 result = state.v[o.vb].f32[w] * nScale;

            if (result > 0x7FFFFFFF)
                state.v[o.vd].s32[w] = 0x7FFFFFFF;
            else if (result < -0x80000000LL)
                state.v[o.vd].s32[w] = -0x80000000LL;
            else {
                state.v[o.vd].s32[w] = (S32)result;
            }
        }
    });
}

void Translator::vctuxs(Instruction code)
{
    INTERPRET({
        int nScale = 1 << o.vuimm;
        for (int w = 0; w < 4; w++) {
            S64 result = (S64)(state.v[o.vb].f32[w] * nScale);

            if (result > 0xFFFFFFFF)
                state.v[o.vd].u32[w] = 0xFFFFFFFF;
            else if (result < 0)
                state.v[o.vd].u32[w] = 0;
            else
                state.v[o.vd].u32[w] = (U32)result;
        }
    });
}

void Translator::vexptefp(Instruction code)
{
    Value* vb = getVR(code.vb);
    Value* vd = vb; // TODO: This is wrong. Just a temporary fix

    //llvm::Function* exp2 = getIntrinsicFloat(llvm::Intrinsic::exp2); // TODO: Get a 4xf32 intrinsic instead
    //vd = builder.createCall(exp2, vb);
    assert_always("Unimplemented");

    setVR(code.vd, vd);
}

void Translator::vlogefp(Instruction code)
{
    Value* vb = getVR(code.vb);
    Value* vd = vb; // TODO: This is wrong. Just a temporary fix

    //llvm::Function* log2 = getIntrinsicFloat(llvm::Intrinsic::log2); // TODO: Get a 4xf32 intrinsic instead
    //vd = builder.createCall(log2, vb);
    assert_always("Unimplemented");

    setVR(code.vd, vd);
}

void Translator::vmaddfp(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vc = getVR(code.vc);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVMul(va, vc, COMPONENT_F32);
    vd = builder.createVAdd(vd, vb, COMPONENT_F32);

    setVR(code.vd, vd);
}

void Translator::vmaxfp(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int w = 0; w < 4; w++) {
            vd.f32[w] = std::max(va.f32[w], vb.f32[w]);
        }
    });
}

void Translator::vmaxsb(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int b = 0; b < 16; b++) {
            vd.s8[b] = std::max(va.s8[b], vb.s8[b]);
        }
    });
}

void Translator::vmaxsh(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int h = 0; h < 8; h++) {
            vd.s16[h] = std::max(va.s16[h], vb.s16[h]);
        }
    });
}

void Translator::vmaxsw(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int w = 0; w < 4; w++) {
            vd.s32[w] = std::max(va.s32[w], vb.s32[w]);
        }
    });
}

void Translator::vmaxub(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int b = 0; b < 16; b++) {
            vd.u8[b] = std::max(va.u8[b], vb.u8[b]);
        }
    });
}

void Translator::vmaxuh(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int h = 0; h < 8; h++) {
            vd.u16[h] = std::max(va.u16[h], vb.u16[h]);
        }
    });
}

void Translator::vmaxuw(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int w = 0; w < 4; w++) {
            vd.u32[w] = std::max(va.u32[w], vb.u32[w]);
        }
    });
}

void Translator::vmhaddshs(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        const auto& vc = state.v[o.vc];
        for (Size h = 0; h < 8; h++) {
            S32 result = (S32)va.s16[h] * (S32)vb.s16[h] + (S32)vc.s16[h];
            if (result > 0x7FFF) {
                vd.s16[h] = 0x7FFF;
                state.vscr.SAT = 1;
            }
            else if (result < 0x8000) {
                vd.s16[h] = 0x8000;
                state.vscr.SAT = 1;
            }
            else {
                vd.s16[h] = (S16)result;
            }
        }
    });
}

void Translator::vmhraddshs(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        const auto& vc = state.v[o.vc];
        for (Size h = 0; h < 8; h++) {
            S32 result = (S32)va.s16[h] * (S32)vb.s16[h] + (S32)vc.s16[h] + 0x4000;
            if (result > 0x7FFF) {
                vd.s16[h] = 0x7FFF;
                state.vscr.SAT = 1;
            }
            else if (result < 0x8000) {
                vd.s16[h] = 0x8000;
                state.vscr.SAT = 1;
            }
            else {
                vd.s16[h] = (S16)result;
            }
        }
    });
}

void Translator::vminfp(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int w = 0; w < 4; w++) {
            vd.f32[w] = std::min(va.f32[w], vb.f32[w]);
        }
    });
}

void Translator::vminsb(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int b = 0; b < 16; b++) {
            vd.s8[b] = std::min(va.s8[b], vb.s8[b]);
        }
    });
}

void Translator::vminsh(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int h = 0; h < 8; h++) {
            vd.s16[h] = std::min(va.s16[h], vb.s16[h]);
        }
    });
}

void Translator::vminsw(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int w = 0; w < 4; w++) {
            vd.s32[w] = std::min(va.s32[w], vb.s32[w]);
        }
    });
}

void Translator::vminub(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int b = 0; b < 16; b++) {
            vd.u8[b] = std::min(va.u8[b], vb.u8[b]);
        }
    });
}

void Translator::vminuh(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int h = 0; h < 8; h++) {
            vd.u16[h] = std::min(va.u16[h], vb.u16[h]);
        }
    });
}

void Translator::vminuw(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int w = 0; w < 4; w++) {
            vd.u32[w] = std::min(va.u32[w], vb.u32[w]);
        }
    });
}

void Translator::vmladduhm(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        const auto& vc = state.v[o.vc];
        for (int h = 0; h < 8; h++) {
            vd.u16[h] = va.u16[h] * vb.u16[h] + vc.u16[h];
        }
    });
}

void Translator::vmrghb(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int h = 0; h < 8; h++) {
            vd.u8[15 - h*2 + 0] = va.u8[15 - h];
            vd.u8[15 - h*2 - 1] = vb.u8[15 - h];
        }
    });
}

void Translator::vmrghh(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int w = 0; w < 4; w++) {
            vd.u16[7 - w*2 + 0] = va.u16[7 - w];
            vd.u16[7 - w*2 - 1] = vb.u16[7 - w];
        }
    });
}

void Translator::vmrghw(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int d = 0; d < 2; d++) {
            vd.u32[3 - d*2 + 0] = va.u32[3 - d];
            vd.u32[3 - d*2 - 1] = vb.u32[3 - d];
        }
    });
}

void Translator::vmrglb(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int h = 0; h < 8; h++) {
            vd.u8[15 - h*2 + 0] = va.u8[7 - h];
            vd.u8[15 - h*2 - 1] = vb.u8[7 - h];
     }
    });
}

void Translator::vmrglh(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int w = 0; w < 4; w++) {
            vd.u16[7 - w*2 - 0] = va.u16[3 - w];
            vd.u16[7 - w*2 - 1] = vb.u16[3 - w];
        }
    });
}

void Translator::vmrglw(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int d = 0; d < 2; d++) {
            vd.u32[3 - d*2 - 0] = va.u32[1 - d];
            vd.u32[3 - d*2 - 1] = vb.u32[1 - d];
        }
    });
}

void Translator::vmsummbm(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        const auto& vc = state.v[o.vc];
        for (int w = 0; w < 4; w++) {
            S32 result = 0;
            for (int b = 0; b < 4; b++) {
                result += va.s8[w*4 + b] * vb.u8[w*4 + b];
            }
            result += vc.s32[w];
            vd.s32[w] = result;
        }
    });
}

void Translator::vmsumshm(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        const auto& vc = state.v[o.vc];
        for (int w = 0; w < 4; w++) {
            S32 result = 0;
            for (int h = 0; h < 2; h++) {
                result += va.s16[w*2 + h] * vb.s16[w*2 + h];
            }
            result += vc.s32[w];
            vd.s32[w] = result;
        }
    });
}

void Translator::vmsumshs(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmsumubm(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        const auto& vc = state.v[o.vc];
        for (int w = 0; w < 4; w++) {
            U32 result = 0;
            for (int b = 0; b < 4; b++) {
                result += va.u8[w*4 + b] * vb.u8[w*4 + b];
            }
            result += vc.u32[w];
            vd.u32[w] = result;
        }
    });
}

void Translator::vmsumuhm(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        const auto& vc = state.v[o.vc];
        for (int w = 0; w < 4; w++) {
            U32 result = 0;
            for (int h = 0; h < 2; h++) {
                result += va.u16[w*2 + h] * vb.u16[w*2 + h];
            }
            result += vc.u32[w];
            vd.u32[w] = result;
        }
    });
}

void Translator::vmsumuhs(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmulesb(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int h = 0; h < 8; h++) {
            vd.s16[h] = (S16)va.s8[h*2 + 1] * (S16)vb.s8[h*2 + 1];
        }
    });
}

void Translator::vmulesh(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int w = 0; w < 4; w++) {
            vd.s32[w] = (S32)va.s16[w*2 + 1] * (S32)vb.s16[w*2 + 1];
        }
    });
}

void Translator::vmuleub(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int h = 0; h < 8; h++) {
            vd.u16[h] = (U16)va.u8[h*2 + 1] * (U16)vb.u8[h*2 + 1];
        }
    });
}

void Translator::vmuleuh(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int w = 0; w < 4; w++) {
            vd.u32[w] = (U32)va.u16[w*2 + 1] * (U32)vb.u16[w*2 + 1];
        }
    });
}

void Translator::vmulosb(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int h = 0; h < 8; h++) {
            vd.s16[h] = (S16)va.s8[h*2] * (S16)vb.s8[h*2];
        }
    });
}

void Translator::vmulosh(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int w = 0; w < 4; w++) {
            vd.s32[w] = (S32)va.s16[w*2] * (S32)vb.s16[w*2];
        }
    });
}

void Translator::vmuloub(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int h = 0; h < 8; h++) {
            vd.u16[h] = (U16)va.u8[h*2] * (U16)vb.u8[h*2];
        }
    });
}

void Translator::vmulouh(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (int w = 0; w < 4; w++) {
            vd.u32[w] = (U32)va.u16[w*2] * (U32)vb.u16[w*2];
        }
    });
}

void Translator::vnmsubfp(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vc = getVR(code.vc);
    Value* vb = getVR(code.vb);
    Value* vd;

    // NOTE: vb-(va*vc) = -((va*vc)-vb)
    vd = builder.createVMul(va, vc, COMPONENT_F32);
    vd = builder.createVSub(vb, vd, COMPONENT_F32);

    setVR(code.vd, vd);
}

void Translator::vnor(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createOr(va, vb);
    vd = builder.createNot(vd);

    setVR(code.vd, vd);
}

void Translator::vor(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createOr(va, vb);

    setVR(code.vd, vd);
}

void Translator::vperm(Instruction code)
{
    INTERPRET({
        V128 vconcat[2];
        vconcat[0] = state.v[o.vb];
        vconcat[1] = state.v[o.va];
        const U08* buffer = reinterpret_cast<U08*>(vconcat);
        for (int b = 0; b < 16; b++) {
            U08 index = state.v[o.vc].u8[b] & 0x1F;
            state.v[o.vd].u8[b] = buffer[0x1F - index];
        }
    });
}

void Translator::vpkpx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vpkshss(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vpkshus(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vpkswss(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vpkswus(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        V128 va = state.v[o.va];
        V128 vb = state.v[o.vb];
        for (Size i = 0; i < 4; i++) {
            S32 result = va.s32[i];
            if (result > 0xFFFF) {
                result = 0xFFFF;
            } else if (result < 0) {
                result = 0;
            }
            vd.u16[i + 4] = result;
            result = vb.s32[i];

            if (result > 0xFFFF) {
                result = 0xFFFF;
            } else if (result < 0) {
                result = 0;
            }
            vd.u16[i] = result;
        }
    });
}

void Translator::vpkuhum(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        V128 va = state.v[o.va];
        V128 vb = state.v[o.vb];
        for (Size i = 0; i < 8; i++) {
            vd.u8[i + 8] = va.u8[i*2];
            vd.u8[i + 0] = vb.u8[i*2];
        }
    });
}

void Translator::vpkuhus(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        V128 va = state.v[o.va];
        V128 vb = state.v[o.vb];
        for (Size i = 0; i < 8; i++) {
            U16 result = va.u16[i];
            if (result > 0xFF) {
                result = 0xFF;
            }
            vd.u8[i + 8] = (U08)result;
            result = vb.u16[i];
            if (result > 0xFF) {
                result = 0xFF;
            }
            vd.u8[i] = (U08)result;
        }
    });
}

void Translator::vpkuwum(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        V128 va = state.v[o.va];
        V128 vb = state.v[o.vb];
        for (Size i = 0; i < 4; i++) {
            vd.u16[i + 4] = va.u16[i * 2];
            vd.u16[i + 0] = vb.u16[i * 2];
        }
    });
}

void Translator::vpkuwus(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        V128 va = state.v[o.va];
        V128 vb = state.v[o.vb];
        for (Size i = 0; i < 4; i++) {
            U32 result = va.u32[i];
            if (result > 0xFFFF) {
                result = 0xFFFF;
            }
            vd.u16[i + 4] = result;
            result = vb.u32[i];
            if (result > 0xFFFF) {
                result = 0xFFFF;
            }
            vd.u16[i] = result;
        }
    });
}

void Translator::vrefp(Instruction code)
{
    INTERPRET({
    });
}

void Translator::vrfim(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& vb = state.v[o.vb];
        for (Size i = 0; i < 4; i++) {
            vd.f32[i] = std::floor(vb.f32[i]);
        }
    });
}

void Translator::vrfin(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& vb = state.v[o.vb];
        for (Size i = 0; i < 4; i++) {
            vd.f32[i] = std::nearbyint(vb.f32[i]);
        }
    });
}

void Translator::vrfip(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& vb = state.v[o.vb];
        for (Size i = 0; i < 4; i++) {
            vd.f32[i] = std::ceil(vb.f32[i]);
        }
    });
}

void Translator::vrfiz(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& vb = state.v[o.vb];
        for (Size i = 0; i < 4; i++) {
            vd.f32[i] = std::truncf(vb.f32[i]);
        }
    });
}

void Translator::vrlb(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (Size i = 0; i < 16; i++) {
            vd.u8[i] = rotl8(va.u8[i], vb.u8[i] & 0x7);
        }
    });
}

void Translator::vrlh(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (Size i = 0; i < 8; i++) {
            vd.u16[i] = rotl16(va.u16[i], vb.u8[i*2] & 0xF);
        }
    });
}

void Translator::vrlw(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (Size i = 0; i < 4; i++) {
            vd.u32[i] = rotl32(va.u32[i], vb.u8[i*4] & 0x1F);
        }
    });
}

void Translator::vrsqrtefp(Instruction code)
{
    INTERPRET({
    });
}

void Translator::vsel(Instruction code)
{
    Value* va = getGPR(code.va);
    Value* vb = getGPR(code.vb);
    Value* vc = getGPR(code.vc);
    Value* vd;

    Value* selA = builder.createAnd(va, builder.createNot(vc));
    Value* selB = builder.createAnd(vb, vc);
    vd = builder.createOr(selA, selB);

    setGPR(code.vd, vd);
}

void Translator::vsl(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        V128 va = state.v[o.va];
        U08 sh = state.v[o.vb].u8[0] & 0x7;
        vd.u8[0] = va.u8[0] << sh;
        for (Size i = 1; i < 16; i++) {
            vd.u8[i] = (va.u8[i] << sh) | (va.u8[i - 1] >> (8 - sh));
        }
    });
}

void Translator::vslb(Instruction code)
{
    INTERPRET({
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (Size i = 0; i < 16; i++) {
            state.v[o.vd].u8[i] = va.u8[i] << (vb.u8[i] & 0x7);
        }
    });
}

void Translator::vsldoi(Instruction code)
{
    INTERPRET({
        V128 vconcat[2];
        vconcat[0] = state.v[o.vb];
        vconcat[1] = state.v[o.va];
        const U08* buffer = reinterpret_cast<U08*>(vconcat);
        for (Size i = 0; i < 16; i++) {
            state.v[o.vd].u8[15 - i] = buffer[31 - (i + o.vshb)];
        }
    });
}

void Translator::vslh(Instruction code)
{
    INTERPRET({
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (Size i = 0; i < 8; i++) {
            state.v[o.vd].u16[i] = va.u16[i] << (vb.u16[i] & 0xF);
        }
    });
}

void Translator::vslo(Instruction code)
{
    INTERPRET({
        V128 vd = V128{};
        V128 va = state.v[o.va];
        U08 shb = (state.v[o.vb].u8[0] >> 3) & 0xF;
        for (Size i = 0; i < 16 - shb; i++) {
            vd.u8[15 - i] = va.u8[15 - (i + shb)];
        }
        state.v[o.vd] = vd;
    });
}

void Translator::vslw(Instruction code)
{
    INTERPRET({
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (Size i = 0; i < 4; i++) {
            state.v[o.vd].u32[i] = va.u32[i] << (vb.u32[i] & 0x1F);
        }
    });
}

void Translator::vspltb(Instruction code)
{
    INTERPRET({
        const U08 b = state.v[o.vb].u8[15 - o.vuimm];
        for (Size i = 0; i < 16; i++) {
            state.v[o.vd].u8[i] = b;
        }
    });
}

void Translator::vsplth(Instruction code)
{
    INTERPRET({
        const U16 h = state.v[o.vb].u16[7 - o.vuimm];
        for (Size i = 0; i < 8; i++) {
            state.v[o.vd].u16[i] = h;
        }
    });
}

void Translator::vspltisb(Instruction code)
{
    INTERPRET({
        for (Size i = 0; i < 16; i++) {
            state.v[o.vd].s8[i] = o.vsimm;
        }
    });
}

void Translator::vspltish(Instruction code)
{
    INTERPRET({
        for (Size i = 0; i < 8; i++) {
            state.v[o.vd].s16[i] = o.vsimm;
        }
    });
}

void Translator::vspltisw(Instruction code)
{
    INTERPRET({
        for (Size i = 0; i < 4; i++) {
            state.v[o.vd].s32[i] = o.vsimm;
        }
    });
}

void Translator::vspltw(Instruction code)
{
    INTERPRET({
        U32 w = state.v[o.vb].u32[3 - o.vuimm];
        for (Size i = 0; i < 4; i++) {
            state.v[o.vd].u32[i] = w;
        }
    });
}

void Translator::vsr(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        V128 va = state.v[o.va];
        U08 sh = state.v[o.vb].u8[0] & 0x7;
        vd.u8[15] = va.u8[15] >> sh;
        for (Size i = 0; i < 14; i++) {
            vd.u8[i] = (va.u8[i] >> sh) | (va.u8[i + 1] << (8 - sh));
        }
    });
}

void Translator::vsrab(Instruction code)
{
    INTERPRET({
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (Size i = 0; i < 16; i++) {
            state.v[o.vd].s8[i] = va.s8[i] >> (vb.u8[i] & 0x7);
        }
    });
}

void Translator::vsrah(Instruction code)
{
    INTERPRET({
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (Size i = 0; i < 8; i++) {
            state.v[o.vd].s16[i] = va.s16[i] >> (vb.u16[i] & 0xF);
        }
    });
}

void Translator::vsraw(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];

        for (Size i = 0; i < 4; i++) {
            vd.s32[i] = va.s32[i] >> (vb.u32[i] & 0x1F);
        }
    });
}

void Translator::vsrb(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        
        for (Size i = 0; i < 16; i++) {
            vd.u8[i] = va.u8[i] >> (vb.u8[i] & 0x7);
        }
    });
}

void Translator::vsrh(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];

        for (Size i = 0; i < 8; i++) {
            vd.u16[i] = va.u16[i] >> (vb.u16[i] & 0xF);
        }
    });
}

void Translator::vsro(Instruction code)
{
    INTERPRET({
        V128 vd = V128{};
        V128 va = state.v[o.va];
        U08 shb = (state.v[o.vb].u8[0] >> 3) & 0xF;
        for (Size i = 0; i < 16 - shb; i++) {
            vd.u8[i] = va.u8[i + shb];
        }
        state.v[o.vd] = vd;
    });
}

void Translator::vsrw(Instruction code)
{
    INTERPRET({
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];
        for (Size i = 0; i < 4; i++) {
            state.v[o.vd].u32[i] = va.u32[i] >> (vb.u32[i] & 0x1F);
        }
    });
}

void Translator::vsubcuw(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsubfp(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVSub(va, vb, COMPONENT_F32);

    setVR(code.vd, vd);
}

void Translator::vsubsbs(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsubshs(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsubsws(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsububm(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVSub(va, vb, COMPONENT_I8 | ARITHMETIC_UNSIGNED);

    setVR(code.vd, vd);
}

void Translator::vsububs(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsubuhm(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVSub(va, vb, COMPONENT_I16 | ARITHMETIC_UNSIGNED);

    setVR(code.vd, vd);
}

void Translator::vsubuhs(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsubuwm(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVSub(va, vb, COMPONENT_I32 | ARITHMETIC_UNSIGNED);

    setVR(code.vd, vd);
}

void Translator::vsubuws(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];

        for (Size i = 0; i < 4; i++) {
            S64 result = (S64)va.u32[i] - (S64)vb.u32[i];
            if (result < 0) {
                vd.u32[i] = 0;
            } else {
                vd.u32[i] = (U32)result;
            }
        }
    });
}

void Translator::vsum2sws(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];

        vd = V128{};
        for (Size i = 0; i < 2; i++) {
            S64 sum = 0;
            sum += va.s32[i*2 + 0];
            sum += va.s32[i*2 + 1];
            sum += vb.s32[i*2 + 0];
            if (sum > 0x7FFFFFFF) {
                vd.s32[i*2] = 0x7FFFFFFF;
            } else if (sum < 0x80000000) {
                vd.s32[i*2] = 0x80000000;
            } else {
                vd.s32[i*2] = (S32)sum;
            }
        }
    });
}

void Translator::vsum4sbs(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];

        for (Size i = 0; i < 4; i++) {
            S64 sum = vb.s32[i];
            for (Size j = 0; j < 4; j++) {
                sum += va.s8[i*4 + j];
            }
            if (sum > 0x7FFFFFFF) {
                vd.s32[i] = 0x7FFFFFFF;
            } else if (sum < 0x80000000) {
                vd.s32[i] = 0x80000000;
            } else {
                vd.s32[i] = (S32)sum;
            }
        }
    });
}

void Translator::vsum4shs(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];

        for (Size i = 0; i < 4; i++) {
            S64 sum = vb.s32[i];
            for (Size j = 0; j < 2; j++) {
                sum += va.s16[i*2 + j];
            }
            if (sum > 0x7FFFFFFF) {
                vd.s32[i] = 0x7FFFFFFF;
            } else if (sum < 0x80000000) {
                vd.s32[i] = 0x80000000;
            } else {
                vd.s32[i] = (S32)sum;
            }
        }
    });
}

void Translator::vsum4ubs(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];

        for (Size i = 0; i < 4; i++) {
            U64 sum = vb.u32[i];
            for (Size j = 0; j < 4; j++) {
                sum += va.u8[i*4 + j];
            }
            if (sum > 0xFFFFFFFF) {
                vd.u32[i] = 0xFFFFFFFF;
            } else {
                vd.u32[i] = (U32)sum;
            }
        }
    });
}

void Translator::vsumsws(Instruction code)
{
    INTERPRET({
        const auto& va = state.v[o.va];
        const auto& vb = state.v[o.vb];

        S64 sum = vb.s32[0];
        for (Size i = 0; i < 4; i++) {
            sum += va.s32[i];
        }
        state.v[o.vd] = V128{};
        if (sum > 0x7FFFFFFF) {
            state.v[o.vd].s32[0] = 0x7FFFFFFF;
        } else if (sum < 0x80000000) {
            state.v[o.vd].s32[0] = 0x80000000;
        } else {
            state.v[o.vd].s32[0] = (S32)sum;
        }
    });
}

void Translator::vupkhpx(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        V128 vb = state.v[o.vb];
        for (Size i = 0; i < 4; i++) {
            vd.s8[i * 4 + 3] = (vb.s8[8 + i*2 + 1] >> 7);
            vd.u8[i * 4 + 2] = (vb.u8[8 + i*2 + 1] >> 2) & 0x1F;
            vd.u8[i * 4 + 1] = ((vb.u8[8 + i*2 + 1] & 0x3) << 3) | ((vb.u8[8 + i*2 + 0] >> 5) & 0x7);
            vd.u8[i * 4 + 0] = (vb.u8[8 + i*2 + 0] & 0x1F);
        }
    });
}

void Translator::vupkhsb(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        V128 vb = state.v[o.vb];
        for (Size i = 0; i < 8; i++) {
            vd.s16[i] = vb.s8[8 + i];
        }
    });
}

void Translator::vupkhsh(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        V128 vb = state.v[o.vb];
        for (Size i = 0; i < 4; i++) {
            vd.s32[i] = vb.s16[4 + i];
        }
    });
}

void Translator::vupklpx(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        V128 vb = state.v[o.vb];
        for (Size i = 0; i < 4; i++) {
            vd.s8[i*4 + 3] = (vb.s8[i*2 + 1] >> 7);
            vd.u8[i*4 + 2] = (vb.u8[i*2 + 1] >> 2) & 0x1F;
            vd.u8[i*4 + 1] = ((vb.u8[i*2 + 1] & 0x3) << 3) | ((vb.u8[i*2 + 0] >> 5) & 0x7);
            vd.u8[i*4 + 0] = (vb.u8[i*2 + 0] & 0x1F);
        }
    });
}

void Translator::vupklsb(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        V128 vb = state.v[o.vb];
        for (Size i = 0; i < 8; i++) {
            vd.s16[i] = vb.s8[i];
        }
    });
}

void Translator::vupklsh(Instruction code)
{
    INTERPRET({
        auto& vd = state.v[o.vd];
        V128 vb = state.v[o.vb];
        for (Size i = 0; i < 4; i++) {
            vd.s32[i] = vb.s16[i];
        }
    });
}

void Translator::vxor(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createXor(va, vb);

    setVR(code.vd, vd);
}

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
