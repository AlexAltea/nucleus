/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_interpreter.h"

namespace cpu {
namespace ppu {

// PowerPC Rotation-related functions
inline u64 rotl64(const u64 x, const u8 n) { return (x << n) | (x >> (64 - n)); }
inline u64 rotl32(const u32 x, const u8 n) { return rotl64((u64)x | ((u64)x << 32), n); }
inline u16 rotl16(const u16 x, const u8 n) { return (x << n) | (x >> (16 - n)); }
inline u8 rotl8(const u8 x, const u8 n) { return (x << n) | (x >> (8 - n)); }

/**
 * PPC64 Instructions:
 *  - UISA: Integer instructions (Section: 4.2.1)
 */

void Interpreter::addx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = thread.gpr[code.ra] + thread.gpr[code.rb];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
    if (code.oe) unknown("addo");
}

void Interpreter::addcx(Instruction code, PPUThread& thread)
{
    const s64 gpra = thread.gpr[code.ra];
    const s64 gprb = thread.gpr[code.rb];
    thread.gpr[code.rd] = gpra + gprb;
    thread.xer.CA = isCarry(gpra, gprb);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
    if (code.oe) unknown("addco");
}

void Interpreter::addex(Instruction code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    const u64 gprb = thread.gpr[code.rb];
    if (thread.xer.CA) {
        if (gpra == ~0ULL) {
            thread.gpr[code.rd] = gprb;
            thread.xer.CA = 1;
        }
        else {
            thread.gpr[code.rd] = gpra + 1 + gprb;
            thread.xer.CA = isCarry(gpra + 1, gprb);
        }
    }
    else {
        thread.gpr[code.rd] = gpra + gprb;
        thread.xer.CA = isCarry(gpra, gprb);
    }
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
    if (code.oe) unknown("addeo");
}

void Interpreter::addi(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = code.ra ? ((s64)thread.gpr[code.ra] + code.simm) : code.simm;
}

void Interpreter::addic(Instruction code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    thread.gpr[code.rd] = gpra + code.simm;
    thread.xer.CA = isCarry(gpra, code.simm);
}

void Interpreter::addic_(Instruction code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    thread.gpr[code.rd] = gpra + code.simm;
    thread.xer.CA = isCarry(gpra, code.simm);
    thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0);
}

void Interpreter::addis(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = code.ra ? ((s64)thread.gpr[code.ra] + (code.simm << 16)) : (code.simm << 16);
}

void Interpreter::addmex(Instruction code, PPUThread& thread)
{
    const s64 gpra = thread.gpr[code.ra];
    thread.gpr[code.rd] = gpra + thread.xer.CA - 1;
    thread.xer.CA |= gpra != 0;
    if (code.oe) unknown("addmeo");
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}

void Interpreter::addzex(Instruction code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    thread.gpr[code.rd] = gpra + thread.xer.CA;
    thread.xer.CA = isCarry(gpra, thread.xer.CA);
    if (code.oe) unknown("addzeo");
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}

void Interpreter::andx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] & thread.gpr[code.rb];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::andcx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] & ~thread.gpr[code.rb];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::andi_(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] & code.uimm;
    thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0);
}

void Interpreter::andis_(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] & (code.uimm << 16);
    thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0);
}

void Interpreter::cmp(Instruction code, PPUThread& thread)
{
    if (code.l10) {
        thread.cr.updateField(code.crfd, (s64)thread.gpr[code.ra], (s64)thread.gpr[code.rb]);
    } else {
        thread.cr.updateField(code.crfd, (s32)thread.gpr[code.ra], (s32)thread.gpr[code.rb]);
    }
}

void Interpreter::cmpi(Instruction code, PPUThread& thread)
{
    if (code.l10) {
        thread.cr.updateField(code.crfd, (s64)thread.gpr[code.ra], (s64)code.simm);
    } else {
        thread.cr.updateField(code.crfd, (s32)thread.gpr[code.ra], (s32)code.simm);
    }
}

void Interpreter::cmpl(Instruction code, PPUThread& thread)
{
    if (code.l10) {
        thread.cr.updateField(code.crfd, (u64)thread.gpr[code.ra], (u64)thread.gpr[code.rb]);
    } else {
        thread.cr.updateField(code.crfd, (u32)thread.gpr[code.ra], (u32)thread.gpr[code.rb]);
    }
}

