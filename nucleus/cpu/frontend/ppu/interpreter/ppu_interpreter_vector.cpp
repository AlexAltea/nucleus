/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_interpreter.h"
#include "nucleus/emulator.h"

namespace cpu {
namespace ppu {

// PowerPC Rotation-related functions
inline u64 rotl64(const u64 x, const u8 n) { return (x << n) | (x >> (64 - n)); }
inline u64 rotl32(const u32 x, const u8 n) { return rotl64((u64)x | ((u64)x << 32), n); }
inline u16 rotl16(const u16 x, const u8 n) { return (x << n) | (x >> (16 - n)); }
inline u8 rotl8(const u8 x, const u8 n) { return (x << n) | (x >> (8 - n)); }

float CheckVSCR_NJ(State& state, const f32 v)
{
    if (!state.vscr.NJ) {
        return v;
    }
    if (std::fpclassify(v) == FP_SUBNORMAL) {
        return std::signbit(v) ? -0.0f : 0.0f;
    }
    return v;
}

/**
 * PPC64 Vector/SIMD Instructions (aka AltiVec):
 *  - Vector UISA Instructions (Section: 4.2.x)
 *  - Vector VEA Instructions (Section: 4.3.x)
 */

void Interpreter::dss(Instruction code)
{
    // TODO: _mm_fence();
}

void Interpreter::dst(Instruction code)
{
    // TODO: _mm_fence();
}

void Interpreter::dstst(Instruction code)
{
    // TODO: _mm_fence();
}

void Interpreter::lvebx(Instruction code)
{
    const u32 addr = (code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb]) & ~0xfULL;
    state.vr[code.vd]._u128 = nucleus.memory.read128(addr);
}

void Interpreter::lvehx(Instruction code)
{
    const u32 addr = (code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb]) & ~0xfULL;
    state.vr[code.vd]._u128 = nucleus.memory.read128(addr);
}

void Interpreter::lvewx(Instruction code)
{
    const u32 addr = (code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb]) & ~0xfULL;
    state.vr[code.vd]._u128 = nucleus.memory.read128(addr);
}

void Interpreter::lvlx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.readLeft(state.vr[code.vd]._u8 + eb, addr, 16 - eb);
}

void Interpreter::lvlxl(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.readLeft(state.vr[code.vd]._u8 + eb, addr, 16 - eb);
}

void Interpreter::lvrx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.readRight(state.vr[code.vd]._u8, addr & ~0xf, eb);
}

void Interpreter::lvrxl(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.readRight(state.vr[code.vd]._u8, addr & ~0xf, eb);
}

void Interpreter::lvsl(Instruction code)
{
    static const u64 lvsl_values[0x10][2] = {
        {0x08090A0B0C0D0E0F, 0x0001020304050607},
        {0x090A0B0C0D0E0F10, 0x0102030405060708},
        {0x0A0B0C0D0E0F1011, 0x0203040506070809},
        {0x0B0C0D0E0F101112, 0x030405060708090A},
        {0x0C0D0E0F10111213, 0x0405060708090A0B},
        {0x0D0E0F1011121314, 0x05060708090A0B0C},
        {0x0E0F101112131415, 0x060708090A0B0C0D},
        {0x0F10111213141516, 0x0708090A0B0C0D0E},
        {0x1011121314151617, 0x08090A0B0C0D0E0F},
        {0x1112131415161718, 0x090A0B0C0D0E0F10},
        {0x1213141516171819, 0x0A0B0C0D0E0F1011},
        {0x131415161718191A, 0x0B0C0D0E0F101112},
        {0x1415161718191A1B, 0x0C0D0E0F10111213},
        {0x15161718191A1B1C, 0x0D0E0F1011121314},
        {0x161718191A1B1C1D, 0x0E0F101112131415},
        {0x1718191A1B1C1D1E, 0x0F10111213141516},
    };
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    state.vr[code.vd]._u64[0] = lvsl_values[addr & 0xf][0];
    state.vr[code.vd]._u64[1] = lvsl_values[addr & 0xf][1];
}

void Interpreter::lvsr(Instruction code)
{
    static const u64 lvsr_values[0x10][2] = {
        {0x18191A1B1C1D1E1F, 0x1011121314151617},
        {0x1718191A1B1C1D1E, 0x0F10111213141516},
        {0x161718191A1B1C1D, 0x0E0F101112131415},
        {0x15161718191A1B1C, 0x0D0E0F1011121314},
        {0x1415161718191A1B, 0x0C0D0E0F10111213},
        {0x131415161718191A, 0x0B0C0D0E0F101112},
        {0x1213141516171819, 0x0A0B0C0D0E0F1011},
        {0x1112131415161718, 0x090A0B0C0D0E0F10},
        {0x1011121314151617, 0x08090A0B0C0D0E0F},
        {0x0F10111213141516, 0x0708090A0B0C0D0E},
        {0x0E0F101112131415, 0x060708090A0B0C0D},
        {0x0D0E0F1011121314, 0x05060708090A0B0C},
        {0x0C0D0E0F10111213, 0x0405060708090A0B},
        {0x0B0C0D0E0F101112, 0x030405060708090A},
        {0x0A0B0C0D0E0F1011, 0x0203040506070809},
        {0x090A0B0C0D0E0F10, 0x0102030405060708},
    };
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    state.vr[code.vd]._u64[0] = lvsr_values[addr & 0xf][0];
    state.vr[code.vd]._u64[1] = lvsr_values[addr & 0xf][1];
}

void Interpreter::lvx(Instruction code)
{
    const u32 addr = (code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb]) & ~0xfULL;
    state.vr[code.vd]._u128 = nucleus.memory.read128(addr);
}

void Interpreter::lvxl(Instruction code)
{
    const u32 addr = (code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb]) & ~0xfULL;
    state.vr[code.vd]._u128 = nucleus.memory.read128(addr);
}

void Interpreter::mfvscr(Instruction code)
{
    state.vr[code.vd].clear();
    state.vr[code.vd]._u32[0] = state.vscr.VSCR;
}

void Interpreter::mtvscr(Instruction code)
{
    state.vscr.VSCR = state.vr[code.vb]._u32[0];
}

void Interpreter::stvebx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.write8(addr, state.vr[code.vs]._u8[15 - eb]);
}

void Interpreter::stvehx(Instruction code)
{
    const u32 addr = (code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb]) & ~1ULL;
    const u8 eb = (addr & 0xf) >> 1;
    nucleus.memory.write16(addr, state.vr[code.vs]._u16[7 - eb]);
}

void Interpreter::stvewx(Instruction code)
{
    const u32 addr = (code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb]) & ~3ULL;
    const u8 eb = (addr & 0xf) >> 2;
    nucleus.memory.write32(addr, state.vr[code.vs]._u32[3 - eb]);
}

void Interpreter::stvlx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.writeLeft(addr, state.vr[code.vs]._u8 + eb, 16 - eb);
}

void Interpreter::stvlxl(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.writeLeft(addr, state.vr[code.vs]._u8 + eb, 16 - eb);
}

void Interpreter::stvrx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.writeRight(addr - eb, state.vr[code.vs]._u8, eb);
}

void Interpreter::stvrxl(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.writeRight(addr - eb, state.vr[code.vs]._u8, eb);
}

