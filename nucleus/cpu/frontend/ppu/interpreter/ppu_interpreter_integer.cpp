/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_interpreter.h"
#include "nucleus/cpu/frontend/ppu/ppu_utils.h"

namespace cpu {
namespace ppu {

// PowerPC rotation-related functions
inline U64 rotl64(const U64 x, const U8 n) { return (x << n) | (x >> (64 - n)); }
inline U64 rotl32(const U32 x, const U8 n) { return rotl64((U64)x | ((U64)x << 32), n); }
inline U16 rotl16(const U16 x, const U8 n) { return (x << n) | (x >> (16 - n)); }
inline U8 rotl8(const U8 x, const U8 n) { return (x << n) | (x >> (8 - n)); }

/**
 * PPC64 Instructions:
 *  - UISA: Integer instructions (Section: 4.2.1)
 */

void Interpreter::addx(Instruction code)
{
    state.gpr[code.rd] = state.gpr[code.ra] + state.gpr[code.rb];
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.rd], (S64)0); }
    if (code.oe) unknown("addo");
}

void Interpreter::addcx(Instruction code)
{
    const S64 gpra = state.gpr[code.ra];
    const S64 gprb = state.gpr[code.rb];
    state.gpr[code.rd] = gpra + gprb;
    state.xer.CA = isCarry(gpra, gprb);
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.rd], (S64)0); }
    if (code.oe) unknown("addco");
}

void Interpreter::addex(Instruction code)
{
    const U64 gpra = state.gpr[code.ra];
    const U64 gprb = state.gpr[code.rb];
    if (state.xer.CA) {
        if (gpra == ~0ULL) {
            state.gpr[code.rd] = gprb;
            state.xer.CA = 1;
        }
        else {
            state.gpr[code.rd] = gpra + 1 + gprb;
            state.xer.CA = isCarry(gpra + 1, gprb);
        }
    }
    else {
        state.gpr[code.rd] = gpra + gprb;
        state.xer.CA = isCarry(gpra, gprb);
    }
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.rd], (S64)0); }
    if (code.oe) unknown("addeo");
}

void Interpreter::addi(Instruction code)
{
    state.gpr[code.rd] = code.ra ? ((S64)state.gpr[code.ra] + code.simm) : code.simm;
}

void Interpreter::addic(Instruction code)
{
    const U64 gpra = state.gpr[code.ra];
    state.gpr[code.rd] = gpra + code.simm;
    state.xer.CA = isCarry(gpra, code.simm);
}

void Interpreter::addic_(Instruction code)
{
    const U64 gpra = state.gpr[code.ra];
    state.gpr[code.rd] = gpra + code.simm;
    state.xer.CA = isCarry(gpra, code.simm);
    state.cr.updateField(0, (S64)state.gpr[code.rd], (S64)0);
}

void Interpreter::addis(Instruction code)
{
    state.gpr[code.rd] = code.ra ? ((S64)state.gpr[code.ra] + (code.simm << 16)) : (code.simm << 16);
}

void Interpreter::addmex(Instruction code)
{
    const S64 gpra = state.gpr[code.ra];
    state.gpr[code.rd] = gpra + state.xer.CA - 1;
    state.xer.CA |= gpra != 0;
    if (code.oe) unknown("addmeo");
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.rd], (S64)0); }
}

void Interpreter::addzex(Instruction code)
{
    const U64 gpra = state.gpr[code.ra];
    state.gpr[code.rd] = gpra + state.xer.CA;
    state.xer.CA = isCarry(gpra, state.xer.CA);
    if (code.oe) unknown("addzeo");
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.rd], (S64)0); }
}

void Interpreter::andx(Instruction code)
{
    state.gpr[code.ra] = state.gpr[code.rs] & state.gpr[code.rb];
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::andcx(Instruction code)
{
    state.gpr[code.ra] = state.gpr[code.rs] & ~state.gpr[code.rb];
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::andi_(Instruction code)
{
    state.gpr[code.ra] = state.gpr[code.rs] & code.uimm;
    state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0);
}

void Interpreter::andis_(Instruction code)
{
    state.gpr[code.ra] = state.gpr[code.rs] & (code.uimm << 16);
    state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0);
}

void Interpreter::cmp(Instruction code)
{
    if (code.l10) {
        state.cr.updateField(code.crfd, (S64)state.gpr[code.ra], (S64)state.gpr[code.rb]);
    } else {
        state.cr.updateField(code.crfd, (S32)state.gpr[code.ra], (S32)state.gpr[code.rb]);
    }
}

void Interpreter::cmpi(Instruction code)
{
    if (code.l10) {
        state.cr.updateField(code.crfd, (S64)state.gpr[code.ra], (S64)code.simm);
    } else {
        state.cr.updateField(code.crfd, (S32)state.gpr[code.ra], (S32)code.simm);
    }
}