void Interpreter::cmpli(Instruction code, PPUThread& thread)
{
    if (code.l10) {
        thread.cr.updateField(code.crfd, (u64)thread.gpr[code.ra], (u64)code.uimm);
    } else {
        thread.cr.updateField(code.crfd, (u32)thread.gpr[code.ra], (u32)code.uimm);
    }
}

void Interpreter::cntlzdx(Instruction code, PPUThread& thread)
{
    int i;
    for (i = 0; i < 64; i++) {
        if (thread.gpr[code.rs] & (1ULL << (63 - i))) {
            break;
        }
    }
    thread.gpr[code.ra] = i;
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::cntlzwx(Instruction code, PPUThread& thread)
{
    int i;
    for (i = 0; i < 32; i++) {
        if (thread.gpr[code.rs] & (1ULL << (31 - i))) {
            break;
        }
    }
    thread.gpr[code.ra] = i;
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::divdx(Instruction code, PPUThread& thread)
{
    const s64 gpra = thread.gpr[code.ra];
    const s64 gprb = thread.gpr[code.rb];
    if (gprb == 0 || ((u64)gpra == (1ULL << 63) && gprb == -1)) {
        if (code.oe) unknown("divdo");
        thread.gpr[code.rd] = 0;
    }
    else {
        thread.gpr[code.rd] = gpra / gprb;
    }
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}

void Interpreter::divdux(Instruction code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    const s64 gprb = thread.gpr[code.rb];
    if (gprb == 0) {
        if (code.oe) unknown("divduo");
        thread.gpr[code.rd] = 0;
    }
    else {
        thread.gpr[code.rd] = gpra / gprb;
    }
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}

void Interpreter::divwx(Instruction code, PPUThread& thread)
{
    const s32 gpra = thread.gpr[code.ra];
    const s32 gprb = thread.gpr[code.rb];
    if (gprb == 0 || ((u32)gpra == (1 << 31) && gprb == -1)) {
        if (code.oe) unknown("divwo");
        thread.gpr[code.rd] = 0;
    }
    else {
        thread.gpr[code.rd] = (u32)(gpra / gprb);
    }
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}

void Interpreter::divwux(Instruction code, PPUThread& thread)
{
    const u32 gpra = thread.gpr[code.ra];
    const u32 gprb = thread.gpr[code.rb];
    if (gprb == 0) {
        if (code.oe) unknown("divwuo");
        thread.gpr[code.rd] = 0;
    }
    else {
        thread.gpr[code.rd] = gpra / gprb;
    }
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}

void Interpreter::eqvx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = ~(thread.gpr[code.rs] ^ thread.gpr[code.rb]);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::extsbx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = (s64)(s8)thread.gpr[code.rs];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::extshx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = (s64)(s16)thread.gpr[code.rs];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::extswx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = (s64)(s32)thread.gpr[code.rs];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::mulhdx(Instruction code, PPUThread& thread)
{
    const s64 a = thread.gpr[code.ra];
    const s64 b = thread.gpr[code.rb];
#if defined(NUCLEUS_PLATFORM_WINDOWS)
    thread.gpr[code.rd] = __mulh(a, b);
#elif defined(NUCLEUS_PLATFORM_LINUX) || defined(NUCLEUS_PLATFORM_MACOS)
    __asm__("mulq %[b]" : "=d" (thread.gpr[code.rd]) : [a] "a" (a), [b] "rm" (b));
#endif
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}

void Interpreter::mulhdux(Instruction code, PPUThread& thread)
{
    const u64 a = thread.gpr[code.ra];
    const u64 b = thread.gpr[code.rb];
#if defined(NUCLEUS_PLATFORM_WINDOWS)
    thread.gpr[code.rd] = __umulh(a, b);
#elif defined(NUCLEUS_PLATFORM_LINUX) || defined(NUCLEUS_PLATFORM_MACOS)
    __asm__("imulq %[b]" : "=d" (thread.gpr[code.rd]) : [a] "a" (a), [b] "rm" (b));
#endif
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}

void Interpreter::mulhwx(Instruction code, PPUThread& thread)
{
    s32 a = thread.gpr[code.ra];
    s32 b = thread.gpr[code.rb];
    thread.gpr[code.rd] = ((s64)a * (s64)b) >> 32;
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}

void Interpreter::mulhwux(Instruction code, PPUThread& thread)
{
    u32 a = thread.gpr[code.ra];
    u32 b = thread.gpr[code.rb];
    thread.gpr[code.rd] = ((u64)a * (u64)b) >> 32;
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}

void Interpreter::mulldx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = (s64)((s64)thread.gpr[code.ra] * (s64)thread.gpr[code.rb]);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
    if (code.oe) unknown("mulldo");
}

void Interpreter::mulli(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = (s64)thread.gpr[code.ra] * code.simm;
}

void Interpreter::mullwx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = (s64)((s64)(s32)thread.gpr[code.ra] * (s64)(s32)thread.gpr[code.rb]);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
    if (code.oe) unknown("mullwo");
}