void Interpreter::stvx(Instruction code)
{
    const u32 addr = (code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb]) & ~0xfULL;
    nucleus.memory.write128(addr, state.vr[code.vs]._u128);
}

void Interpreter::stvxl(Instruction code)
{
    const u32 addr = (code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb]) & ~0xfULL;
    nucleus.memory.write128(addr, state.vr[code.vs]._u128);
}

void Interpreter::vaddcuw(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = ~state.vr[code.va]._u32[w] < state.vr[code.vb]._u32[w];
    }
}

void Interpreter::vaddfp(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._f32[w] = state.vr[code.va]._f32[w] + state.vr[code.vb]._f32[w];
    }
}

void Interpreter::vaddsbs(Instruction code)
{
    for (u32 b=0; b<16; ++b) {
        const s16 result = (s16)state.vr[code.va]._s8[b] + (s16)state.vr[code.vb]._s8[b];
        if (result > 0x7F) {
            state.vr[code.vd]._s8[b] = 0x7F;
            state.vscr.SAT = 1;
        }
        else if (result < -0x80) {
            state.vr[code.vd]._s8[b] = -0x80;
            state.vscr.SAT = 1;
        }
        else {
            state.vr[code.vd]._s8[b] = result;
        }
    }
}

void Interpreter::vaddshs(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        const s32 result = (s32)state.vr[code.va]._s16[h] + (s32)state.vr[code.vb]._s16[h];
        if (result > 0x7FFF) {
            state.vr[code.vd]._s16[h] = 0x7FFF;
            state.vscr.SAT = 1;
        }
        else if (result < -0x8000) {
            state.vr[code.vd]._s16[h] = -0x8000;
            state.vscr.SAT = 1;
        }
        else {
            state.vr[code.vd]._s16[h] = result;
        }
    }
}

void Interpreter::vaddsws(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        const s64 result = (s64)state.vr[code.va]._s32[w] + (s64)state.vr[code.vb]._s32[w];
        if (result > 0x7FFFFFFF) {
            state.vr[code.vd]._s32[w] = 0x7FFFFFFF;
            state.vscr.SAT = 1;
        }
        else if (result < (s32)0x80000000) {
            state.vr[code.vd]._s32[w] = 0x80000000;
            state.vscr.SAT = 1;
        }
        else {
            state.vr[code.vd]._s32[w] = result;
        }
    }
}

void Interpreter::vaddubm(Instruction code)
{
    for (int b = 0; b < 16; b++) {
        state.vr[code.vd]._u8[b] = state.vr[code.va]._u8[b] + state.vr[code.vb]._u8[b];
    }
}

void Interpreter::vaddubs(Instruction code)
{
    for (int b = 0; b < 16; b++) {
        const u16 result = (u16)state.vr[code.va]._u8[b] + (u16)state.vr[code.vb]._u8[b];
        if (result > 0xFF) {
            state.vr[code.vd]._u8[b] = 0xFF;
            state.vscr.SAT = 1;
        }
        else {
            state.vr[code.vd]._u8[b] = result;
        }
    }
}

void Interpreter::vadduhm(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._u16[h] = state.vr[code.va]._u16[h] + state.vr[code.vb]._u16[h];
    }
}

void Interpreter::vadduhs(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        const u32 result = (u32)state.vr[code.va]._u16[h] + (u32)state.vr[code.vb]._u16[h];
        if (result > 0xFFFF) {
            state.vr[code.vd]._u16[h] = 0xFFFF;
            state.vscr.SAT = 1;
        }
        else {
            state.vr[code.vd]._u16[h] = result;
        }
    }
}

void Interpreter::vadduwm(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = state.vr[code.va]._u32[w] + state.vr[code.vb]._u32[w];
    }
}

void Interpreter::vadduws(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        const u64 result = (u64)state.vr[code.va]._u32[w] + (u64)state.vr[code.vb]._u32[w];
        if (result > 0xFFFFFFFF) {
            state.vr[code.vd]._u32[w] = 0xFFFFFFFF;
            state.vscr.SAT = 1;
        }
        else {
            state.vr[code.vd]._u32[w] = result;
        }
    }
}

void Interpreter::vand(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = state.vr[code.va]._u32[w] & state.vr[code.vb]._u32[w];
    }
}

void Interpreter::vandc(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = state.vr[code.va]._u32[w] & (~state.vr[code.vb]._u32[w]);
    }
}

void Interpreter::vavgsb(Instruction code)
{
    for (int b = 0; b < 16; b++) {
        state.vr[code.vd]._s8[b] = (state.vr[code.va]._s8[b] + state.vr[code.vb]._s8[b] + 1) >> 1;
    }
}

void Interpreter::vavgsh(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._s16[h] = (state.vr[code.va]._s16[h] + state.vr[code.vb]._s16[h] + 1) >> 1;
    }
}

void Interpreter::vavgsw(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._s32[w] = ((s64)state.vr[code.va]._s32[w] + (s64)state.vr[code.vb]._s32[w] + 1) >> 1;
    }
}

void Interpreter::vavgub(Instruction code)
{
    for (int b = 0; b < 16; b++) {
        state.vr[code.vd]._u8[b] = (state.vr[code.va]._u8[b] + state.vr[code.vb]._u8[b] + 1) >> 1;
    }
}

void Interpreter::vavguh(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._u16[h] = (state.vr[code.va]._u16[h] + state.vr[code.vb]._u16[h] + 1) >> 1;
    }
}

void Interpreter::vavguw(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = ((u64)state.vr[code.va]._u32[w] + (u64)state.vr[code.vb]._u32[w] + 1) >> 1;
    }
}

void Interpreter::vcfsx(Instruction code)
{
    const u32 scale = 1 << code.vuimm;
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._f32[w] = ((f32)state.vr[code.vb]._s32[w]) / scale;
    }
}

void Interpreter::vcfux(Instruction code)
{
    const u32 scale = 1 << code.vuimm;
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._f32[w] = ((f32)state.vr[code.vb]._u32[w]) / scale;
    }
}

void Interpreter::vcmpbfp(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        u32 mask = 0;
        const f32 A = CheckVSCR_NJ(state, state.vr[code.va]._f32[w]);
        const f32 B = CheckVSCR_NJ(state, state.vr[code.vb]._f32[w]);

        if (A >  B) mask |= 1 << 31;
        if (A < -B) mask |= 1 << 30;

        state.vr[code.vd]._u32[w] = mask;
    }
}

void Interpreter::vcmpbfp_(Instruction code)
{
    bool allInBounds = true;
    for (int w = 0; w < 4; w++) {
        u32 mask = 0;
        const f32 A = CheckVSCR_NJ(state, state.vr[code.va]._f32[w]);
        const f32 B = CheckVSCR_NJ(state, state.vr[code.vb]._f32[w]);

        if (A >  B) mask |= 1 << 31;
        if (A < -B) mask |= 1 << 30;

        state.vr[code.vd]._u32[w] = mask;
        if (mask) {
            allInBounds = false;
        }
    }
    state.cr.setBit(6*4 + 2, allInBounds);
}

void Interpreter::vcmpeqfp(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = state.vr[code.va]._f32[w] == state.vr[code.vb]._f32[w] ? 0xFFFFFFFF : 0;
    }
}