void Interpreter::cmpl(Instruction code)
{
    if (code.l10) {
        state.cr.updateField(code.crfd, (U64)state.gpr[code.ra], (U64)state.gpr[code.rb]);
    } else {
        state.cr.updateField(code.crfd, (U32)state.gpr[code.ra], (U32)state.gpr[code.rb]);
    }
}

void Interpreter::cmpli(Instruction code)
{
    if (code.l10) {
        state.cr.updateField(code.crfd, (U64)state.gpr[code.ra], (U64)code.uimm);
    } else {
        state.cr.updateField(code.crfd, (U32)state.gpr[code.ra], (U32)code.uimm);
    }
}

void Interpreter::cntlzdx(Instruction code)
{
    int i;
    for (i = 0; i < 64; i++) {
        if (state.gpr[code.rs] & (1ULL << (63 - i))) {
            break;
        }
    }
    state.gpr[code.ra] = i;
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::cntlzwx(Instruction code)
{
    int i;
    for (i = 0; i < 32; i++) {
        if (state.gpr[code.rs] & (1ULL << (31 - i))) {
            break;
        }
    }
    state.gpr[code.ra] = i;
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::divdx(Instruction code)
{
    const S64 gpra = state.gpr[code.ra];
    const S64 gprb = state.gpr[code.rb];
    if (gprb == 0 || ((U64)gpra == (1ULL << 63) && gprb == -1)) {
        if (code.oe) unknown("divdo");
        state.gpr[code.rd] = 0;
    }
    else {
        state.gpr[code.rd] = gpra / gprb;
    }
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.rd], (S64)0); }
}

void Interpreter::divdux(Instruction code)
{
    const U64 gpra = state.gpr[code.ra];
    const S64 gprb = state.gpr[code.rb];
    if (gprb == 0) {
        if (code.oe) unknown("divduo");
        state.gpr[code.rd] = 0;
    }
    else {
        state.gpr[code.rd] = gpra / gprb;
    }
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.rd], (S64)0); }
}

void Interpreter::divwx(Instruction code)
{
    const S32 gpra = state.gpr[code.ra];
    const S32 gprb = state.gpr[code.rb];
    if (gprb == 0 || ((U32)gpra == (1 << 31) && gprb == -1)) {
        if (code.oe) unknown("divwo");
        state.gpr[code.rd] = 0;
    }
    else {
        state.gpr[code.rd] = (U32)(gpra / gprb);
    }
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.rd], (S64)0); }
}

void Interpreter::divwux(Instruction code)
{
    const U32 gpra = state.gpr[code.ra];
    const U32 gprb = state.gpr[code.rb];
    if (gprb == 0) {
        if (code.oe) unknown("divwuo");
        state.gpr[code.rd] = 0;
    }
    else {
        state.gpr[code.rd] = gpra / gprb;
    }
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.rd], (S64)0); }
}

void Interpreter::eqvx(Instruction code)
{
    state.gpr[code.ra] = ~(state.gpr[code.rs] ^ state.gpr[code.rb]);
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::extsbx(Instruction code)
{
    state.gpr[code.ra] = (S64)(U8)state.gpr[code.rs];
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::extshx(Instruction code)
{
    state.gpr[code.ra] = (S64)(S16)state.gpr[code.rs];
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::extswx(Instruction code)
{
    state.gpr[code.ra] = (S64)(S32)state.gpr[code.rs];
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::mulhdx(Instruction code)
{
    const S64 a = state.gpr[code.ra];
    const S64 b = state.gpr[code.rb];
#if defined(NUCLEUS_PLATFORM_WINDOWS)
    state.gpr[code.rd] = __mulh(a, b);
#elif defined(NUCLEUS_PLATFORM_LINUX) || defined(NUCLEUS_PLATFORM_OSX)
    __asm__("mulq %[b]" : "=d" (state.gpr[code.rd]) : [a] "a" (a), [b] "rm" (b));
#endif
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.rd], (S64)0); }
}

void Interpreter::mulhdux(Instruction code)
{
    const U64 a = state.gpr[code.ra];
    const U64 b = state.gpr[code.rb];
#if defined(NUCLEUS_PLATFORM_WINDOWS)
    state.gpr[code.rd] = __umulh(a, b);
#elif defined(NUCLEUS_PLATFORM_LINUX) || defined(NUCLEUS_PLATFORM_OSX)
    __asm__("imulq %[b]" : "=d" (state.gpr[code.rd]) : [a] "a" (a), [b] "rm" (b));
#endif
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.rd], (S64)0); }
}

void Interpreter::mulhwx(Instruction code)
{
    S32 a = state.gpr[code.ra];
    S32 b = state.gpr[code.rb];
    state.gpr[code.rd] = ((S64)a * (S64)b) >> 32;
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.rd], (S64)0); }
}