void Interpreter::nandx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = ~(thread.gpr[code.rs] & thread.gpr[code.rb]);

    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::negx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = 0 - thread.gpr[code.ra];
    if (code.oe) unknown("nego");
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}

void Interpreter::norx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = ~(thread.gpr[code.rs] | thread.gpr[code.rb]);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::orx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] | thread.gpr[code.rb];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::orcx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] | ~thread.gpr[code.rb];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::ori(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] | code.uimm;
}

void Interpreter::oris(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] | (code.uimm << 16);
}

void Interpreter::rldc_lr(Instruction code, PPUThread& thread)
{
    const u32 rotate = thread.gpr[code.rb] & 0x3F;
    const u32 mb = code.mb | (code.mb_ << 5);
    if (code.aa) {
        // rldcrx
        thread.gpr[code.ra] = rotl64(thread.gpr[code.rs], rotate) & rotateMask[0][mb];
    }
    else {
        // rldclx
        thread.gpr[code.ra] = rotl64(thread.gpr[code.rs], rotate) & rotateMask[mb][63];
    }
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::rldicx(Instruction code, PPUThread& thread)
{
    const u32 sh = code.sh | (code.sh_ << 5);
    const u32 mb = code.mb | (code.mb_ << 5);
    thread.gpr[code.ra] = rotl64(thread.gpr[code.rs], sh) & rotateMask[mb][63-sh];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::rldiclx(Instruction code, PPUThread& thread)
{
    const u32 sh = code.sh | (code.sh_ << 5);
    const u32 mb = code.mb | (code.mb_ << 5);
    thread.gpr[code.ra] = rotl64(thread.gpr[code.rs], sh) & rotateMask[mb][63];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::rldicrx(Instruction code, PPUThread& thread)
{
    const u32 sh = code.sh | (code.sh_ << 5);
    const u32 me = code.me_ | (code.me__ << 5);
    thread.gpr[code.ra] = rotl64(thread.gpr[code.rs], sh) & rotateMask[0][me];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::rldimix(Instruction code, PPUThread& thread)
{
    const u32 sh = code.sh | (code.sh_ << 5);
    const u32 mb = code.mb | (code.mb_ << 5);
    const u64 mask = rotateMask[mb][63-sh];
    thread.gpr[code.ra] = (thread.gpr[code.ra] & ~mask) | (rotl64(thread.gpr[code.rs], sh) & mask);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::rlwimix(Instruction code, PPUThread& thread)
{
    const u64 r = rotl32(thread.gpr[code.rs], code.sh);
    const u64 m = rotateMask[32 + code.mb][32 + code.me];
    thread.gpr[code.ra] = (r & m) | (thread.gpr[code.ra] & ~m);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::rlwinmx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = rotl32(thread.gpr[code.rs], code.sh) & rotateMask[32 + code.mb][32 + code.me];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::rlwnmx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = rotl32(thread.gpr[code.rs], thread.gpr[code.rb] & 0x1f) & rotateMask[32 + code.mb][32 + code.me];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::sldx(Instruction code, PPUThread& thread)
{
    const u64 shift = thread.gpr[code.rb] & 0x7F;
    thread.gpr[code.ra] = (shift & 0x40) ? 0 : (thread.gpr[code.rs] << shift);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::slwx(Instruction code, PPUThread& thread)
{
    u32 n = thread.gpr[code.rb] & 0x1f;
    u32 r = rotl32((u32)thread.gpr[code.rs], n);
    u32 m = (thread.gpr[code.rb] & 0x20) ? 0 : rotateMask[32][63 - n];
    thread.gpr[code.ra] = r & m;
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::sradx(Instruction code, PPUThread& thread)
{
    s64 RS = thread.gpr[code.rs];
    u8 shift = thread.gpr[code.rb] & 127;
    if (shift > 63) {
        thread.gpr[code.ra] = 0 - (RS < 0);
        thread.xer.CA = (RS < 0);
    }
    else {
        thread.gpr[code.ra] = RS >> shift;
        thread.xer.CA = (RS < 0) & ((thread.gpr[code.ra] << shift) != RS);
    }
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::sradix(Instruction code, PPUThread& thread)
{
    const s64 rs = thread.gpr[code.rs];
    const u32 sh = code.sh | (code.sh_ << 5);
    thread.gpr[code.ra] = rs >> sh;
    thread.xer.CA = (rs < 0) & ((thread.gpr[code.ra] << sh) != rs);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::srawx(Instruction code, PPUThread& thread)
{
    s32 gprs = thread.gpr[code.rs];
    u8 shift = thread.gpr[code.rb] & 63;
    if (shift > 31) {
        thread.gpr[code.ra] = 0 - (gprs < 0);
        thread.xer.CA = (gprs < 0);
    }
    else {
        thread.gpr[code.ra] = gprs >> shift;
        thread.xer.CA = (gprs < 0) & ((thread.gpr[code.ra] << shift) != gprs);
    }
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::srawix(Instruction code, PPUThread& thread)
{
    s32 gprs = (u32)thread.gpr[code.rs];
    thread.gpr[code.ra] = gprs >> code.sh;
    thread.xer.CA = (gprs < 0) & ((u32)(thread.gpr[code.ra] << code.sh) != gprs);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::srdx(Instruction code, PPUThread& thread)
{
    const u64 shift = thread.gpr[code.rb] & 0x7F;
    thread.gpr[code.ra] = (shift & 0x40) ? 0 : (thread.gpr[code.rs] >> shift);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::srwx(Instruction code, PPUThread& thread)
{
    u32 n = thread.gpr[code.rb] & 0x1f;
    u32 m = (thread.gpr[code.rb] & 0x20) ? 0 : rotateMask[32 + n][63];
    u32 r = rotl32((u32)thread.gpr[code.rs], 64 - n);
    thread.gpr[code.ra] = r & m;
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::subfx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = thread.gpr[code.rb] - thread.gpr[code.ra];
    if (code.oe) unknown("subfo");
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}

void Interpreter::subfcx(Instruction code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    const s64 gprb = thread.gpr[code.rb];
    thread.gpr[code.rd] = ~gpra + gprb + 1;
    thread.xer.CA = isCarry(~gpra, gprb, 1);
    if (code.oe) unknown("subfco");
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}

void Interpreter::subfex(Instruction code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    const s64 gprb = thread.gpr[code.rb];
    thread.gpr[code.rd] = ~gpra + gprb + thread.xer.CA;
    thread.xer.CA = isCarry(~gpra, gprb, thread.xer.CA);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
    if (code.oe) unknown("subfeo");
}

void Interpreter::subfic(Instruction code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    const u64 IMM = (s64)code.simm;
    thread.gpr[code.rd] = ~gpra + IMM + 1;

    thread.xer.CA = isCarry(~gpra, IMM, 1);
}

void Interpreter::subfmex(Instruction code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    thread.gpr[code.rd] = ~gpra + thread.xer.CA + ~0ULL;
    thread.xer.CA = isCarry(~gpra, thread.xer.CA, ~0ULL);
    if (code.oe) unknown("subfmeo");
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}

void Interpreter::subfzex(Instruction code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    thread.gpr[code.rd] = ~gpra + thread.xer.CA;
    thread.xer.CA = isCarry(~gpra, thread.xer.CA);
    if (code.oe) unknown("subfzeo");
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}

void Interpreter::xorx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] ^ thread.gpr[code.rb];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}

void Interpreter::xori(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] ^ code.uimm;
}

void Interpreter::xoris(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] ^ (code.uimm << 16);
}

}  // namespace ppu
}  // namespace cpu