void Interpreter::vcmpeqfp_(Instruction code)
{
    int all_equal = 0x8;
    int none_equal = 0x2;

    for (int w = 0; w < 4; w++) {
        if (state.vr[code.va]._f32[w] == state.vr[code.vb]._f32[w]) {
            state.vr[code.vd]._u32[w] = 0xFFFFFFFF;
            none_equal = 0;
        }
        else {
            state.vr[code.vd]._u32[w] = 0;
            all_equal = 0;
        }
    }

    state.cr.setField(6, all_equal | none_equal);
}

void Interpreter::vcmpequb(Instruction code)
{
    for (int b = 0; b < 16; b++) {
        state.vr[code.vd]._u8[b] = state.vr[code.va]._u8[b] == state.vr[code.vb]._u8[b] ? 0xFF : 0;
    }
}

void Interpreter::vcmpequb_(Instruction code)
{
    int all_equal = 0x8;
    int none_equal = 0x2;

    for (int b = 0; b < 16; b++) {
        if (state.vr[code.va]._u8[b] == state.vr[code.vb]._u8[b]) {
            state.vr[code.vd]._u8[b] = 0xFF;
            none_equal = 0;
        }
        else {
            state.vr[code.vd]._u8[b] = 0;
            all_equal = 0;
        }
    }

    state.cr.setField(6, all_equal | none_equal);
}

void Interpreter::vcmpequh(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._u16[h] = state.vr[code.va]._u16[h] == state.vr[code.vb]._u16[h] ? 0xFFFF : 0;
    }
}

void Interpreter::vcmpequh_(Instruction code)
{
    int all_equal = 0x8;
    int none_equal = 0x2;

    for (int h = 0; h < 8; h++) {
        if (state.vr[code.va]._u16[h] == state.vr[code.vb]._u16[h]) {
            state.vr[code.vd]._u16[h] = 0xFFFF;
            none_equal = 0;
        }
        else {
            state.vr[code.vd]._u16[h] = 0;
            all_equal = 0;
        }
    }
    state.cr.setField(6, all_equal | none_equal);
}

void Interpreter::vcmpequw(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = state.vr[code.va]._u32[w] == state.vr[code.vb]._u32[w] ? 0xFFFFFFFF : 0;
    }
}

void Interpreter::vcmpequw_(Instruction code)
{
    int all_equal = 0x8;
    int none_equal = 0x2;

    for (int w = 0; w < 4; w++) {
        if (state.vr[code.va]._u32[w] == state.vr[code.vb]._u32[w]) {
            state.vr[code.vd]._u32[w] = 0xFFFFFFFF;
            none_equal = 0;
        }
        else {
            state.vr[code.vd]._u32[w] = 0;
            all_equal = 0;
        }
    }

    state.cr.setField(6, all_equal | none_equal);
}

void Interpreter::vcmpgefp(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = state.vr[code.va]._f32[w] >= state.vr[code.vb]._f32[w] ? 0xFFFFFFFF : 0;
    }
}

void Interpreter::vcmpgefp_(Instruction code)
{
    int all_ge = 0x8;
    int none_ge = 0x2;

    for (int w = 0; w < 4; w++) {
        if (state.vr[code.va]._f32[w] >= state.vr[code.vb]._f32[w]) {
            state.vr[code.vd]._u32[w] = 0xFFFFFFFF;
            none_ge = 0;
        }
        else {
            state.vr[code.vd]._u32[w] = 0;
            all_ge = 0;
        }
    }

    state.cr.setField(6, all_ge | none_ge);
}

void Interpreter::vcmpgtfp(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = state.vr[code.va]._f32[w] > state.vr[code.vb]._f32[w] ? 0xFFFFFFFF : 0;
    }
}

void Interpreter::vcmpgtfp_(Instruction code)
{
    int all_ge = 0x8;
    int none_ge = 0x2;

    for (int w = 0; w < 4; w++) {
        if (state.vr[code.va]._f32[w] > state.vr[code.vb]._f32[w]) {
            state.vr[code.vd]._u32[w] = 0xFFFFFFFF;
            none_ge = 0;
        }
        else {
            state.vr[code.vd]._u32[w] = 0;
            all_ge = 0;
        }
    }

    state.cr.setField(6, all_ge | none_ge);
}

void Interpreter::vcmpgtsb(Instruction code)
{
    for (int b = 0; b < 16; b++) {
        state.vr[code.vd]._u8[b] = state.vr[code.va]._s8[b] > state.vr[code.vb]._s8[b] ? 0xFF : 0;
    }
}

void Interpreter::vcmpgtsb_(Instruction code)
{
    int all_gt = 0x8;
    int none_gt = 0x2;

    for (int b = 0; b < 16; b++) {
        if (state.vr[code.va]._s8[b] > state.vr[code.vb]._s8[b]) {
            state.vr[code.vd]._u8[b] = 0xFF;
            none_gt = 0;
        }
        else {
            state.vr[code.vd]._u8[b] = 0;
            all_gt = 0;
        }
    }

    state.cr.setField(6, all_gt | none_gt);
}

void Interpreter::vcmpgtsh(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._u16[h] = state.vr[code.va]._s16[h] > state.vr[code.vb]._s16[h] ? 0xFFFF : 0;
    }
}

void Interpreter::vcmpgtsh_(Instruction code)
{
    int all_gt = 0x8;
    int none_gt = 0x2;

    for (int h = 0; h < 8; h++) {
        if (state.vr[code.va]._s16[h] > state.vr[code.vb]._s16[h]) {
            state.vr[code.vd]._u16[h] = 0xFFFF;
            none_gt = 0;
        }
        else {
            state.vr[code.vd]._u16[h] = 0;
            all_gt = 0;
        }
    }

    state.cr.setField(6, all_gt | none_gt);
}

void Interpreter::vcmpgtsw(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = state.vr[code.va]._s32[w] > state.vr[code.vb]._s32[w] ? 0xFFFFFFFF : 0;
    }
}

void Interpreter::vcmpgtsw_(Instruction code)
{
    int all_gt = 0x8;
    int none_gt = 0x2;

    for (int w = 0; w < 4; w++) {
        if (state.vr[code.va]._s32[w] > state.vr[code.vb]._s32[w]) {
            state.vr[code.vd]._u32[w] = 0xFFFFFFFF;
            none_gt = 0;
        }
        else {
            state.vr[code.vd]._u32[w] = 0;
            all_gt = 0;
        }
    }

    state.cr.setField(6, all_gt | none_gt);
}

void Interpreter::vcmpgtub(Instruction code)
{
    for (int b = 0; b < 16; b++) {
        state.vr[code.vd]._u8[b] = state.vr[code.va]._u8[b] > state.vr[code.vb]._u8[b] ? 0xFF : 0;
    }
}

void Interpreter::vcmpgtub_(Instruction code)
{
    int all_gt = 0x8;
    int none_gt = 0x2;

    for (int b = 0; b < 16; b++) {
        if (state.vr[code.va]._u8[b] > state.vr[code.vb]._u8[b]) {
            state.vr[code.vd]._u8[b] = 0xFF;
            none_gt = 0;
        }
        else {
            state.vr[code.vd]._u8[b] = 0;
            all_gt = 0;
        }
    }

    state.cr.setField(6, all_gt | none_gt);
}