void Interpreter::mulhwux(Instruction code)
{
    U32 a = state.gpr[code.ra];
    U32 b = state.gpr[code.rb];
    state.gpr[code.rd] = ((U64)a * (U64)b) >> 32;
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.rd], (S64)0); }
}

void Interpreter::mulldx(Instruction code)
{
    state.gpr[code.rd] = (S64)((S64)state.gpr[code.ra] * (S64)state.gpr[code.rb]);
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.rd], (S64)0); }
    if (code.oe) unknown("mulldo");
}

void Interpreter::mulli(Instruction code)
{
    state.gpr[code.rd] = (S64)state.gpr[code.ra] * code.simm;
}

void Interpreter::mullwx(Instruction code)
{
    state.gpr[code.rd] = (S64)((S64)(S32)state.gpr[code.ra] * (S64)(S32)state.gpr[code.rb]);
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.rd], (S64)0); }
    if (code.oe) unknown("mullwo");
}

void Interpreter::nandx(Instruction code)
{
    state.gpr[code.ra] = ~(state.gpr[code.rs] & state.gpr[code.rb]);

    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::negx(Instruction code)
{
    state.gpr[code.rd] = 0 - state.gpr[code.ra];
    if (code.oe) unknown("nego");
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.rd], (S64)0); }
}