void Interpreter::vcmpgtuh(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._u16[h] = state.vr[code.va]._u16[h] > state.vr[code.vb]._u16[h] ? 0xFFFF : 0;
    }
}

void Interpreter::vcmpgtuh_(Instruction code)
{
    int all_gt = 0x8;
    int none_gt = 0x2;

    for (int h = 0; h < 8; h++) {
        if (state.vr[code.va]._u16[h] > state.vr[code.vb]._u16[h]) {
            state.vr[code.vd]._u16[h] = 0xFFFF;
            none_gt = 0;
        }
        else {
            state.vr[code.vd]._u16[h] = 0;
            all_gt = 0;
        }
    }

    state.cr.setField(6, all_gt | none_gt);
}

void Interpreter::vcmpgtuw(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = state.vr[code.va]._u32[w] > state.vr[code.vb]._u32[w] ? 0xFFFFFFFF : 0;
    }
}

void Interpreter::vcmpgtuw_(Instruction code)
{
    int all_gt = 0x8;
    int none_gt = 0x2;

    for (int w = 0; w < 4; w++) {
        if (state.vr[code.va]._u32[w] > state.vr[code.vb]._u32[w]) {
            state.vr[code.vd]._u32[w] = 0xFFFFFFFF;
            none_gt = 0;
        }
        else {
            state.vr[code.vd]._u32[w] = 0;
            all_gt = 0;
        }
    }

    state.cr.setField(6, all_gt | none_gt);
}

void Interpreter::vctsxs(Instruction code)
{
    int nScale = 1 << code.vuimm;

    for (int w = 0; w < 4; w++) {
        f32 result = state.vr[code.vb]._f32[w] * nScale;

        if (result > 0x7FFFFFFF)
            state.vr[code.vd]._s32[w] = 0x7FFFFFFF;
        else if (result < -0x80000000LL)
            state.vr[code.vd]._s32[w] = -0x80000000LL;
        else {
            state.vr[code.vd]._s32[w] = (int)result;
        }
    }
}

void Interpreter::vctuxs(Instruction code)
{
    int nScale = 1 << code.vuimm;

    for (int w = 0; w < 4; w++) {
        s64 result = (s64)(state.vr[code.vb]._f32[w] * nScale);

        if (result > 0xFFFFFFFF)
            state.vr[code.vd]._u32[w] = 0xFFFFFFFF;
        else if (result < 0)
            state.vr[code.vd]._u32[w] = 0;
        else
            state.vr[code.vd]._u32[w] = (u32)result;
    }
}

void Interpreter::vexptefp(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._f32[w] = exp(state.vr[code.vb]._f32[w] * log(2.0f));
    }
}

void Interpreter::vlogefp(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._f32[w] = log(state.vr[code.vb]._f32[w]) / log(2.0f);
    }
}

void Interpreter::vmaddfp(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._f32[w] = state.vr[code.va]._f32[w] * state.vr[code.vc]._f32[w] + state.vr[code.vb]._f32[w];
    }
}

void Interpreter::vmaxfp(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._f32[w] = std::max(state.vr[code.va]._f32[w], state.vr[code.vb]._f32[w]);
    }
}

void Interpreter::vmaxsb(Instruction code)
{
    for (int b = 0; b < 16; b++)
        state.vr[code.vd]._s8[b] = std::max(state.vr[code.va]._s8[b], state.vr[code.vb]._s8[b]);
}

void Interpreter::vmaxsh(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._s16[h] = std::max(state.vr[code.va]._s16[h], state.vr[code.vb]._s16[h]);
    }
}

void Interpreter::vmaxsw(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._s32[w] = std::max(state.vr[code.va]._s32[w], state.vr[code.vb]._s32[w]);
    }
}

void Interpreter::vmaxub(Instruction code)
{
    for (int b = 0; b < 16; b++)
        state.vr[code.vd]._u8[b] = std::max(state.vr[code.va]._u8[b], state.vr[code.vb]._u8[b]);
}

void Interpreter::vmaxuh(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._u16[h] = std::max(state.vr[code.va]._u16[h], state.vr[code.vb]._u16[h]);
    }
}

void Interpreter::vmaxuw(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = std::max(state.vr[code.va]._u32[w], state.vr[code.vb]._u32[w]);
    }
}

void Interpreter::vmhaddshs(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        s32 result = (s32)state.vr[code.va]._s16[h] * (s32)state.vr[code.vb]._s16[h] + (s32)state.vr[code.vc]._s16[h];
        if (result > INT16_MAX) {
            state.vr[code.vd]._s16[h] = (s16)INT16_MAX;
            state.vscr.SAT = 1;
        }
        else if (result < INT16_MIN) {
            state.vr[code.vd]._s16[h] = (s16)INT16_MIN;
            state.vscr.SAT = 1;
        }
        else {
            state.vr[code.vd]._s16[h] = (s16)result;
        }
    }
}

void Interpreter::vmhraddshs(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        s32 result = (s32)state.vr[code.va]._s16[h] * (s32)state.vr[code.vb]._s16[h] + (s32)state.vr[code.vc]._s16[h] + 0x4000;

        if (result > INT16_MAX) {
            state.vr[code.vd]._s16[h] = (s16)INT16_MAX;
            state.vscr.SAT = 1;
        }
        else if (result < INT16_MIN) {
            state.vr[code.vd]._s16[h] = (s16)INT16_MIN;
            state.vscr.SAT = 1;
        }
        else {
            state.vr[code.vd]._s16[h] = (s16)result;
        }
    }
}

void Interpreter::vminfp(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._f32[w] = std::min(state.vr[code.va]._f32[w], state.vr[code.vb]._f32[w]);
    }
}

void Interpreter::vminsb(Instruction code)
{
    for (int b = 0; b < 16; b++) {
        state.vr[code.vd]._s8[b] = std::min(state.vr[code.va]._s8[b], state.vr[code.vb]._s8[b]);
    }
}

void Interpreter::vminsh(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._s16[h] = std::min(state.vr[code.va]._s16[h], state.vr[code.vb]._s16[h]);
    }
}

void Interpreter::vminsw(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._s32[w] = std::min(state.vr[code.va]._s32[w], state.vr[code.vb]._s32[w]);
    }
}

void Interpreter::vminub(Instruction code)
{
    for (int b = 0; b < 16; b++) {
        state.vr[code.vd]._u8[b] = std::min(state.vr[code.va]._u8[b], state.vr[code.vb]._u8[b]);
    }
}

void Interpreter::vminuh(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._u16[h] = std::min(state.vr[code.va]._u16[h], state.vr[code.vb]._u16[h]);
    }
}

void Interpreter::vminuw(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = std::min(state.vr[code.va]._u32[w], state.vr[code.vb]._u32[w]);
    }
}

void Interpreter::vmladduhm(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._u16[h] = state.vr[code.va]._u16[h] * state.vr[code.vb]._u16[h] + state.vr[code.vc]._u16[h];
    }
}

void Interpreter::vmrghb(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._u8[15 - h*2 + 0] = state.vr[code.va]._u8[15 - h];
        state.vr[code.vd]._u8[15 - h*2 - 1] = state.vr[code.vb]._u8[15 - h];
    }
}