void Interpreter::norx(Instruction code)
{
    state.gpr[code.ra] = ~(state.gpr[code.rs] | state.gpr[code.rb]);
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::orx(Instruction code)
{
    state.gpr[code.ra] = state.gpr[code.rs] | state.gpr[code.rb];
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::orcx(Instruction code)
{
    state.gpr[code.ra] = state.gpr[code.rs] | ~state.gpr[code.rb];
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::ori(Instruction code)
{
    state.gpr[code.ra] = state.gpr[code.rs] | code.uimm;
}

void Interpreter::oris(Instruction code)
{
    state.gpr[code.ra] = state.gpr[code.rs] | (code.uimm << 16);
}

void Interpreter::rldc_lr(Instruction code)
{
    const U32 rotate = state.gpr[code.rb] & 0x3F;
    const U32 mb = code.mb | (code.mb_ << 5);
    if (code.aa) {
        // rldcrx
        state.gpr[code.ra] = rotl64(state.gpr[code.rs], rotate) & rotateMask[0][mb];
    }
    else {
        // rldclx
        state.gpr[code.ra] = rotl64(state.gpr[code.rs], rotate) & rotateMask[mb][63];
    }
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::rldicx(Instruction code)
{
    const U32 sh = code.sh | (code.sh_ << 5);
    const U32 mb = code.mb | (code.mb_ << 5);
    state.gpr[code.ra] = rotl64(state.gpr[code.rs], sh) & rotateMask[mb][63-sh];
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::rldiclx(Instruction code)
{
    const U32 sh = code.sh | (code.sh_ << 5);
    const U32 mb = code.mb | (code.mb_ << 5);
    state.gpr[code.ra] = rotl64(state.gpr[code.rs], sh) & rotateMask[mb][63];
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::rldicrx(Instruction code)
{
    const U32 sh = code.sh | (code.sh_ << 5);
    const U32 me = code.me_ | (code.me__ << 5);
    state.gpr[code.ra] = rotl64(state.gpr[code.rs], sh) & rotateMask[0][me];
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::rldimix(Instruction code)
{
    const U32 sh = code.sh | (code.sh_ << 5);
    const U32 mb = code.mb | (code.mb_ << 5);
    const U64 mask = rotateMask[mb][63-sh];
    state.gpr[code.ra] = (state.gpr[code.ra] & ~mask) | (rotl64(state.gpr[code.rs], sh) & mask);
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::rlwimix(Instruction code)
{
    const U64 r = rotl32(state.gpr[code.rs], code.sh);
    const U64 m = rotateMask[32 + code.mb][32 + code.me];
    state.gpr[code.ra] = (r & m) | (state.gpr[code.ra] & ~m);
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::rlwinmx(Instruction code)
{
    state.gpr[code.ra] = rotl32(state.gpr[code.rs], code.sh) & rotateMask[32 + code.mb][32 + code.me];
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::rlwnmx(Instruction code)
{
    state.gpr[code.ra] = rotl32(state.gpr[code.rs], state.gpr[code.rb] & 0x1f) & rotateMask[32 + code.mb][32 + code.me];
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::sldx(Instruction code)
{
    const U64 shift = state.gpr[code.rb] & 0x7F;
    state.gpr[code.ra] = (shift & 0x40) ? 0 : (state.gpr[code.rs] << shift);
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::slwx(Instruction code)
{
    U32 n = state.gpr[code.rb] & 0x1f;
    U32 r = rotl32((U32)state.gpr[code.rs], n);
    U32 m = (state.gpr[code.rb] & 0x20) ? 0 : rotateMask[32][63 - n];
    state.gpr[code.ra] = r & m;
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::sradx(Instruction code)
{
    S64 RS = state.gpr[code.rs];
    U8 shift = state.gpr[code.rb] & 127;
    if (shift > 63) {
        state.gpr[code.ra] = 0 - (RS < 0);
        state.xer.CA = (RS < 0);
    }
    else {
        state.gpr[code.ra] = RS >> shift;
        state.xer.CA = (RS < 0) & ((state.gpr[code.ra] << shift) != RS);
    }
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::sradix(Instruction code)
{
    const S64 rs = state.gpr[code.rs];
    const U32 sh = code.sh | (code.sh_ << 5);
    state.gpr[code.ra] = rs >> sh;
    state.xer.CA = (rs < 0) & ((state.gpr[code.ra] << sh) != rs);
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::srawx(Instruction code)
{
    S32 gprs = state.gpr[code.rs];
    U8 shift = state.gpr[code.rb] & 63;
    if (shift > 31) {
        state.gpr[code.ra] = 0 - (gprs < 0);
        state.xer.CA = (gprs < 0);
    }
    else {
        state.gpr[code.ra] = gprs >> shift;
        state.xer.CA = (gprs < 0) & ((state.gpr[code.ra] << shift) != gprs);
    }
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::srawix(Instruction code)
{
    S32 gprs = (U32)state.gpr[code.rs];
    state.gpr[code.ra] = gprs >> code.sh;
    state.xer.CA = (gprs < 0) & ((U32)(state.gpr[code.ra] << code.sh) != gprs);
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::srdx(Instruction code)
{
    const U64 shift = state.gpr[code.rb] & 0x7F;
    state.gpr[code.ra] = (shift & 0x40) ? 0 : (state.gpr[code.rs] >> shift);
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::srwx(Instruction code)
{
    U32 n = state.gpr[code.rb] & 0x1f;
    U32 m = (state.gpr[code.rb] & 0x20) ? 0 : rotateMask[32 + n][63];
    U32 r = rotl32((U32)state.gpr[code.rs], 64 - n);
    state.gpr[code.ra] = r & m;
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::subfx(Instruction code)
{
    state.gpr[code.rd] = state.gpr[code.rb] - state.gpr[code.ra];
    if (code.oe) unknown("subfo");
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.rd], (S64)0); }
}

void Interpreter::subfcx(Instruction code)
{
    const U64 gpra = state.gpr[code.ra];
    const S64 gprb = state.gpr[code.rb];
    state.gpr[code.rd] = ~gpra + gprb + 1;
    state.xer.CA = isCarry(~gpra, gprb, 1);
    if (code.oe) unknown("subfco");
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.rd], (S64)0); }
}

void Interpreter::subfex(Instruction code)
{
    const U64 gpra = state.gpr[code.ra];
    const S64 gprb = state.gpr[code.rb];
    state.gpr[code.rd] = ~gpra + gprb + state.xer.CA;
    state.xer.CA = isCarry(~gpra, gprb, state.xer.CA);
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.rd], (S64)0); }
    if (code.oe) unknown("subfeo");
}

void Interpreter::subfic(Instruction code)
{
    const U64 gpra = state.gpr[code.ra];
    const U64 IMM = (S64)code.simm;
    state.gpr[code.rd] = ~gpra + IMM + 1;

    state.xer.CA = isCarry(~gpra, IMM, 1);
}

void Interpreter::subfmex(Instruction code)
{
    const U64 gpra = state.gpr[code.ra];
    state.gpr[code.rd] = ~gpra + state.xer.CA + ~0ULL;
    state.xer.CA = isCarry(~gpra, state.xer.CA, ~0ULL);
    if (code.oe) unknown("subfmeo");
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.rd], (S64)0); }
}

void Interpreter::subfzex(Instruction code)
{
    const U64 gpra = state.gpr[code.ra];
    state.gpr[code.rd] = ~gpra + state.xer.CA;
    state.xer.CA = isCarry(~gpra, state.xer.CA);
    if (code.oe) unknown("subfzeo");
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.rd], (S64)0); }
}

void Interpreter::xorx(Instruction code)
{
    state.gpr[code.ra] = state.gpr[code.rs] ^ state.gpr[code.rb];
    if (code.rc) { state.cr.updateField(0, (S64)state.gpr[code.ra], (S64)0); }
}

void Interpreter::xori(Instruction code)
{
    state.gpr[code.ra] = state.gpr[code.rs] ^ code.uimm;
}

void Interpreter::xoris(Instruction code)
{
    state.gpr[code.ra] = state.gpr[code.rs] ^ (code.uimm << 16);
}

}  // namespace ppu
}  // namespace cpu