void Interpreter::vmrghh(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u16[7 - w*2 + 0] = state.vr[code.va]._u16[7 - w];
        state.vr[code.vd]._u16[7 - w*2 - 1] = state.vr[code.vb]._u16[7 - w];
    }
}

void Interpreter::vmrghw(Instruction code)
{
    for (int d = 0; d < 2; d++) {
        state.vr[code.vd]._u32[3 - d*2 + 0] = state.vr[code.va]._u32[3 - d];
        state.vr[code.vd]._u32[3 - d*2 - 1] = state.vr[code.vb]._u32[3 - d];
    }
}

void Interpreter::vmrglb(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._u8[15 - h*2 + 0] = state.vr[code.va]._u8[7 - h];
        state.vr[code.vd]._u8[15 - h*2 - 1] = state.vr[code.vb]._u8[7 - h];
    }
}

void Interpreter::vmrglh(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u16[7 - w*2] = state.vr[code.va]._u16[3 - w];
        state.vr[code.vd]._u16[7 - w*2 - 1] = state.vr[code.vb]._u16[3 - w];
    }
}

void Interpreter::vmrglw(Instruction code)
{
    for (int d = 0; d < 2; d++) {
        state.vr[code.vd]._u32[3 - d*2 + 0] = state.vr[code.va]._u32[1 - d];
        state.vr[code.vd]._u32[3 - d*2 - 1] = state.vr[code.vb]._u32[1 - d];
    }
}

void Interpreter::vmsummbm(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        s32 result = 0;
        for (int b = 0; b < 4; b++) {
            result += state.vr[code.va]._s8[w*4 + b] * state.vr[code.vb]._u8[w*4 + b];
        }
        result += state.vr[code.vc]._s32[w];
        state.vr[code.vd]._s32[w] = result;
    }
}

void Interpreter::vmsumshm(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        s32 result = 0;
        for (int h = 0; h < 2; h++) {
            result += state.vr[code.va]._s16[w*2 + h] * state.vr[code.vb]._s16[w*2 + h];
        }
        result += state.vr[code.vc]._s32[w];
        state.vr[code.vd]._s32[w] = result;
    }
}

void Interpreter::vmsumshs(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        s64 result = 0;
        for (int h = 0; h < 2; h++) {
            result += state.vr[code.va]._s16[w*2 + h] * state.vr[code.vb]._s16[w*2 + h];
        }
        result += state.vr[code.vc]._s32[w];

        if (result > 0x7FFFFFFF) {
            state.vr[code.vd]._s32[w] = 0x7FFFFFFF;
            state.vscr.SAT = 1;
        }
        else if (result < -0x80000000LL) {
            state.vr[code.vd]._s32[w] = -0x80000000LL;
            state.vscr.SAT = 1;
        }
        else {
            state.vr[code.vd]._s32[w] = (s32)result;
        }
    }
}

void Interpreter::vmsumubm(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        u32 result = 0;
        for (int b = 0; b < 4; b++) {
            result += state.vr[code.va]._u8[w*4 + b] * state.vr[code.vb]._u8[w*4 + b];
        }
        result += state.vr[code.vc]._u32[w];
        state.vr[code.vd]._u32[w] = result;
    }
}

void Interpreter::vmsumuhm(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        u32 result = 0;

        for (int h = 0; h < 2; h++) {
            result += state.vr[code.va]._u16[w*2 + h] * state.vr[code.vb]._u16[w*2 + h];
        }

        result += state.vr[code.vc]._u32[w];
        state.vr[code.vd]._u32[w] = result;
    }
}

void Interpreter::vmsumuhs(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        u64 result = 0;
        for (int h = 0; h < 2; h++) {
            result += state.vr[code.va]._u16[w*2 + h] * state.vr[code.vb]._u16[w*2 + h];
        }
        result += state.vr[code.vc]._u32[w];

        if (result > 0xFFFFFFFF) {
            state.vr[code.vd]._u32[w] = 0xFFFFFFFF;
            state.vscr.SAT = 1;
        }
        else {
            state.vr[code.vd]._u32[w] = (u32)result;
        }
    }
}

void Interpreter::vmulesb(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._s16[h] = (s16)state.vr[code.va]._s8[h*2+1] * (s16)state.vr[code.vb]._s8[h*2+1];
    }
}

void Interpreter::vmulesh(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._s32[w] = (s32)state.vr[code.va]._s16[w*2+1] * (s32)state.vr[code.vb]._s16[w*2+1];
    }
}

void Interpreter::vmuleub(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._u16[h] = (u16)state.vr[code.va]._u8[h*2+1] * (u16)state.vr[code.vb]._u8[h*2+1];
    }
}

void Interpreter::vmuleuh(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = (u32)state.vr[code.va]._u16[w*2+1] * (u32)state.vr[code.vb]._u16[w*2+1];
    }
}

void Interpreter::vmulosb(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._s16[h] = (s16)state.vr[code.va]._s8[h*2] * (s16)state.vr[code.vb]._s8[h*2];
    }
}

void Interpreter::vmulosh(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._s32[w] = (s32)state.vr[code.va]._s16[w*2] * (s32)state.vr[code.vb]._s16[w*2];
    }
}

void Interpreter::vmuloub(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._u16[h] = (u16)state.vr[code.va]._u8[h*2] * (u16)state.vr[code.vb]._u8[h*2];
    }
}

void Interpreter::vmulouh(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = (u32)state.vr[code.va]._u16[w*2] * (u32)state.vr[code.vb]._u16[w*2];
    }
}

void Interpreter::vnmsubfp(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._f32[w] = -(state.vr[code.va]._f32[w] * state.vr[code.vc]._f32[w] - state.vr[code.vb]._f32[w]);
    }
}

void Interpreter::vnor(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = ~(state.vr[code.va]._u32[w] | state.vr[code.vb]._u32[w]);
    }
}

void Interpreter::vor(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = state.vr[code.va]._u32[w] | state.vr[code.vb]._u32[w];
    }
}

void Interpreter::vperm(Instruction code)
{
    u8 tmpSRC[32];
    memcpy(tmpSRC, state.vr[code.vb]._u8, 16);
    memcpy(tmpSRC + 16, state.vr[code.va]._u8, 16);

    for (int b = 0; b < 16; b++) {
        u8 index = state.vr[code.vc]._u8[b] & 0x1f;

        state.vr[code.vd]._u8[b] = tmpSRC[0x1f - index];
    }
}

void Interpreter::vpkpx(Instruction code)
{
    for (int h = 0; h < 4; h++) {
        const u16 bb7  = state.vr[code.vb]._u8[15 - (h*4 + 0)] & 0x1;
        const u16 bb8  = state.vr[code.vb]._u8[15 - (h*4 + 1)] >> 3;
        const u16 bb16 = state.vr[code.vb]._u8[15 - (h*4 + 2)] >> 3;
        const u16 bb24 = state.vr[code.vb]._u8[15 - (h*4 + 3)] >> 3;
        const u16 ab7  = state.vr[code.va]._u8[15 - (h*4 + 0)] & 0x1;
        const u16 ab8  = state.vr[code.va]._u8[15 - (h*4 + 1)] >> 3;
        const u16 ab16 = state.vr[code.va]._u8[15 - (h*4 + 2)] >> 3;
        const u16 ab24 = state.vr[code.va]._u8[15 - (h*4 + 3)] >> 3;

        state.vr[code.vd]._u16[3 - h] = (bb7 << 15) | (bb8 << 10) | (bb16 << 5) | bb24;
        state.vr[code.vd]._u16[7 - h] = (ab7 << 15) | (ab8 << 10) | (ab16 << 5) | ab24;
    }
}

void Interpreter::vpkshss(Instruction code)
{
    for (int b = 0; b < 8; b++) {
        s16 result = state.vr[code.va]._s16[b];

        if (result > INT8_MAX) {
            result = INT8_MAX;
            state.vscr.SAT = 1;
        }
        else if (result < INT8_MIN) {
            result = INT8_MIN;
            state.vscr.SAT = 1;
        }

        state.vr[code.vd]._s8[b+8] = result;

        result = state.vr[code.vb]._s16[b];

        if (result > INT8_MAX) {
            result = INT8_MAX;
            state.vscr.SAT = 1;
        }
        else if (result < INT8_MIN) {
            result = INT8_MIN;
            state.vscr.SAT = 1;
        }

        state.vr[code.vd]._s8[b] = result;
    }
}

void Interpreter::vpkshus(Instruction code)
{
    for (int b = 0; b < 8; b++) {
        s16 result = state.vr[code.va]._s16[b];

        if (result > UINT8_MAX) {
            result = UINT8_MAX;
            state.vscr.SAT = 1;
        }
        else if (result < 0) {
            result = 0;
            state.vscr.SAT = 1;
        }

        state.vr[code.vd]._u8[b+8] = result;

        result = state.vr[code.vb]._s16[b];

        if (result > UINT8_MAX) {
            result = UINT8_MAX;
            state.vscr.SAT = 1;
        }
        else if (result < 0) {
            result = 0;
            state.vscr.SAT = 1;
        }

        state.vr[code.vd]._u8[b] = result;
    }
}

void Interpreter::vpkswss(Instruction code)
{
    for (int h = 0; h < 4; h++) {
        s32 result = state.vr[code.va]._s32[h];

        if (result > INT16_MAX) {
            result = INT16_MAX;
            state.vscr.SAT = 1;
        }
        else if (result < INT16_MIN) {
            result = INT16_MIN;
            state.vscr.SAT = 1;
        }

        state.vr[code.vd]._s16[h+4] = result;

        result = state.vr[code.vb]._s32[h];

        if (result > INT16_MAX) {
            result = INT16_MAX;
            state.vscr.SAT = 1;
        }
        else if (result < INT16_MIN) {
            result = INT16_MIN;
            state.vscr.SAT = 1;
        }

        state.vr[code.vd]._s16[h] = result;
    }
}

void Interpreter::vpkswus(Instruction code)
{
    for (int h = 0; h < 4; h++) {
        s32 result = state.vr[code.va]._s32[h];

        if (result > UINT16_MAX) {
            result = UINT16_MAX;
            state.vscr.SAT = 1;
        }
        else if (result < 0) {
            result = 0;
            state.vscr.SAT = 1;
        }

        state.vr[code.vd]._u16[h+4] = result;

        result = state.vr[code.vb]._s32[h];

        if (result > UINT16_MAX) {
            result = UINT16_MAX;
            state.vscr.SAT = 1;
        }
        else if (result < 0) {
            result = 0;
            state.vscr.SAT = 1;
        }

        state.vr[code.vd]._u16[h] = result;
    }
}

void Interpreter::vpkuhum(Instruction code)
{
    for (int b = 0; b < 8; b++) {
        state.vr[code.vd]._u8[b+8] = state.vr[code.va]._u8[b*2];
        state.vr[code.vd]._u8[b  ] = state.vr[code.vb]._u8[b*2];
    }
}

void Interpreter::vpkuhus(Instruction code)
{
    for (int b = 0; b < 8; b++) {
        u16 result = state.vr[code.va]._u16[b];

        if (result > UINT8_MAX) {
            result = UINT8_MAX;
            state.vscr.SAT = 1;
        }

        state.vr[code.vd]._u8[b+8] = result;

        result = state.vr[code.vb]._u16[b];

        if (result > UINT8_MAX) {
            result = UINT8_MAX;
            state.vscr.SAT = 1;
        }

        state.vr[code.vd]._u8[b] = result;
    }
}

void Interpreter::vpkuwum(Instruction code)
{
    for (int h = 0; h < 4; h++) {
        state.vr[code.vd]._u16[h+4] = state.vr[code.va]._u16[h*2];
        state.vr[code.vd]._u16[h  ] = state.vr[code.vb]._u16[h*2];
    }
}

void Interpreter::vpkuwus(Instruction code)
{
    for (int h = 0; h < 4; h++) {
        u32 result = state.vr[code.va]._u32[h];

        if (result > UINT16_MAX) {
            result = UINT16_MAX;
            state.vscr.SAT = 1;
        }

        state.vr[code.vd]._u16[h+4] = result;

        result = state.vr[code.vb]._u32[h];

        if (result > UINT16_MAX) {
            result = UINT16_MAX;
            state.vscr.SAT = 1;
        }

        state.vr[code.vd]._u16[h] = result;
    }
}

void Interpreter::vrefp(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._f32[w] = 1.0f / state.vr[code.vb]._f32[w];
    }
}

void Interpreter::vrfim(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._f32[w] = floor(state.vr[code.vb]._f32[w]);
    }
}

void Interpreter::vrfin(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._f32[w] = floor(state.vr[code.vb]._f32[w] + 0.5f);
    }
}

void Interpreter::vrfip(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._f32[w] = ceil(state.vr[code.vb]._f32[w]);
    }
}

void Interpreter::vrfiz(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        f32 f;
        modff(state.vr[code.vb]._f32[w], &f);
        state.vr[code.vd]._f32[w] = f;
    }
}

void Interpreter::vrlb(Instruction code)
{
    for (int b = 0; b < 16; b++) {
        int nRot = state.vr[code.vb]._u8[b] & 0x7;

        state.vr[code.vd]._u8[b] = (state.vr[code.va]._u8[b] << nRot) | (state.vr[code.va]._u8[b] >> (8 - nRot));
    }
}

void Interpreter::vrlh(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._u16[h] = rotl16(state.vr[code.va]._u16[h], state.vr[code.vb]._u8[h*2] & 0xf);
    }
}

void Interpreter::vrlw(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = rotl32(state.vr[code.va]._u32[w], state.vr[code.vb]._u8[w*4] & 0x1f);
    }
}

void Interpreter::vrsqrtefp(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._f32[w] = 1.0f / sqrtf(state.vr[code.vb]._f32[w]);
    }
}

void Interpreter::vsel(Instruction code)
{
    for (int b = 0; b < 16; b++) {
        state.vr[code.vd]._u8[b] = (state.vr[code.vb]._u8[b] & state.vr[code.vc]._u8[b]) | (state.vr[code.va]._u8[b] & (~state.vr[code.vc]._u8[b]));
    }
}

void Interpreter::vsl(Instruction code)
{
    u8 sh = state.vr[code.vb]._u8[0] & 0x7;

    u32 t = 1;

    for (int b = 0; b < 16; b++) {
        t &= (state.vr[code.vb]._u8[b] & 0x7) == sh;
    }

    if (t) {
        state.vr[code.vd]._u8[0] = state.vr[code.va]._u8[0] << sh;

        for (int b = 1; b < 16; b++) {
            state.vr[code.vd]._u8[b] = (state.vr[code.va]._u8[b] << sh) | (state.vr[code.va]._u8[b-1] >> (8 - sh));
        }
    }
}

void Interpreter::vslb(Instruction code)
{
    for (int b = 0; b < 16; b++) {
        state.vr[code.vd]._u8[b] = state.vr[code.va]._u8[b] << (state.vr[code.vb]._u8[b] & 0x7);
    }
}

void Interpreter::vsldoi(Instruction code)
{
    u8 tmpSRC[32];
    memcpy(tmpSRC, state.vr[code.vb]._u8, 16);
    memcpy(tmpSRC + 16, state.vr[code.va]._u8, 16);
    for (int b = 0; b < 16; b++) {
        state.vr[code.vd]._u8[15 - b] = tmpSRC[31 - (b + code.sh)];
    }
}

void Interpreter::vslh(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._u16[h] = state.vr[code.va]._u16[h] << (state.vr[code.vb]._u8[h*2] & 0xf);
    }
}

void Interpreter::vslo(Instruction code)
{
    u8 nShift = (state.vr[code.vb]._u8[0] >> 3) & 0xf;

    state.vr[code.vd].clear();

    for (u8 b = 0; b < 16 - nShift; b++) {
        state.vr[code.vd]._u8[15 - b] = state.vr[code.va]._u8[15 - (b + nShift)];
    }
}

void Interpreter::vslw(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = state.vr[code.va]._u32[w] << (state.vr[code.vb]._u32[w] & 0x1f);
    }
}

void Interpreter::vspltb(Instruction code)
{
    u8 byte = state.vr[code.vb]._u8[15 - code.vuimm];

    for (int b = 0; b < 16; b++) {
        state.vr[code.vd]._u8[b] = byte;
    }
}

void Interpreter::vsplth(Instruction code)
{
    const u16 hword = state.vr[code.vb]._u16[7 - code.vuimm];
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._u16[h] = hword;
    }
}

void Interpreter::vspltisb(Instruction code)
{
    for (int b = 0; b < 16; b++) {
        state.vr[code.vd]._u8[b] = code.vsimm;
    }
}

void Interpreter::vspltish(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._u16[h] = (s16)code.vsimm;
    }
}

void Interpreter::vspltisw(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = (s32)code.vsimm;
    }
}

void Interpreter::vspltw(Instruction code)
{
    const u32 word = state.vr[code.vb]._u32[3 - code.vuimm];
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = word;
    }
}

void Interpreter::vsr(Instruction code)
{
    u8 sh = state.vr[code.vb]._u8[0] & 0x7;
    u32 t = 1;

    for (int b = 0; b < 16; b++) {
        t &= (state.vr[code.vb]._u8[b] & 0x7) == sh;
    }
    if (t) {
        state.vr[code.vd]._u8[15] = state.vr[code.va]._u8[15] >> sh;

        for (int b = 0; b < 15; b++) {
            state.vr[code.vd]._u8[14-b] = (state.vr[code.va]._u8[14-b] >> sh) | (state.vr[code.va]._u8[14-b+1] << (8 - sh));
        }
    }
}

void Interpreter::vsrab(Instruction code)
{
    for (int b = 0; b < 16; b++) {
        state.vr[code.vd]._s8[b] = state.vr[code.va]._s8[b] >> (state.vr[code.vb]._u8[b] & 0x7);
    }
}

void Interpreter::vsrah(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._s16[h] = state.vr[code.va]._s16[h] >> (state.vr[code.vb]._u8[h*2] & 0xf);
    }
}

void Interpreter::vsraw(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._s32[w] = state.vr[code.va]._s32[w] >> (state.vr[code.vb]._u8[w*4] & 0x1f);
    }
}

void Interpreter::vsrb(Instruction code)
{
    for (int b = 0; b < 16; b++) {
        state.vr[code.vd]._u8[b] = state.vr[code.va]._u8[b] >> (state.vr[code.vb]._u8[b] & 0x7);
    }
}

void Interpreter::vsrh(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._u16[h] = state.vr[code.va]._u16[h] >> (state.vr[code.vb]._u8[h*2] & 0xf);
    }
}

void Interpreter::vsro(Instruction code)
{
    const u8 nShift = (state.vr[code.vb]._u8[0] >> 3) & 0xf;
    state.vr[code.vd].clear();

    for (u8 b = 0; b < 16 - nShift; b++) {
        state.vr[code.vd]._u8[b] = state.vr[code.va]._u8[b + nShift];
    }
}

void Interpreter::vsrw(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = state.vr[code.va]._u32[w] >> (state.vr[code.vb]._u8[w*4] & 0x1f);
    }
}

void Interpreter::vsubcuw(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = state.vr[code.va]._u32[w] < state.vr[code.vb]._u32[w] ? 0 : 1;
    }
}

void Interpreter::vsubfp(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._f32[w] = state.vr[code.va]._f32[w] - state.vr[code.vb]._f32[w];
    }
}

void Interpreter::vsubsbs(Instruction code)
{
    for (int b = 0; b < 16; b++) {
        s16 result = (s16)state.vr[code.va]._s8[b] - (s16)state.vr[code.vb]._s8[b];

        if (result < INT8_MIN) {
            state.vr[code.vd]._s8[b] = INT8_MIN;
            state.vscr.SAT = 1;
        }
        else if (result > INT8_MAX) {
            state.vr[code.vd]._s8[b] = INT8_MAX;
            state.vscr.SAT = 1;
        }
        else
            state.vr[code.vd]._s8[b] = (s8)result;
    }
}

void Interpreter::vsubshs(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        s32 result = (s32)state.vr[code.va]._s16[h] - (s32)state.vr[code.vb]._s16[h];

        if (result < INT16_MIN) {
            state.vr[code.vd]._s16[h] = (s16)INT16_MIN;
            state.vscr.SAT = 1;
        }
        else if (result > INT16_MAX) {
            state.vr[code.vd]._s16[h] = (s16)INT16_MAX;
            state.vscr.SAT = 1;
        }
        else
            state.vr[code.vd]._s16[h] = (s16)result;
    }
}

void Interpreter::vsubsws(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        s64 result = (s64)state.vr[code.va]._s32[w] - (s64)state.vr[code.vb]._s32[w];

        if (result < INT32_MIN) {
            state.vr[code.vd]._s32[w] = (s32)INT32_MIN;
            state.vscr.SAT = 1;
        }
        else if (result > INT32_MAX) {
            state.vr[code.vd]._s32[w] = (s32)INT32_MAX;
            state.vscr.SAT = 1;
        }
        else
            state.vr[code.vd]._s32[w] = (s32)result;
    }
}

void Interpreter::vsububm(Instruction code)
{
    for (int b = 0; b < 16; b++) {
        state.vr[code.vd]._u8[b] = (u8)((state.vr[code.va]._u8[b] - state.vr[code.vb]._u8[b]) & 0xFF);
    }
}

void Interpreter::vsububs(Instruction code)
{
    for (int b = 0; b < 16; b++) {
        s16 result = (s16)state.vr[code.va]._u8[b] - (s16)state.vr[code.vb]._u8[b];

        if (result < 0) {
            state.vr[code.vd]._u8[b] = 0;
            state.vscr.SAT = 1;
        } else {
            state.vr[code.vd]._u8[b] = (u8)result;
        }
    }
}

void Interpreter::vsubuhm(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._u16[h] = state.vr[code.va]._u16[h] - state.vr[code.vb]._u16[h];
    }
}

void Interpreter::vsubuhs(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        s32 result = (s32)state.vr[code.va]._u16[h] - (s32)state.vr[code.vb]._u16[h];

        if (result < 0) {
            state.vr[code.vd]._u16[h] = 0;
            state.vscr.SAT = 1;
        }
        else
            state.vr[code.vd]._u16[h] = (u16)result;
    }
}

void Interpreter::vsubuwm(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._u32[w] = state.vr[code.va]._u32[w] - state.vr[code.vb]._u32[w];
    }
}

void Interpreter::vsubuws(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        s64 result = (s64)state.vr[code.va]._u32[w] - (s64)state.vr[code.vb]._u32[w];

        if (result < 0) {
            state.vr[code.vd]._u32[w] = 0;
            state.vscr.SAT = 1;
        }
        else
            state.vr[code.vd]._u32[w] = (u32)result;
    }
}

void Interpreter::vsum2sws(Instruction code)
{
    for (int n = 0; n < 2; n++) {
        s64 sum = (s64)state.vr[code.va]._s32[n*2] + state.vr[code.va]._s32[n*2 + 1] + state.vr[code.vb]._s32[n*2];

        if (sum > INT32_MAX) {
            state.vr[code.vd]._s32[n*2] = (s32)INT32_MAX;
            state.vscr.SAT = 1;
        }
        else if (sum < INT32_MIN) {
            state.vr[code.vd]._s32[n*2] = (s32)INT32_MIN;
            state.vscr.SAT = 1;
        }
        else {
            state.vr[code.vd]._s32[n*2] = (s32)sum;
        }
    }
    state.vr[code.vd]._s32[1] = 0;
    state.vr[code.vd]._s32[3] = 0;
}

void Interpreter::vsum4sbs(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        s64 sum = state.vr[code.vb]._s32[w];

        for (int b = 0; b < 4; b++) {
            sum += state.vr[code.va]._s8[w*4 + b];
        }

        if (sum > INT32_MAX) {
            state.vr[code.vd]._s32[w] = (s32)INT32_MAX;
            state.vscr.SAT = 1;
        }
        else if (sum < INT32_MIN) {
            state.vr[code.vd]._s32[w] = (s32)INT32_MIN;
            state.vscr.SAT = 1;
        }
        else {
            state.vr[code.vd]._s32[w] = (s32)sum;
        }
    }
}

void Interpreter::vsum4shs(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        s64 sum = state.vr[code.vb]._s32[w];

        for (int h = 0; h < 2; h++) {
            sum += state.vr[code.va]._s16[w*2 + h];
        }

        if (sum > INT32_MAX) {
            state.vr[code.vd]._s32[w] = (s32)INT32_MAX;
            state.vscr.SAT = 1;
        }
        else if (sum < INT32_MIN) {
            state.vr[code.vd]._s32[w] = (s32)INT32_MIN;
            state.vscr.SAT = 1;
        }
        else {
            state.vr[code.vd]._s32[w] = (s32)sum;
        }
    }
}

void Interpreter::vsum4ubs(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        u64 sum = state.vr[code.vb]._u32[w];

        for (int b = 0; b < 4; b++) {
            sum += state.vr[code.va]._u8[w*4 + b];
        }

        if (sum > UINT32_MAX) {
            state.vr[code.vd]._u32[w] = (u32)UINT32_MAX;
            state.vscr.SAT = 1;
        }
        else {
            state.vr[code.vd]._u32[w] = (u32)sum;
        }
    }
}

void Interpreter::vsumsws(Instruction code)
{
    state.vr[code.vd].clear();

    s64 sum = state.vr[code.vb]._s32[3];

    for (int w = 0; w < 4; w++) {
        sum += state.vr[code.va]._s32[w];
    }

    if (sum > INT32_MAX) {
        state.vr[code.vd]._s32[0] = (s32)INT32_MAX;
        state.vscr.SAT = 1;
    }
    else if (sum < INT32_MIN) {
        state.vr[code.vd]._s32[0] = (s32)INT32_MIN;
        state.vscr.SAT = 1;
    }
    else
        state.vr[code.vd]._s32[0] = (s32)sum;
}

void Interpreter::vupkhpx(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._s8[(3 - w)*4 + 3] = state.vr[code.vb]._s8[w*2 + 0] >> 7;
        state.vr[code.vd]._u8[(3 - w)*4 + 2] = (state.vr[code.vb]._u8[w*2 + 0] >> 2) & 0x1f;
        state.vr[code.vd]._u8[(3 - w)*4 + 1] = ((state.vr[code.vb]._u8[w*2 + 0] & 0x3) << 3) | ((state.vr[code.vb]._u8[w*2 + 1] >> 5) & 0x7);
        state.vr[code.vd]._u8[(3 - w)*4 + 0] = state.vr[code.vb]._u8[w*2 + 1] & 0x1f;
    }
}

void Interpreter::vupkhsb(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._s16[h] = state.vr[code.vb]._s8[h];
    }
}

void Interpreter::vupkhsh(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._s32[w] = state.vr[code.vb]._s16[w];
    }
}

void Interpreter::vupklpx(Instruction code)
{
    for (int w = 0; w < 4; w++) {
        state.vr[code.vd]._s8[(3 - w)*4 + 3] = state.vr[code.vb]._s8[8 + w*2 + 0] >> 7;
        state.vr[code.vd]._u8[(3 - w)*4 + 2] = (state.vr[code.vb]._u8[8 + w*2 + 0] >> 2) & 0x1f;
        state.vr[code.vd]._u8[(3 - w)*4 + 1] = ((state.vr[code.vb]._u8[8 + w*2 + 0] & 0x3) << 3) | ((state.vr[code.vb]._u8[8 + w*2 + 1] >> 5) & 0x7);
        state.vr[code.vd]._u8[(3 - w)*4 + 0] = state.vr[code.vb]._u8[8 + w*2 + 1] & 0x1f;
    }
}

void Interpreter::vupklsb(Instruction code)
{
    for (int h = 0; h < 8; h++) {
        state.vr[code.vd]._s16[h] = state.vr[code.vb]._s8[8 + h];
    }
}

void Interpreter::vupklsh(Instruction code)
{
    state.vr[code.vd]._s32[0] = state.vr[code.vb]._s16[4 + 0];
    state.vr[code.vd]._s32[1] = state.vr[code.vb]._s16[4 + 1];
    state.vr[code.vd]._s32[2] = state.vr[code.vb]._s16[4 + 2];
    state.vr[code.vd]._s32[3] = state.vr[code.vb]._s16[4 + 3];
}

void Interpreter::vxor(Instruction code)
{
    state.vr[code.vd]._u64[0] = state.vr[code.va]._u64[0] ^ state.vr[code.vb]._u64[0];
    state.vr[code.vd]._u64[1] = state.vr[code.va]._u64[1] ^ state.vr[code.vb]._u64[1];
}

}  // namespace ppu
}  // namespace cpu
