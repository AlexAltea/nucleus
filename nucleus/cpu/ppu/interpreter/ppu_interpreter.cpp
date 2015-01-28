/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_interpreter.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/ppu/ppu_tables.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>

#if defined(NUCLEUS_PLATFORM_WINDOWS)
#include <Windows.h>
#elif defined(NUCLEUS_PLATFORM_LINUX)
#define InterlockedCompareExchange(ptr,new_val,old_val)  __sync_val_compare_and_swap(ptr,old_val,new_val)
#define InterlockedCompareExchange64(ptr,new_val,old_val)  __sync_val_compare_and_swap(ptr,old_val,new_val)
#endif

using namespace cpu::ppu;

// PowerPC Rotation-related functions
inline u64 rotl64(const u64 x, const u8 n) { return (x << n) | (x >> (64 - n)); }
inline u64 rotl32(const u32 x, const u8 n) { return rotl64((u64)x | ((u64)x << 32), n); }
inline u16 rotl16(const u16 x, const u8 n) { return (x << n) | (x >> (16 - n)); }
inline u8 rotl8(const u8 x, const u8 n) { return (x << n) | (x >> (8 - n)); }

// Reverse order of bits
u32 bitReverse32(u32 x)
{
    x = ((x >> 1) & 0x55555555) | ((x & 0x55555555) << 1);
    x = ((x >> 2) & 0x33333333) | ((x & 0x33333333) << 2);
    x = ((x >> 4) & 0x0F0F0F0F) | ((x & 0x0F0F0F0F) << 4);
    x = ((x >> 8) & 0x00FF00FF) | ((x & 0x00FF00FF) << 8);
    x = ((x >> 16) & 0xFFFF) | ((x & 0xFFFF) << 16);
    return x;
}

// Return the corresponding FPSCR flags for the FPRF field for a given double value
u32 getFPRFlags(PPU_FPR reg)
{
    const f64 value = reg._f64;
    switch (std::fpclassify(value)) {
    case FP_NAN:        return FPR_FPRF_QNAN;
    case FP_INFINITE:   return std::signbit(value) ? FPR_FPRF_NINF : FPR_FPRF_PINF;
    case FP_SUBNORMAL:  return std::signbit(value) ? FPR_FPRF_ND : FPR_FPRF_PD;
    case FP_ZERO:       return std::signbit(value) ? FPR_FPRF_NZ : FPR_FPRF_PZ;
    default:            return std::signbit(value) ? FPR_FPRF_NN : FPR_FPRF_PN;
    }
}

u64 getTimebase()
{
#ifdef NUCLEUS_PLATFORM_WINDOWS
    static struct PerformanceFreqHolder {
        u64 value;
        PerformanceFreqHolder() {
            LARGE_INTEGER freq;
            QueryPerformanceFrequency(&freq);
            value = freq.QuadPart;
        }
    } freq;

    LARGE_INTEGER cycle;
    QueryPerformanceCounter(&cycle);
    const u64 sec = cycle.QuadPart / freq.value;
    return sec * 79800000 + (cycle.QuadPart % freq.value) * 79800000 / freq.value;
#else
    nucleus.log.error(LOG_CPU, "Could not get the Timebase value");
    return 0;
#endif
}

static u64 rotateMask[64][64];
void initRotateMask()
{
    static bool initialized = false;
    if (initialized) {
        return;
    }
    for (u32 mb = 0; mb < 64; mb++) {
        for (u32 me = 0; me < 64; me++) {
            const u64 mask = (~0ULL >> mb) ^ ((me >= 63) ? 0 : ~0ULL >> (me + 1));
            rotateMask[mb][me] = mb > me ? ~mask : mask;
        }
    }
    initialized = true;
}

PPUInterpreter::PPUInterpreter(PPUThread& thr) : thread(thr)
{
    initRotateMask();
}

void PPUInterpreter::step()
{
    // Callback finished
    if (thread.pc == 0) {
        thread.finishCallback();
        return;
    }

    const Instruction code = { nucleus.memory.read32(thread.pc) };
    const Entry& entry = get_entry(code);

    entry.interpreter(code, thread);
    thread.pc += 4;
}

/**
 * Auxiliary functions
 */
bool PPUInterpreter::isCarry(u64 a, u64 b)
{
    return (a + b) < a;
}

bool PPUInterpreter::isCarry(u64 a, u64 b, u64 c)
{
    return isCarry(a, b) || isCarry(a + b, c);
}

float PPUInterpreter::CheckVSCR_NJ(PPUThread& thread, const f32 v)
{
    if (!thread.vscr.NJ) {
        return v;
    }
    if (std::fpclassify(v) == FP_SUBNORMAL) {
        return std::signbit(v) ? -0.0f : 0.0f;
    }
    return v;
}

bool PPUInterpreter::CheckCondition(PPUThread& thread, u32 bo, u32 bi)
{
    const u8 bo0 = (bo & 0x10) ? 1 : 0;
    const u8 bo1 = (bo & 0x08) ? 1 : 0;
    const u8 bo2 = (bo & 0x04) ? 1 : 0;
    const u8 bo3 = (bo & 0x02) ? 1 : 0;

    if (!bo2) --thread.ctr;

    const u8 ctr_ok = bo2 | ((thread.ctr != 0) ^ bo3);
    const u8 cond_ok = bo0 | (thread.cr.getBit(bi) ^ (~bo1 & 0x1));

    return ctr_ok && cond_ok;
}

u64& PPUInterpreter::GetRegBySPR(PPUThread& thread, u32 spr)
{
    const u32 n = (spr >> 5) | ((spr & 0x1f) << 5);

    switch (n) {
    case 0x001: return thread.xer.XER;
    case 0x008: return thread.lr;
    case 0x009: return thread.ctr;
    }

    //unknown("GetRegBySPR error: Unknown SPR!");
    return thread.xer.XER;
}

/**
 * PPU Instructions
 */
void PPUInterpreter::addx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = thread.gpr[code.ra] + thread.gpr[code.rb];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
    if (code.oe) unknown("addo");
}
void PPUInterpreter::addcx(Instruction code, PPUThread& thread)
{
    const s64 gpra = thread.gpr[code.ra];
    const s64 gprb = thread.gpr[code.rb];
    thread.gpr[code.rd] = gpra + gprb;
    thread.xer.CA = isCarry(gpra, gprb);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
    if (code.oe) unknown("addco");
}
void PPUInterpreter::addex(Instruction code, PPUThread& thread)
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
void PPUInterpreter::addi(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = code.ra ? ((s64)thread.gpr[code.ra] + code.simm) : code.simm;
}
void PPUInterpreter::addic(Instruction code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    thread.gpr[code.rd] = gpra + code.simm;
    thread.xer.CA = isCarry(gpra, code.simm);
}
void PPUInterpreter::addic_(Instruction code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    thread.gpr[code.rd] = gpra + code.simm;
    thread.xer.CA = isCarry(gpra, code.simm);
    thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0);
}
void PPUInterpreter::addis(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = code.ra ? ((s64)thread.gpr[code.ra] + (code.simm << 16)) : (code.simm << 16);
}
void PPUInterpreter::addmex(Instruction code, PPUThread& thread)
{
    const s64 gpra = thread.gpr[code.ra];
    thread.gpr[code.rd] = gpra + thread.xer.CA - 1;
    thread.xer.CA |= gpra != 0;
    if (code.oe) unknown("addmeo");
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}
void PPUInterpreter::addzex(Instruction code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    thread.gpr[code.rd] = gpra + thread.xer.CA;
    thread.xer.CA = isCarry(gpra, thread.xer.CA);
    if (code.oe) unknown("addzeo");
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}
void PPUInterpreter::andx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] & thread.gpr[code.rb];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::andcx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] & ~thread.gpr[code.rb];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::andi_(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] & code.uimm;
    thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0);
}
void PPUInterpreter::andis_(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] & (code.uimm << 16);
    thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0);
}
void PPUInterpreter::bx(Instruction code, PPUThread& thread)
{
    if (code.lk) thread.lr = thread.pc + 4;
    thread.pc = (code.aa ? (code.li << 2) : thread.pc + (code.li << 2)) & ~0x3ULL;
    thread.pc -= 4;
}
void PPUInterpreter::bcx(Instruction code, PPUThread& thread)
{
    if (CheckCondition(thread, code.bo, code.bi)) {
        if (code.lk) thread.lr = thread.pc + 4;
        thread.pc = (code.aa ? (code.bd << 2) : thread.pc + (code.bd << 2)) & ~0x3ULL;
        thread.pc -= 4;
    }
}
void PPUInterpreter::bcctrx(Instruction code, PPUThread& thread)
{
    if (code.bo & 0x10 || thread.cr.getBit(code.bi) == ((code.bo >> 3) & 1)) {
        if (code.lk) thread.lr = thread.pc + 4;
        thread.pc = thread.ctr & ~0x3ULL;
        thread.pc -= 4;
    }
}
void PPUInterpreter::bclrx(Instruction code, PPUThread& thread)
{
    if (CheckCondition(thread, code.bo, code.bi)) {
        const u32 newLR = thread.pc + 4;
        thread.pc = thread.lr & ~0x3ULL;
        thread.pc -= 4;
        if (code.lk) thread.lr = newLR;
    }
}
void PPUInterpreter::cmp(Instruction code, PPUThread& thread)
{
    if (code.l10) {
        thread.cr.updateField(code.crfd, (s64)thread.gpr[code.ra], (s64)thread.gpr[code.rb]);
    } else {
        thread.cr.updateField(code.crfd, (s32)thread.gpr[code.ra], (s32)thread.gpr[code.rb]);
    }
}
void PPUInterpreter::cmpi(Instruction code, PPUThread& thread)
{
    if (code.l10) {
        thread.cr.updateField(code.crfd, (s64)thread.gpr[code.ra], (s64)code.simm);
    } else {
        thread.cr.updateField(code.crfd, (s32)thread.gpr[code.ra], (s32)code.simm);
    }
}
void PPUInterpreter::cmpl(Instruction code, PPUThread& thread)
{
    if (code.l10) {
        thread.cr.updateField(code.crfd, (u64)thread.gpr[code.ra], (u64)thread.gpr[code.rb]);
    } else {
        thread.cr.updateField(code.crfd, (u32)thread.gpr[code.ra], (u32)thread.gpr[code.rb]);
    }
}
void PPUInterpreter::cmpli(Instruction code, PPUThread& thread)
{
    if (code.l10) {
        thread.cr.updateField(code.crfd, (u64)thread.gpr[code.ra], (u64)code.uimm);
    } else {
        thread.cr.updateField(code.crfd, (u32)thread.gpr[code.ra], (u32)code.uimm);
    }
}
void PPUInterpreter::cntlzdx(Instruction code, PPUThread& thread)
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
void PPUInterpreter::cntlzwx(Instruction code, PPUThread& thread)
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
void PPUInterpreter::crand(Instruction code, PPUThread& thread)
{
    const u8 value = thread.cr.getBit(code.crba) & thread.cr.getBit(code.crbb);
    thread.cr.setBit(code.crbd, value);
}
void PPUInterpreter::crandc(Instruction code, PPUThread& thread)
{
    const u8 value = thread.cr.getBit(code.crba) & (1 ^ thread.cr.getBit(code.crbb));
    thread.cr.setBit(code.crbd, value);
}
void PPUInterpreter::creqv(Instruction code, PPUThread& thread)
{
    const u8 value = 1 ^ (thread.cr.getBit(code.crba) ^ thread.cr.getBit(code.crbb));
    thread.cr.setBit(code.crbd, value);
}
void PPUInterpreter::crnand(Instruction code, PPUThread& thread)
{
    const u8 value = 1 ^ (thread.cr.getBit(code.crba) & thread.cr.getBit(code.crbb));
    thread.cr.setBit(code.crbd, value);
}
void PPUInterpreter::crnor(Instruction code, PPUThread& thread)
{
    const u8 value = 1 ^ (thread.cr.getBit(code.crba) | thread.cr.getBit(code.crbb));
    thread.cr.setBit(code.crbd, value);
}
void PPUInterpreter::cror(Instruction code, PPUThread& thread)
{
    const u8 value = thread.cr.getBit(code.crba) | thread.cr.getBit(code.crbb);
    thread.cr.setBit(code.crbd, value);
}
void PPUInterpreter::crorc(Instruction code, PPUThread& thread)
{
    const u8 value = thread.cr.getBit(code.crba) | (1 ^ thread.cr.getBit(code.crbb));
    thread.cr.setBit(code.crbd, value);
}
void PPUInterpreter::crxor(Instruction code, PPUThread& thread)
{
    const u8 value = thread.cr.getBit(code.crba) ^ thread.cr.getBit(code.crbb);
    thread.cr.setBit(code.crbd, value);
}
void PPUInterpreter::dcbf(Instruction code, PPUThread& thread)
{
    //unknown("dcbf", false);
    // TODO: _mm_fence();
}
void PPUInterpreter::dcbst(Instruction code, PPUThread& thread)
{
    //unknown("dcbst", false);
    // TODO: _mm_fence();
}
void PPUInterpreter::dcbt(Instruction code, PPUThread& thread)
{
    //unknown("dcbt", false);
    // TODO: _mm_fence();
}
void PPUInterpreter::dcbtst(Instruction code, PPUThread& thread)
{
    //unknown("dcbtst", false);
    // TODO: _mm_fence();
}
void PPUInterpreter::dcbz(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    void* cache_line = nucleus.memory.ptr(addr & ~127);
    if (cache_line) {
        memset(cache_line, 0, 128);
    }
    // TODO: _mm_fence();
}
void PPUInterpreter::divdx(Instruction code, PPUThread& thread)
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
void PPUInterpreter::divdux(Instruction code, PPUThread& thread)
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
void PPUInterpreter::divwx(Instruction code, PPUThread& thread)
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
void PPUInterpreter::divwux(Instruction code, PPUThread& thread)
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
void PPUInterpreter::dss(Instruction code, PPUThread& thread)
{
    // TODO: _mm_fence();
}
void PPUInterpreter::dst(Instruction code, PPUThread& thread)
{
    // TODO: _mm_fence();
}
void PPUInterpreter::dstst(Instruction code, PPUThread& thread)
{
    // TODO: _mm_fence();
}
void PPUInterpreter::eciwx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read32(addr);
}
void PPUInterpreter::ecowx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write32(addr, thread.gpr[code.rs]);
}
void PPUInterpreter::eieio(Instruction code, PPUThread& thread)
{
    // TODO: _mm_fence();
}
void PPUInterpreter::eqvx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = ~(thread.gpr[code.rs] ^ thread.gpr[code.rb]);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::extsbx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = (s64)(s8)thread.gpr[code.rs];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::extshx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = (s64)(s16)thread.gpr[code.rs];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::extswx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = (s64)(s32)thread.gpr[code.rs];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::icbi(Instruction code, PPUThread& thread)
{
    // Nothing to do in the interpreter.
}
void PPUInterpreter::isync(Instruction code, PPUThread& thread)
{
    // TODO: _mm_fence();
}
void PPUInterpreter::lbz(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = nucleus.memory.read8(code.ra ? thread.gpr[code.ra] + code.d : code.d);
}
void PPUInterpreter::lbzu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    thread.gpr[code.rd] = nucleus.memory.read8(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lbzux(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read8(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lbzx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read8(addr);
}
void PPUInterpreter::ld(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + (code.ds << 2) : (code.ds << 2);
    thread.gpr[code.rd] = nucleus.memory.read64(addr);
}
void PPUInterpreter::ldarx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read64(addr);
    thread.reserve_addr = addr;
    thread.reserve_value = re64(thread.gpr[code.rd]);
}
void PPUInterpreter::ldbrx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = re64(nucleus.memory.read64(addr));
}
void PPUInterpreter::ldu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + (code.ds << 2);
    thread.gpr[code.rd] = nucleus.memory.read64(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::ldux(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read64(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::ldx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read64(addr);
}
void PPUInterpreter::lha(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    thread.gpr[code.rd] = (s64)(s16)nucleus.memory.read16(addr);
}
void PPUInterpreter::lhau(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    thread.gpr[code.rd] = (s64)(s16)nucleus.memory.read16(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lhaux(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = (s64)(s16)nucleus.memory.read16(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lhax(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = (s64)(s16)nucleus.memory.read16(addr);
}
void PPUInterpreter::lhbrx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = re16(nucleus.memory.read16(addr));
}
void PPUInterpreter::lhz(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    thread.gpr[code.rd] = nucleus.memory.read16(addr);
}
void PPUInterpreter::lhzu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    thread.gpr[code.rd] = nucleus.memory.read16(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lhzux(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read16(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lhzx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read16(addr);
}
void PPUInterpreter::lmw(Instruction code, PPUThread& thread)
{
    u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    for (u32 i = code.rd; i < 32; i++) {
        thread.gpr[i] = nucleus.memory.read32(addr);
        addr += 4;
    }
}
void PPUInterpreter::lswi(Instruction code, PPUThread& thread)
{
    u64 ea = code.ra ? thread.gpr[code.ra] : 0;
    u64 n = code.nb ? code.nb : 32;
    u8 reg = code.rd;
    while (n > 0) {
        if (n > 3) {
            thread.gpr[reg] = nucleus.memory.read32(ea);
            ea += 4;
            n -= 4;
        }
        else {
            u32 buf = 0;
            u32 i = 3;
            while (n > 0) {
                n--;
                buf |= nucleus.memory.read8(ea) << (i * 8);
                ea++;
                i--;
            }
            thread.gpr[reg] = buf;
        }
        reg = (reg + 1) % 32;
    }
}
void PPUInterpreter::lswx(Instruction code, PPUThread& thread)
{
    unknown("lswx");
}
void PPUInterpreter::lwa(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = (s64)(s32)nucleus.memory.read32(code.ra ? thread.gpr[code.ra] + (code.ds << 2) : (code.ds << 2));
}
void PPUInterpreter::lwarx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read32(addr);
    thread.reserve_addr = addr;
    thread.reserve_value = re32(thread.gpr[code.rd]);
}
void PPUInterpreter::lwaux(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = (s64)(s32)nucleus.memory.read32(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lwax(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = (s64)(s32)nucleus.memory.read32(addr);
}
void PPUInterpreter::lwbrx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = re32(nucleus.memory.read32(addr));
}
void PPUInterpreter::lwz(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    thread.gpr[code.rd] = nucleus.memory.read32(addr);
}
void PPUInterpreter::lwzu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    thread.gpr[code.rd] = nucleus.memory.read32(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lwzux(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read32(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lwzx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read32(addr);
}
void PPUInterpreter::mcrf(Instruction code, PPUThread& thread)
{
    thread.cr.setField(code.crfd, thread.cr.getField(code.crfs));
}
void PPUInterpreter::mfocrf(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = bitReverse32(thread.cr.CR);
}
void PPUInterpreter::mfspr(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = GetRegBySPR(thread, code.spr);
}
void PPUInterpreter::mftb(Instruction code, PPUThread& thread)
{
    const u32 n = (code.spr >> 5) | ((code.spr & 0x1f) << 5);
    thread.tb = getTimebase();

    switch (n) {
    case 0x10C: thread.gpr[code.rd] = thread.tb; break;
    case 0x10D: thread.gpr[code.rd] = thread.tbu; break;
    default: unknown("mftb"); break;
    }
}
void PPUInterpreter::mtocrf(Instruction code, PPUThread& thread)
{
    if (code.l11) {
        u32 n = 0, count = 0;
        for (int i = 0; i < 8; i++) {
            if (code.crm & (1 << i)) {
                n = i;
                count++;
            }
        }
        if (count == 1) {
            thread.cr.setField(7 - n, (thread.gpr[code.rs] >> (4*n)) & 0xf);
        }
        else {
            thread.cr.CR = 0;
        }
    }
    else {
        u32 mask = 0;
        for (int i = 0; i < 8; i++) {
            if (code.crm & (1 << i)) {
                mask |= (0xF << (i * 4));
            }
        }
        thread.cr.CR = bitReverse32(thread.gpr[code.rs] & mask) | (bitReverse32(thread.cr.CR) & ~mask);
    }
}
void PPUInterpreter::mtspr(Instruction code, PPUThread& thread)
{
    GetRegBySPR(thread, code.spr) = thread.gpr[code.rs];
}
void PPUInterpreter::mulhdx(Instruction code, PPUThread& thread)
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
void PPUInterpreter::mulhdux(Instruction code, PPUThread& thread)
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
void PPUInterpreter::mulhwx(Instruction code, PPUThread& thread)
{
    s32 a = thread.gpr[code.ra];
    s32 b = thread.gpr[code.rb];
    thread.gpr[code.rd] = ((s64)a * (s64)b) >> 32;
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}
void PPUInterpreter::mulhwux(Instruction code, PPUThread& thread)
{
    u32 a = thread.gpr[code.ra];
    u32 b = thread.gpr[code.rb];
    thread.gpr[code.rd] = ((u64)a * (u64)b) >> 32;
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}
void PPUInterpreter::mulldx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = (s64)((s64)thread.gpr[code.ra] * (s64)thread.gpr[code.rb]);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
    if (code.oe) unknown("mulldo");
}
void PPUInterpreter::mulli(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = (s64)thread.gpr[code.ra] * code.simm;
}
void PPUInterpreter::mullwx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = (s64)((s64)(s32)thread.gpr[code.ra] * (s64)(s32)thread.gpr[code.rb]);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
    if (code.oe) unknown("mullwo");
}
void PPUInterpreter::nandx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = ~(thread.gpr[code.rs] & thread.gpr[code.rb]);

    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::negx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = 0 - thread.gpr[code.ra];
    if (code.oe) unknown("nego");
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}
void PPUInterpreter::nop(Instruction code, PPUThread& thread)
{
}
void PPUInterpreter::norx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = ~(thread.gpr[code.rs] | thread.gpr[code.rb]);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::orx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] | thread.gpr[code.rb];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::orcx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] | ~thread.gpr[code.rb];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::ori(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] | code.uimm;
}
void PPUInterpreter::oris(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] | (code.uimm << 16);
}
void PPUInterpreter::rldc_lr(Instruction code, PPUThread& thread)
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
void PPUInterpreter::rldicx(Instruction code, PPUThread& thread)
{
    const u32 sh = code.sh | (code.sh_ << 5);
    const u32 mb = code.mb | (code.mb_ << 5);
    thread.gpr[code.ra] = rotl64(thread.gpr[code.rs], sh) & rotateMask[mb][63-sh];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::rldiclx(Instruction code, PPUThread& thread)
{
    const u32 sh = code.sh | (code.sh_ << 5);
    const u32 mb = code.mb | (code.mb_ << 5);
    thread.gpr[code.ra] = rotl64(thread.gpr[code.rs], sh) & rotateMask[mb][63];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::rldicrx(Instruction code, PPUThread& thread)
{
    const u32 sh = code.sh | (code.sh_ << 5);
    const u32 me = code.me_ | (code.me__ << 5);
    thread.gpr[code.ra] = rotl64(thread.gpr[code.rs], sh) & rotateMask[0][me];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::rldimix(Instruction code, PPUThread& thread)
{
    const u32 sh = code.sh | (code.sh_ << 5);
    const u32 mb = code.mb | (code.mb_ << 5);
    const u64 mask = rotateMask[mb][63-sh];
    thread.gpr[code.ra] = (thread.gpr[code.ra] & ~mask) | (rotl64(thread.gpr[code.rs], sh) & mask);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::rlwimix(Instruction code, PPUThread& thread)
{
    const u64 r = rotl32(thread.gpr[code.rs], code.sh);
    const u64 m = rotateMask[32 + code.mb][32 + code.me];
    thread.gpr[code.ra] = (r & m) | (thread.gpr[code.ra] & ~m);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::rlwinmx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = rotl32(thread.gpr[code.rs], code.sh) & rotateMask[32 + code.mb][32 + code.me];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::rlwnmx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = rotl32(thread.gpr[code.rs], thread.gpr[code.rb] & 0x1f) & rotateMask[32 + code.mb][32 + code.me];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::sc(Instruction code, PPUThread& thread)
{
    switch (code.lev) {
    case 0:
        nucleus.lv2.call(thread);
        break;
    case 1:
        unknown("hvsc");
        break;
    case 2:
        nucleus.lv2.modules.call(thread);
        break;
    default:
        unknown("sc");
    }
}
void PPUInterpreter::sldx(Instruction code, PPUThread& thread)
{
    const u64 shift = thread.gpr[code.rb] & 0x7F;
    thread.gpr[code.ra] = (shift & 0x40) ? 0 : (thread.gpr[code.rs] << shift);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::slwx(Instruction code, PPUThread& thread)
{
    u32 n = thread.gpr[code.rb] & 0x1f;
    u32 r = rotl32((u32)thread.gpr[code.rs], n);
    u32 m = (thread.gpr[code.rb] & 0x20) ? 0 : rotateMask[32][63 - n];
    thread.gpr[code.ra] = r & m;
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::sradx(Instruction code, PPUThread& thread)
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
void PPUInterpreter::sradix(Instruction code, PPUThread& thread)
{
    const s64 rs = thread.gpr[code.rs];
    const u32 sh = code.sh | (code.sh_ << 5);
    thread.gpr[code.ra] = rs >> sh;
    thread.xer.CA = (rs < 0) & ((thread.gpr[code.ra] << sh) != rs);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::srawx(Instruction code, PPUThread& thread)
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
void PPUInterpreter::srawix(Instruction code, PPUThread& thread)
{
    s32 gprs = (u32)thread.gpr[code.rs];
    thread.gpr[code.ra] = gprs >> code.sh;
    thread.xer.CA = (gprs < 0) & ((u32)(thread.gpr[code.ra] << code.sh) != gprs);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::srdx(Instruction code, PPUThread& thread)
{
    const u64 shift = thread.gpr[code.rb] & 0x7F;
    thread.gpr[code.ra] = (shift & 0x40) ? 0 : (thread.gpr[code.rs] >> shift);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::srwx(Instruction code, PPUThread& thread)
{
    u32 n = thread.gpr[code.rb] & 0x1f;
    u32 m = (thread.gpr[code.rb] & 0x20) ? 0 : rotateMask[32 + n][63];
    u32 r = rotl32((u32)thread.gpr[code.rs], 64 - n);
    thread.gpr[code.ra] = r & m;
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::stb(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    nucleus.memory.write8(addr, thread.gpr[code.rs]);
}
void PPUInterpreter::stbu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    nucleus.memory.write8(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::stbux(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    nucleus.memory.write8(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::stbx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write8(addr, thread.gpr[code.rs]);
}
void PPUInterpreter::std(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    nucleus.memory.write64(addr, thread.gpr[code.rs]);
}
void PPUInterpreter::stdcx_(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    if (thread.reserve_addr == addr) {
        bool value = InterlockedCompareExchange((volatile u64*)(nucleus.memory.ptr(addr)), re64(thread.gpr[code.rs]), thread.reserve_value) == thread.reserve_value;
        thread.cr.setBit(PPU_CR::CR_EQ, value);
    }
    else {
        thread.cr.setBit(PPU_CR::CR_EQ, false);
    }
}
void PPUInterpreter::stdu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + (code.ds << 2);
    nucleus.memory.write64(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::stdux(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    nucleus.memory.write64(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::stdx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write64(addr, thread.gpr[code.rs]);
}
void PPUInterpreter::sth(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    nucleus.memory.write16(addr, thread.gpr[code.rs]);
}
void PPUInterpreter::sthbrx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write16(addr, re16((u16)thread.gpr[code.rs]));
}
void PPUInterpreter::sthu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    nucleus.memory.write16(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::sthux(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    nucleus.memory.write16(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::sthx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write16(addr, thread.gpr[code.rs]);
}
void PPUInterpreter::stmw(Instruction code, PPUThread& thread)
{
    u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    for (int i = code.rs; i < 32; i++) {
        nucleus.memory.write32(addr, thread.gpr[i]);
        addr += 4;
    }
}
void PPUInterpreter::stswi(Instruction code, PPUThread& thread)
{
    u64 ea = code.ra ? thread.gpr[code.ra] : 0;
    u64 n = code.nb ? code.nb : 32;
    u8 reg = code.rd;

    while (n > 0) {
        if (n > 3) {
            nucleus.memory.write32(ea, thread.gpr[reg]);
            ea += 4;
            n -= 4;
        }
        else {
            u32 buf = thread.gpr[reg];
            while (n > 0) {
                n--;
                nucleus.memory.write8(ea, (0xFF000000 & buf) >> 24);
                buf <<= 8;
                ea++;
            }
        }
        reg = (reg + 1) % 32;
    }
}
void PPUInterpreter::stswx(Instruction code, PPUThread& thread)
{
    unknown("stwsx");
}
void PPUInterpreter::stw(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    nucleus.memory.write32(addr, thread.gpr[code.rs]);
}
void PPUInterpreter::stwbrx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write32(addr, re32(thread.gpr[code.rs]));
}
void PPUInterpreter::stwcx_(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    if (thread.reserve_addr == addr) {
        bool value = InterlockedCompareExchange((volatile u32*)(nucleus.memory.ptr(addr)), re32((u32)thread.gpr[code.rs]), (u32)thread.reserve_value) == (u32)thread.reserve_value;
        thread.cr.setBit(PPU_CR::CR_EQ, value);
    }
    else {
        thread.cr.setBit(PPU_CR::CR_EQ, false);
    }
}
void PPUInterpreter::stwu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    nucleus.memory.write32(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::stwux(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    nucleus.memory.write32(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::stwx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write32(addr, thread.gpr[code.rs]);
}
void PPUInterpreter::subfx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = thread.gpr[code.rb] - thread.gpr[code.ra];
    if (code.oe) unknown("subfo");
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}
void PPUInterpreter::subfcx(Instruction code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    const s64 gprb = thread.gpr[code.rb];
    thread.gpr[code.rd] = ~gpra + gprb + 1;
    thread.xer.CA = isCarry(~gpra, gprb, 1);
    if (code.oe) unknown("subfco");
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}
void PPUInterpreter::subfex(Instruction code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    const s64 gprb = thread.gpr[code.rb];
    thread.gpr[code.rd] = ~gpra + gprb + thread.xer.CA;
    thread.xer.CA = isCarry(~gpra, gprb, thread.xer.CA);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
    if (code.oe) unknown("subfeo");
}
void PPUInterpreter::subfic(Instruction code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    const u64 IMM = (s64)code.simm;
    thread.gpr[code.rd] = ~gpra + IMM + 1;

    thread.xer.CA = isCarry(~gpra, IMM, 1);
}
void PPUInterpreter::subfmex(Instruction code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    thread.gpr[code.rd] = ~gpra + thread.xer.CA + ~0ULL;
    thread.xer.CA = isCarry(~gpra, thread.xer.CA, ~0ULL);
    if (code.oe) unknown("subfmeo");
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}
void PPUInterpreter::subfzex(Instruction code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    thread.gpr[code.rd] = ~gpra + thread.xer.CA;
    thread.xer.CA = isCarry(~gpra, thread.xer.CA);
    if (code.oe) unknown("subfzeo");
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}
void PPUInterpreter::sync(Instruction code, PPUThread& thread)
{
    // TODO: _mm_fence();
}
void PPUInterpreter::td(Instruction code, PPUThread& thread)
{
    unknown("td");
}
void PPUInterpreter::tdi(Instruction code, PPUThread& thread)
{
    unknown("tdi");
}
void PPUInterpreter::tw(Instruction code, PPUThread& thread)
{
    unknown("tw");
}
void PPUInterpreter::twi(Instruction code, PPUThread& thread)
{
    unknown("twi");
}
void PPUInterpreter::xorx(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] ^ thread.gpr[code.rb];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::xori(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] ^ code.uimm;
}
void PPUInterpreter::xoris(Instruction code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] ^ (code.uimm << 16);
}

// Floating-Point Unit instructions
void PPUInterpreter::lfd(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    thread.fpr[code.frd]._u64 = nucleus.memory.read64(addr);
}
void PPUInterpreter::lfdu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    thread.fpr[code.frd]._u64 = nucleus.memory.read64(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lfdux(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    thread.fpr[code.frd]._u64 = nucleus.memory.read64(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lfdx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.fpr[code.frd]._u64 = nucleus.memory.read64(addr);
}
void PPUInterpreter::lfs(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    const u32 value = nucleus.memory.read32(addr);
    thread.fpr[code.frd]._f64 = (f32&)value;
}
void PPUInterpreter::lfsu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    const u32 value = nucleus.memory.read32(addr);
    thread.fpr[code.frd]._f64 = (f32&)value;
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lfsux(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    const u32 value = nucleus.memory.read32(addr);
    thread.fpr[code.frd]._f64 = (f32&)value;
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lfsx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const u32 value = nucleus.memory.read32(addr);
    thread.fpr[code.frd]._f64 = (f32&)value;
}
void PPUInterpreter::fabsx(Instruction code, PPUThread& thread)
{
    const f64 value = thread.fpr[code.frb]._f64;
    thread.fpr[code.frd]._f64 = (value < 0.0) ? -value : value;
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::faddx(Instruction code, PPUThread& thread)
{
    const f64 a = thread.fpr[code.fra]._f64;
    const f64 b = thread.fpr[code.frb]._f64;
    const f64 t = a + b;
    if (a != a) {
        thread.fpr[code.frd]._f64 = a;
    } else if (b != b) {
        thread.fpr[code.frd]._f64 = b;
    } else if (t != t) {
        thread.fpscr.setException(FPSCR_VXISI);
        (u64&)thread.fpr[code.frd] = 0x7FF8000000000000ULL;
    } else {
        thread.fpr[code.frd]._f64 = t;
    }
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPRF >> 28); }
}
void PPUInterpreter::faddsx(Instruction code, PPUThread& thread)
{
    const f64 a = thread.fpr[code.fra]._f64;
    const f64 b = thread.fpr[code.frb]._f64;
    const f64 t = a + b;
    if (a != a) {
        thread.fpr[code.frd]._f64 = a;
    } else if (b != b) {
        thread.fpr[code.frd]._f64 = b;
    } else if (t != t) {
        thread.fpscr.setException(FPSCR_VXISI);
        (u64&)thread.fpr[code.frd] = 0x7FF8000000000000ULL;
    } else {
        thread.fpr[code.frd]._f64 = t;
    }
    thread.fpr[code.frd]._f64 = static_cast<f32>(thread.fpr[code.frd]._f64);
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fcfidx(Instruction code, PPUThread& thread)
{
    const s64 bi = (s64&)thread.fpr[code.frb];
    const f64 bf = (f64)bi;
    const s64 bfi = (s64)bf;
    if (bi == bfi) {
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    } else {
        thread.fpscr.FI = 1;
        thread.fpscr.setException(FPSCR_XX);
        thread.fpscr.FR = abs(bfi) > abs(bi);
    }
    thread.fpr[code.frd]._f64 = bf;
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fcmpo(Instruction code, PPUThread& thread)
{
    const int compareResult = PPU_FPR::compare(thread.fpr[code.fra], thread.fpr[code.frb]);
    if (compareResult == PPU_CR::CR_SO) {
        if (thread.fpr[code.fra].isSNaN() || thread.fpr[code.frb].isSNaN()) {
            thread.fpscr.setException(FPSCR_VXSNAN);
            if (!thread.fpscr.VE) {
                thread.fpscr.setException(FPSCR_VXVC);
            }
        }
        else {
            thread.fpscr.setException(FPSCR_VXVC);
        }
        thread.fpscr.FX = 1;
    }
    thread.fpscr.FPRF = (1 << (3 - compareResult));
    thread.cr.setField(code.crfd, 1 << compareResult);
}
void PPUInterpreter::fcmpu(Instruction code, PPUThread& thread)
{
    const int compareResult = PPU_FPR::compare(thread.fpr[code.fra], thread.fpr[code.frb]);
    if (compareResult == PPU_CR::CR_SO) {
        if (std::isnan(thread.fpr[code.fra]._f64) || std::isnan(thread.fpr[code.frb]._f64)) {
            thread.fpscr.setException(FPSCR_VXSNAN);
        }
    }
    thread.fpscr.FPRF = (1 << (3 - compareResult));
    thread.cr.setField(code.crfd, 1 << compareResult);
}
void PPUInterpreter::fctidx(Instruction code, PPUThread& thread)
{
    const f64 b = thread.fpr[code.frb]._f64;
    u64 r;
    if (b > (f64)0x7FFFFFFFFFFFFFFF) {
        r = 0x7FFFFFFFFFFFFFFF;
        if (thread.fpscr.VXCVI) {
            thread.fpscr.FX = 1;
        }
        thread.fpscr.VXCVI = 1;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    }
    else if (b < -(f64)0x8000000000000000) {
        r = 0x8000000000000000;
        if (thread.fpscr.VXCVI) {
            thread.fpscr.FX = 1;
        }
        thread.fpscr.VXCVI = 1;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    }
    else {
        s64 i = 0;
        switch (thread.fpscr.RN) {
        case FPSCR_RN_NEAR:
            {
                f64 t = b + 0.5;
                i = (s64)t;
                if (t - i < 0 || (t - i == 0 && b > 0)) i--;
                break;
            }
        case FPSCR_RN_ZERO:
            i = (s64)b;
            break;
        case FPSCR_RN_PINF:
            i = (s64)b;
            if (b - i > 0) i++;
            break;
        case FPSCR_RN_MINF:
            i = (s64)b;
            if (b - i < 0) i--;
            break;
        }
        r = (u64)i;
        f64 di = i;
        if (di == b) {
            thread.fpscr.FI = 0;
            thread.fpscr.FR = 0;
        }
        else {
            thread.fpscr.FI = 1;
            thread.fpscr.FR = fabs(di) > fabs(b);
        }
    }

    thread.fpr[code.frd]._u64 = r;
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fctidzx(Instruction code, PPUThread& thread)
{
    const f64 b = thread.fpr[code.frb]._f64;
    u64 r;
    if (b > (f64)0x7FFFFFFFFFFFFFFF) {
        r = 0x7FFFFFFFFFFFFFFF;
        if (thread.fpscr.VXCVI) {
            thread.fpscr.FX = 1;
        }
        thread.fpscr.VXCVI = 1;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    }
    else if (b < -(f64)0x8000000000000000) {
        r = 0x8000000000000000;
        if (thread.fpscr.VXCVI) {
            thread.fpscr.FX = 1;
        }
        thread.fpscr.VXCVI = 1;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    }
    else {
        s64 i = (s64)b;
        f64 di = i;
        if (di == b) {
            thread.fpscr.FI = 0;
            thread.fpscr.FR = 0;
        }
        else {
            thread.fpscr.FI = 1;
            thread.fpscr.FR = fabs(di) > fabs(b);
        }
        r = (u64)i;
    }

    thread.fpr[code.frd]._u64 = r;
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fctiwx(Instruction code, PPUThread& thread)
{
    const f64 b = thread.fpr[code.frb]._f64;
    u32 r;
    if (b > (f64)0x7FFFFFFF) {
        r = 0x7FFFFFFF;
        if (thread.fpscr.VXCVI) {
            thread.fpscr.FX = 1;
        }
        thread.fpscr.VXCVI = 1;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    }
    else if (b < -(f64)0x80000000) {
        r = 0x80000000;
        if (thread.fpscr.VXCVI) {
            thread.fpscr.FX = 1;
        }
        thread.fpscr.VXCVI = 1;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    }
    else {
        s32 i = 0;
        switch (thread.fpscr.RN)
        {
        case FPSCR_RN_NEAR:
            {
                f64 t = b + 0.5;
                i = (s32)t;
                if (t - i < 0 || (t - i == 0 && b > 0)) i--;
                break;
            }
        case FPSCR_RN_ZERO:
            i = (s32)b;
            break;
        case FPSCR_RN_PINF:
            i = (s32)b;
            if (b - i > 0) i++;
            break;
        case FPSCR_RN_MINF:
            i = (s32)b;
            if (b - i < 0) i--;
            break;
        }
        r = (u32)i;
        f64 di = i;
        if (di == b) {
            thread.fpscr.FI = 0;
            thread.fpscr.FR = 0;
        }
        else {
            thread.fpscr.FI = 1;
            thread.fpscr.FR = fabs(di) > fabs(b);
        }
    }

    (u64&)thread.fpr[code.frd] = r;
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fctiwzx(Instruction code, PPUThread& thread)
{
    const f64 b = thread.fpr[code.frb]._f64;
    u32 value;
    if (b > (f64)0x7FFFFFFF) {
        value = 0x7FFFFFFF;
        if (thread.fpscr.VXCVI) thread.fpscr.FX = 1;
        thread.fpscr.VXCVI = 1;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    }
    else if (b < -(f64)0x80000000) {
        value = 0x80000000;
        if (thread.fpscr.VXCVI) thread.fpscr.FX = 1;
        thread.fpscr.VXCVI = 1;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    }
    else {
        s32 i = (s32)b;
        f64 di = i;
        if (di == b) {
            thread.fpscr.FI = 0;
            thread.fpscr.FR = 0;
        }
        else {
            thread.fpscr.FI = 1;
            thread.fpscr.FR = fabs(di) > fabs(b);
        }
        value = (u32)i;
    }

    (u64&)thread.fpr[code.frd] = value;
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fdivx(Instruction code, PPUThread& thread)
{
    if (std::isnan(thread.fpr[code.fra]._f64)) {
        thread.fpr[code.frd] = thread.fpr[code.fra];
    }
    else if (std::isnan(thread.fpr[code.frb]._f64)) {
        thread.fpr[code.frd] = thread.fpr[code.frb];
    }
    else {
        if (thread.fpr[code.frb] == 0.0) {
            if (thread.fpr[code.fra] == 0.0) {
                thread.fpscr.VXZDZ = 1;
                thread.fpr[code.frd]._u64 = PPU_FPR::FPR_NAN;
            }
            else {
                thread.fpr[code.frd]._f64 = thread.fpr[code.fra]._f64 / thread.fpr[code.frb]._f64;
            }
            thread.fpscr.setException(FPSCR_ZX);
        }
        else if (thread.fpr[code.fra].isInf() && thread.fpr[code.frb].isInf()) {
            thread.fpscr.VXIDI = 1;
            thread.fpr[code.frd]._u64 = PPU_FPR::FPR_NAN;
        }
        else {
            thread.fpr[code.frd]._f64 = thread.fpr[code.fra]._f64 / thread.fpr[code.frb]._f64;
        }
    }
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fdivsx(Instruction code, PPUThread& thread)
{
    if (std::isnan(thread.fpr[code.fra]._f64)) {
        thread.fpr[code.frd] = thread.fpr[code.fra];
    }
    else if (std::isnan(thread.fpr[code.frb]._f64)) {
        thread.fpr[code.frd] = thread.fpr[code.frb];
    }
    else {
        if (thread.fpr[code.frb] == 0.0) {
            if (thread.fpr[code.fra] == 0.0) {
                thread.fpscr.VXZDZ = true;
                thread.fpr[code.frd]._u64 = PPU_FPR::FPR_NAN;
            }
            else {
                thread.fpr[code.frd]._f64 = (f32)(thread.fpr[code.fra]._f64 / thread.fpr[code.frb]._f64);
            }

            thread.fpscr.ZX = true;
        }
        else if (thread.fpr[code.fra].isInf() && thread.fpr[code.frb].isInf()) {
            thread.fpscr.VXIDI = true;
            thread.fpr[code.frd]._u64 = PPU_FPR::FPR_NAN;
        }
        else {
            thread.fpr[code.frd]._f64 = (f32)(thread.fpr[code.fra]._f64 / thread.fpr[code.frb]._f64);
        }
    }
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fmaddx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = thread.fpr[code.fra]._f64 * thread.fpr[code.frc]._f64 + thread.fpr[code.frb]._f64;
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fmaddsx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = static_cast<f32>(thread.fpr[code.fra]._f64 * thread.fpr[code.frc]._f64 + thread.fpr[code.frb]._f64);
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fmrx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd] = thread.fpr[code.frb];
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fmsubx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = thread.fpr[code.fra]._f64 * thread.fpr[code.frc]._f64 - thread.fpr[code.frb]._f64;
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fmsubsx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = static_cast<f32>(thread.fpr[code.fra]._f64 * thread.fpr[code.frc]._f64 - thread.fpr[code.frb]._f64);
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fmulx(Instruction code, PPUThread& thread)
{
    if ((thread.fpr[code.fra].isInf() && thread.fpr[code.frc] == 0.0) ||
        (thread.fpr[code.frc].isInf() && thread.fpr[code.fra] == 0.0)) {
        thread.fpscr.setException(FPSCR_VXIMZ);
        thread.fpr[code.frd]._u64 = PPU_FPR::FPR_NAN;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
        thread.fpscr.FPRF = FPSCR_FPRF::FPR_FPRF_QNAN;
    }
    else {
        if (thread.fpr[code.fra].isSNaN() || thread.fpr[code.frc].isSNaN()) {
            thread.fpscr.setException(FPSCR_VXSNAN);
        }
        thread.fpr[code.frd]._f64 = thread.fpr[code.fra]._f64 * thread.fpr[code.frc]._f64;
        thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    }
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fmulsx(Instruction code, PPUThread& thread)
{
    const f64 a = thread.fpr[code.fra]._f64;
    const f64 b = thread.fpr[code.frc]._f64;
    const f32 t = a * b;
    if (a != a) {
        thread.fpr[code.frd]._f64 = a;
    } else if (b != b) {
        thread.fpr[code.frd]._f64 = b;
    } else if (t != t) {
        thread.fpscr.setException(FPSCR_VXISI);
        (u64&)thread.fpr[code.frd] = 0x7FF8000000000000ULL;
    } else {
        thread.fpr[code.frd]._f64 = t;
    }
    thread.fpscr.FI = 0;
    thread.fpscr.FR = 0;
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fnabsx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = -::fabs(thread.fpr[code.frb]._f64);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fnegx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = -thread.fpr[code.frb]._f64;
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fnmaddx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = -(thread.fpr[code.fra]._f64 * thread.fpr[code.frc]._f64 + thread.fpr[code.frb]._f64);
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fnmaddsx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = static_cast<f32>(-(thread.fpr[code.fra]._f64 * thread.fpr[code.frc]._f64 + thread.fpr[code.frb]._f64));
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fnmsubx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = -(thread.fpr[code.fra]._f64 * thread.fpr[code.frc]._f64 - thread.fpr[code.frb]._f64);
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fnmsubsx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = static_cast<f32>(-(thread.fpr[code.fra]._f64 * thread.fpr[code.frc]._f64 - thread.fpr[code.frb]._f64));
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fresx(Instruction code, PPUThread& thread)
{
    if (thread.fpr[code.frb] == 0.0) {
        thread.fpscr.setException(FPSCR_ZX);
    }
    thread.fpr[code.frd]._f64 = static_cast<f32>(1.0 / thread.fpr[code.frb]._f64);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::frspx(Instruction code, PPUThread& thread)
{
    const f64 b = thread.fpr[code.frb]._f64;
    f64 b0 = b;
    if (thread.fpscr.NI) {
        if (((u64&)b0 & DOUBLE_EXP) < 0x3800000000000000ULL) {
            (u64&)b0 &= DOUBLE_SIGN;
        }
    }
    const f64 r = static_cast<f32>(b0);
    thread.fpscr.FR = fabs(r) > fabs(b);
    if (b != r) {
        thread.fpscr.FI = 1;
        thread.fpscr.setException(FPSCR_XX);
    } else {
        thread.fpscr.FI = 0;
    }
    thread.fpr[code.frd]._f64 = r;
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
}
void PPUInterpreter::frsqrtex(Instruction code, PPUThread& thread)
{
    if (thread.fpr[code.frb] == 0.0) {
        thread.fpscr.setException(FPSCR_ZX);
    }
    thread.fpr[code.frd]._f64 = 1.0 / sqrt(thread.fpr[code.frb]._f64);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fselx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd] = thread.fpr[code.fra]._f64 >= 0.0 ? thread.fpr[code.frc] : thread.fpr[code.frb];
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fsqrtx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = sqrt(thread.fpr[code.frb]._f64);
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fsqrtsx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = static_cast<f32>(sqrt(thread.fpr[code.frb]._f64));
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fsubx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = thread.fpr[code.fra]._f64 - thread.fpr[code.frb]._f64;
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::fsubsx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = static_cast<f32>(thread.fpr[code.fra]._f64 - thread.fpr[code.frb]._f64);
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::mcrfs(Instruction code, PPUThread& thread)
{
    u64 mask = (1ULL << code.crbd);
    thread.cr.CR &= ~mask;
    thread.cr.CR |= thread.fpscr.FPSCR & mask;
}
void PPUInterpreter::mffsx(Instruction code, PPUThread& thread)
{
    (u64&)thread.fpr[code.frd] = thread.fpscr.FPSCR;
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}
void PPUInterpreter::mtfsb0x(Instruction code, PPUThread& thread)
{
    thread.fpscr.FPSCR &= ~(1ULL << code.crbd);
    if (code.rc) unknown("mtfsb0.");
}
void PPUInterpreter::mtfsb1x(Instruction code, PPUThread& thread)
{
    thread.fpscr.FPSCR |= (1ULL << code.crbd);
    if (code.rc) unknown("mtfsb1.");
}
void PPUInterpreter::mtfsfix(Instruction code, PPUThread& thread)
{
    const u32 mask = 0xF << (code.crfd * 4);
    const u32 value = (code.imm & 0xF) << (code.crfd * 4);
    thread.fpscr.FPSCR &= ~mask;
    thread.fpscr.FPSCR |= value;
    if (code.rc) unknown("mtfsfi.");
}
void PPUInterpreter::mtfsfx(Instruction code, PPUThread& thread)
{
    u32 mask = 0;
    for (int i = 0; i < 8; i++) {
        if (code.fm & (1 << i)) {
            mask |= 0xf << (i * 4);
        }
    }
    thread.fpscr.FPSCR = (thread.fpscr.FPSCR & ~mask) | ((u32&)thread.fpr[code.frb] & mask);
    if (code.rc) unknown("mtfsf.");
}
void PPUInterpreter::stfd(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    nucleus.memory.write64(addr, thread.fpr[code.frs]._u64);
}
void PPUInterpreter::stfdu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    nucleus.memory.write64(addr, thread.fpr[code.frs]._u64);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::stfdux(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    nucleus.memory.write64(addr, thread.fpr[code.frs]._u64);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::stfdx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write64(addr, thread.fpr[code.frs]._u64);
}
void PPUInterpreter::stfiwx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write32(addr, (u32&)thread.fpr[code.frs]);
}
void PPUInterpreter::stfs(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    const f32 value = thread.fpr[code.frs]._f64;
    nucleus.memory.write32(addr, (u32&)value);
}
void PPUInterpreter::stfsu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    const f32 value = thread.fpr[code.frs]._f64;
    nucleus.memory.write32(addr, (u32&)value);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::stfsux(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    const f32 value = thread.fpr[code.frs]._f64;
    nucleus.memory.write32(addr, (u32&)value);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::stfsx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const f32 value = thread.fpr[code.frs]._f64;
    nucleus.memory.write32(addr, (u32&)value);
}

// Vector/SIMD instructions
void PPUInterpreter::lvebx(Instruction code, PPUThread& thread)
{
    const u32 addr = (code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb]) & ~0xfULL;
    thread.vr[code.vd]._u128 = nucleus.memory.read128(addr);
}
void PPUInterpreter::lvehx(Instruction code, PPUThread& thread)
{
    const u32 addr = (code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb]) & ~0xfULL;
    thread.vr[code.vd]._u128 = nucleus.memory.read128(addr);
}
void PPUInterpreter::lvewx(Instruction code, PPUThread& thread)
{
    const u32 addr = (code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb]) & ~0xfULL;
    thread.vr[code.vd]._u128 = nucleus.memory.read128(addr);
}
void PPUInterpreter::lvlx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.readLeft(thread.vr[code.vd]._u8 + eb, addr, 16 - eb);
}
void PPUInterpreter::lvlxl(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.readLeft(thread.vr[code.vd]._u8 + eb, addr, 16 - eb);
}
void PPUInterpreter::lvrx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.readRight(thread.vr[code.vd]._u8, addr & ~0xf, eb);
}
void PPUInterpreter::lvrxl(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.readRight(thread.vr[code.vd]._u8, addr & ~0xf, eb);
}
void PPUInterpreter::lvsl(Instruction code, PPUThread& thread)
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
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.vr[code.vd]._u64[0] = lvsl_values[addr & 0xf][0];
    thread.vr[code.vd]._u64[1] = lvsl_values[addr & 0xf][1];
}
void PPUInterpreter::lvsr(Instruction code, PPUThread& thread)
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
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.vr[code.vd]._u64[0] = lvsr_values[addr & 0xf][0];
    thread.vr[code.vd]._u64[1] = lvsr_values[addr & 0xf][1];
}
void PPUInterpreter::lvx(Instruction code, PPUThread& thread)
{
    const u32 addr = (code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb]) & ~0xfULL;
    thread.vr[code.vd]._u128 = nucleus.memory.read128(addr);
}
void PPUInterpreter::lvxl(Instruction code, PPUThread& thread)
{
    const u32 addr = (code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb]) & ~0xfULL;
    thread.vr[code.vd]._u128 = nucleus.memory.read128(addr);
}
void PPUInterpreter::mfvscr(Instruction code, PPUThread& thread)
{
    thread.vr[code.vd].clear();
    thread.vr[code.vd]._u32[0] = thread.vscr.VSCR;
}
void PPUInterpreter::mtvscr(Instruction code, PPUThread& thread)
{
    thread.vscr.VSCR = thread.vr[code.vb]._u32[0];
}
void PPUInterpreter::stvebx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.write8(addr, thread.vr[code.vs]._u8[15 - eb]);
}
void PPUInterpreter::stvehx(Instruction code, PPUThread& thread)
{
    const u32 addr = (code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb]) & ~1ULL;
    const u8 eb = (addr & 0xf) >> 1;
    nucleus.memory.write16(addr, thread.vr[code.vs]._u16[7 - eb]);
}
void PPUInterpreter::stvewx(Instruction code, PPUThread& thread)
{
    const u32 addr = (code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb]) & ~3ULL;
    const u8 eb = (addr & 0xf) >> 2;
    nucleus.memory.write32(addr, thread.vr[code.vs]._u32[3 - eb]);
}
void PPUInterpreter::stvlx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.writeLeft(addr, thread.vr[code.vs]._u8 + eb, 16 - eb);
}
void PPUInterpreter::stvlxl(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.writeLeft(addr, thread.vr[code.vs]._u8 + eb, 16 - eb);
}
void PPUInterpreter::stvrx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.writeRight(addr - eb, thread.vr[code.vs]._u8, eb);
}
void PPUInterpreter::stvrxl(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.writeRight(addr - eb, thread.vr[code.vs]._u8, eb);
}
void PPUInterpreter::stvx(Instruction code, PPUThread& thread)
{
    const u32 addr = (code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb]) & ~0xfULL;
    nucleus.memory.write128(addr, thread.vr[code.vs]._u128);
}
void PPUInterpreter::stvxl(Instruction code, PPUThread& thread)
{
    const u32 addr = (code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb]) & ~0xfULL;
    nucleus.memory.write128(addr, thread.vr[code.vs]._u128);
}
void PPUInterpreter::vaddcuw(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = ~thread.vr[code.va]._u32[w] < thread.vr[code.vb]._u32[w];
    }
}
void PPUInterpreter::vaddfp(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = thread.vr[code.va]._f32[w] + thread.vr[code.vb]._f32[w];
    }
}
void PPUInterpreter::vaddsbs(Instruction code, PPUThread& thread)
{
    for (u32 b=0; b<16; ++b) {
        const s16 result = (s16)thread.vr[code.va]._s8[b] + (s16)thread.vr[code.vb]._s8[b];
        if (result > 0x7F) {
            thread.vr[code.vd]._s8[b] = 0x7F;
            thread.vscr.SAT = 1;
        }
        else if (result < -0x80) {
            thread.vr[code.vd]._s8[b] = -0x80;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[code.vd]._s8[b] = result;
        }
    }
}
void PPUInterpreter::vaddshs(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        const s32 result = (s32)thread.vr[code.va]._s16[h] + (s32)thread.vr[code.vb]._s16[h];
        if (result > 0x7FFF) {
            thread.vr[code.vd]._s16[h] = 0x7FFF;
            thread.vscr.SAT = 1;
        }
        else if (result < -0x8000) {
            thread.vr[code.vd]._s16[h] = -0x8000;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[code.vd]._s16[h] = result;
        }
    }
}
void PPUInterpreter::vaddsws(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        const s64 result = (s64)thread.vr[code.va]._s32[w] + (s64)thread.vr[code.vb]._s32[w];
        if (result > 0x7FFFFFFF) {
            thread.vr[code.vd]._s32[w] = 0x7FFFFFFF;
            thread.vscr.SAT = 1;
        }
        else if (result < (s32)0x80000000) {
            thread.vr[code.vd]._s32[w] = 0x80000000;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[code.vd]._s32[w] = result;
        }
    }
}
void PPUInterpreter::vaddubm(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = thread.vr[code.va]._u8[b] + thread.vr[code.vb]._u8[b];
    }
}
void PPUInterpreter::vaddubs(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        const u16 result = (u16)thread.vr[code.va]._u8[b] + (u16)thread.vr[code.vb]._u8[b];
        if (result > 0xFF) {
            thread.vr[code.vd]._u8[b] = 0xFF;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[code.vd]._u8[b] = result;
        }
    }
}
void PPUInterpreter::vadduhm(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = thread.vr[code.va]._u16[h] + thread.vr[code.vb]._u16[h];
    }
}
void PPUInterpreter::vadduhs(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        const u32 result = (u32)thread.vr[code.va]._u16[h] + (u32)thread.vr[code.vb]._u16[h];
        if (result > 0xFFFF) {
            thread.vr[code.vd]._u16[h] = 0xFFFF;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[code.vd]._u16[h] = result;
        }
    }
}
void PPUInterpreter::vadduwm(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._u32[w] + thread.vr[code.vb]._u32[w];
    }
}
void PPUInterpreter::vadduws(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        const u64 result = (u64)thread.vr[code.va]._u32[w] + (u64)thread.vr[code.vb]._u32[w];
        if (result > 0xFFFFFFFF) {
            thread.vr[code.vd]._u32[w] = 0xFFFFFFFF;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[code.vd]._u32[w] = result;
        }
    }
}
void PPUInterpreter::vand(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._u32[w] & thread.vr[code.vb]._u32[w];
    }
}
void PPUInterpreter::vandc(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._u32[w] & (~thread.vr[code.vb]._u32[w]);
    }
}
void PPUInterpreter::vavgsb(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._s8[b] = (thread.vr[code.va]._s8[b] + thread.vr[code.vb]._s8[b] + 1) >> 1;
    }
}
void PPUInterpreter::vavgsh(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._s16[h] = (thread.vr[code.va]._s16[h] + thread.vr[code.vb]._s16[h] + 1) >> 1;
    }
}
void PPUInterpreter::vavgsw(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._s32[w] = ((s64)thread.vr[code.va]._s32[w] + (s64)thread.vr[code.vb]._s32[w] + 1) >> 1;
    }
}
void PPUInterpreter::vavgub(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = (thread.vr[code.va]._u8[b] + thread.vr[code.vb]._u8[b] + 1) >> 1;
    }
}
void PPUInterpreter::vavguh(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = (thread.vr[code.va]._u16[h] + thread.vr[code.vb]._u16[h] + 1) >> 1;
    }
}
void PPUInterpreter::vavguw(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = ((u64)thread.vr[code.va]._u32[w] + (u64)thread.vr[code.vb]._u32[w] + 1) >> 1;
    }
}
void PPUInterpreter::vcfsx(Instruction code, PPUThread& thread)
{
    const u32 scale = 1 << code.vuimm;
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = ((f32)thread.vr[code.vb]._s32[w]) / scale;
    }
}
void PPUInterpreter::vcfux(Instruction code, PPUThread& thread)
{
    const u32 scale = 1 << code.vuimm;
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = ((f32)thread.vr[code.vb]._u32[w]) / scale;
    }
}
void PPUInterpreter::vcmpbfp(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        u32 mask = 0;
        const f32 A = CheckVSCR_NJ(thread, thread.vr[code.va]._f32[w]);
        const f32 B = CheckVSCR_NJ(thread, thread.vr[code.vb]._f32[w]);

        if (A >  B) mask |= 1 << 31;
        if (A < -B) mask |= 1 << 30;

        thread.vr[code.vd]._u32[w] = mask;
    }
}
void PPUInterpreter::vcmpbfp_(Instruction code, PPUThread& thread)
{
    bool allInBounds = true;
    for (int w = 0; w < 4; w++) {
        u32 mask = 0;
        const f32 A = CheckVSCR_NJ(thread, thread.vr[code.va]._f32[w]);
        const f32 B = CheckVSCR_NJ(thread, thread.vr[code.vb]._f32[w]);

        if (A >  B) mask |= 1 << 31;
        if (A < -B) mask |= 1 << 30;

        thread.vr[code.vd]._u32[w] = mask;
        if (mask) {
            allInBounds = false;
        }
    }
    thread.cr.setBit(6*4 + 2, allInBounds);
}
void PPUInterpreter::vcmpeqfp(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._f32[w] == thread.vr[code.vb]._f32[w] ? 0xFFFFFFFF : 0;
    }
}
void PPUInterpreter::vcmpeqfp_(Instruction code, PPUThread& thread)
{
    int all_equal = 0x8;
    int none_equal = 0x2;

    for (int w = 0; w < 4; w++) {
        if (thread.vr[code.va]._f32[w] == thread.vr[code.vb]._f32[w]) {
            thread.vr[code.vd]._u32[w] = 0xFFFFFFFF;
            none_equal = 0;
        }
        else {
            thread.vr[code.vd]._u32[w] = 0;
            all_equal = 0;
        }
    }

    thread.cr.setField(6, all_equal | none_equal);
}
void PPUInterpreter::vcmpequb(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = thread.vr[code.va]._u8[b] == thread.vr[code.vb]._u8[b] ? 0xFF : 0;
    }
}
void PPUInterpreter::vcmpequb_(Instruction code, PPUThread& thread)
{
    int all_equal = 0x8;
    int none_equal = 0x2;

    for (int b = 0; b < 16; b++) {
        if (thread.vr[code.va]._u8[b] == thread.vr[code.vb]._u8[b]) {
            thread.vr[code.vd]._u8[b] = 0xFF;
            none_equal = 0;
        }
        else {
            thread.vr[code.vd]._u8[b] = 0;
            all_equal = 0;
        }
    }

    thread.cr.setField(6, all_equal | none_equal);
}
void PPUInterpreter::vcmpequh(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = thread.vr[code.va]._u16[h] == thread.vr[code.vb]._u16[h] ? 0xFFFF : 0;
    }
}
void PPUInterpreter::vcmpequh_(Instruction code, PPUThread& thread)
{
    int all_equal = 0x8;
    int none_equal = 0x2;

    for (int h = 0; h < 8; h++) {
        if (thread.vr[code.va]._u16[h] == thread.vr[code.vb]._u16[h]) {
            thread.vr[code.vd]._u16[h] = 0xFFFF;
            none_equal = 0;
        }
        else {
            thread.vr[code.vd]._u16[h] = 0;
            all_equal = 0;
        }
    }
    thread.cr.setField(6, all_equal | none_equal);
}
void PPUInterpreter::vcmpequw(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._u32[w] == thread.vr[code.vb]._u32[w] ? 0xFFFFFFFF : 0;
    }
}
void PPUInterpreter::vcmpequw_(Instruction code, PPUThread& thread)
{
    int all_equal = 0x8;
    int none_equal = 0x2;

    for (int w = 0; w < 4; w++) {
        if (thread.vr[code.va]._u32[w] == thread.vr[code.vb]._u32[w]) {
            thread.vr[code.vd]._u32[w] = 0xFFFFFFFF;
            none_equal = 0;
        }
        else {
            thread.vr[code.vd]._u32[w] = 0;
            all_equal = 0;
        }
    }

    thread.cr.setField(6, all_equal | none_equal);
}
void PPUInterpreter::vcmpgefp(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._f32[w] >= thread.vr[code.vb]._f32[w] ? 0xFFFFFFFF : 0;
    }
}
void PPUInterpreter::vcmpgefp_(Instruction code, PPUThread& thread)
{
    int all_ge = 0x8;
    int none_ge = 0x2;

    for (int w = 0; w < 4; w++) {
        if (thread.vr[code.va]._f32[w] >= thread.vr[code.vb]._f32[w]) {
            thread.vr[code.vd]._u32[w] = 0xFFFFFFFF;
            none_ge = 0;
        }
        else {
            thread.vr[code.vd]._u32[w] = 0;
            all_ge = 0;
        }
    }

    thread.cr.setField(6, all_ge | none_ge);
}
void PPUInterpreter::vcmpgtfp(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._f32[w] > thread.vr[code.vb]._f32[w] ? 0xFFFFFFFF : 0;
    }
}
void PPUInterpreter::vcmpgtfp_(Instruction code, PPUThread& thread)
{
    int all_ge = 0x8;
    int none_ge = 0x2;

    for (int w = 0; w < 4; w++) {
        if (thread.vr[code.va]._f32[w] > thread.vr[code.vb]._f32[w]) {
            thread.vr[code.vd]._u32[w] = 0xFFFFFFFF;
            none_ge = 0;
        }
        else {
            thread.vr[code.vd]._u32[w] = 0;
            all_ge = 0;
        }
    }

    thread.cr.setField(6, all_ge | none_ge);
}
void PPUInterpreter::vcmpgtsb(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = thread.vr[code.va]._s8[b] > thread.vr[code.vb]._s8[b] ? 0xFF : 0;
    }
}
void PPUInterpreter::vcmpgtsb_(Instruction code, PPUThread& thread)
{
    int all_gt = 0x8;
    int none_gt = 0x2;

    for (int b = 0; b < 16; b++) {
        if (thread.vr[code.va]._s8[b] > thread.vr[code.vb]._s8[b]) {
            thread.vr[code.vd]._u8[b] = 0xFF;
            none_gt = 0;
        }
        else {
            thread.vr[code.vd]._u8[b] = 0;
            all_gt = 0;
        }
    }

    thread.cr.setField(6, all_gt | none_gt);
}
void PPUInterpreter::vcmpgtsh(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = thread.vr[code.va]._s16[h] > thread.vr[code.vb]._s16[h] ? 0xFFFF : 0;
    }
}
void PPUInterpreter::vcmpgtsh_(Instruction code, PPUThread& thread)
{
    int all_gt = 0x8;
    int none_gt = 0x2;

    for (int h = 0; h < 8; h++) {
        if (thread.vr[code.va]._s16[h] > thread.vr[code.vb]._s16[h]) {
            thread.vr[code.vd]._u16[h] = 0xFFFF;
            none_gt = 0;
        }
        else {
            thread.vr[code.vd]._u16[h] = 0;
            all_gt = 0;
        }
    }

    thread.cr.setField(6, all_gt | none_gt);
}
void PPUInterpreter::vcmpgtsw(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._s32[w] > thread.vr[code.vb]._s32[w] ? 0xFFFFFFFF : 0;
    }
}
void PPUInterpreter::vcmpgtsw_(Instruction code, PPUThread& thread)
{
    int all_gt = 0x8;
    int none_gt = 0x2;

    for (int w = 0; w < 4; w++) {
        if (thread.vr[code.va]._s32[w] > thread.vr[code.vb]._s32[w]) {
            thread.vr[code.vd]._u32[w] = 0xFFFFFFFF;
            none_gt = 0;
        }
        else {
            thread.vr[code.vd]._u32[w] = 0;
            all_gt = 0;
        }
    }

    thread.cr.setField(6, all_gt | none_gt);
}
void PPUInterpreter::vcmpgtub(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = thread.vr[code.va]._u8[b] > thread.vr[code.vb]._u8[b] ? 0xFF : 0;
    }
}
void PPUInterpreter::vcmpgtub_(Instruction code, PPUThread& thread)
{
    int all_gt = 0x8;
    int none_gt = 0x2;

    for (int b = 0; b < 16; b++) {
        if (thread.vr[code.va]._u8[b] > thread.vr[code.vb]._u8[b]) {
            thread.vr[code.vd]._u8[b] = 0xFF;
            none_gt = 0;
        }
        else {
            thread.vr[code.vd]._u8[b] = 0;
            all_gt = 0;
        }
    }

    thread.cr.setField(6, all_gt | none_gt);
}
void PPUInterpreter::vcmpgtuh(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = thread.vr[code.va]._u16[h] > thread.vr[code.vb]._u16[h] ? 0xFFFF : 0;
    }
}
void PPUInterpreter::vcmpgtuh_(Instruction code, PPUThread& thread)
{
    int all_gt = 0x8;
    int none_gt = 0x2;

    for (int h = 0; h < 8; h++) {
        if (thread.vr[code.va]._u16[h] > thread.vr[code.vb]._u16[h]) {
            thread.vr[code.vd]._u16[h] = 0xFFFF;
            none_gt = 0;
        }
        else {
            thread.vr[code.vd]._u16[h] = 0;
            all_gt = 0;
        }
    }

    thread.cr.setField(6, all_gt | none_gt);
}
void PPUInterpreter::vcmpgtuw(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._u32[w] > thread.vr[code.vb]._u32[w] ? 0xFFFFFFFF : 0;
    }
}
void PPUInterpreter::vcmpgtuw_(Instruction code, PPUThread& thread)
{
    int all_gt = 0x8;
    int none_gt = 0x2;

    for (int w = 0; w < 4; w++) {
        if (thread.vr[code.va]._u32[w] > thread.vr[code.vb]._u32[w]) {
            thread.vr[code.vd]._u32[w] = 0xFFFFFFFF;
            none_gt = 0;
        }
        else {
            thread.vr[code.vd]._u32[w] = 0;
            all_gt = 0;
        }
    }

    thread.cr.setField(6, all_gt | none_gt);
}
void PPUInterpreter::vctsxs(Instruction code, PPUThread& thread)
{
    int nScale = 1 << code.vuimm;

    for (int w = 0; w < 4; w++) {
        f32 result = thread.vr[code.vb]._f32[w] * nScale;

        if (result > 0x7FFFFFFF)
            thread.vr[code.vd]._s32[w] = 0x7FFFFFFF;
        else if (result < -0x80000000LL)
            thread.vr[code.vd]._s32[w] = -0x80000000LL;
        else {
            thread.vr[code.vd]._s32[w] = (int)result;
        }
    }
}
void PPUInterpreter::vctuxs(Instruction code, PPUThread& thread)
{
    int nScale = 1 << code.vuimm;

    for (int w = 0; w < 4; w++) {
        s64 result = (s64)(thread.vr[code.vb]._f32[w] * nScale);

        if (result > 0xFFFFFFFF)
            thread.vr[code.vd]._u32[w] = 0xFFFFFFFF;
        else if (result < 0)
            thread.vr[code.vd]._u32[w] = 0;
        else
            thread.vr[code.vd]._u32[w] = (u32)result;
    }
}
void PPUInterpreter::vexptefp(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = exp(thread.vr[code.vb]._f32[w] * log(2.0f));
    }
}
void PPUInterpreter::vlogefp(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = log(thread.vr[code.vb]._f32[w]) / log(2.0f);
    }
}
void PPUInterpreter::vmaddfp(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = thread.vr[code.va]._f32[w] * thread.vr[code.vc]._f32[w] + thread.vr[code.vb]._f32[w];
    }
}
void PPUInterpreter::vmaxfp(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = std::max(thread.vr[code.va]._f32[w], thread.vr[code.vb]._f32[w]);
    }
}
void PPUInterpreter::vmaxsb(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++)
        thread.vr[code.vd]._s8[b] = std::max(thread.vr[code.va]._s8[b], thread.vr[code.vb]._s8[b]);
}
void PPUInterpreter::vmaxsh(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._s16[h] = std::max(thread.vr[code.va]._s16[h], thread.vr[code.vb]._s16[h]);
    }
}
void PPUInterpreter::vmaxsw(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._s32[w] = std::max(thread.vr[code.va]._s32[w], thread.vr[code.vb]._s32[w]);
    }
}
void PPUInterpreter::vmaxub(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++)
        thread.vr[code.vd]._u8[b] = std::max(thread.vr[code.va]._u8[b], thread.vr[code.vb]._u8[b]);
}
void PPUInterpreter::vmaxuh(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = std::max(thread.vr[code.va]._u16[h], thread.vr[code.vb]._u16[h]);
    }
}
void PPUInterpreter::vmaxuw(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = std::max(thread.vr[code.va]._u32[w], thread.vr[code.vb]._u32[w]);
    }
}
void PPUInterpreter::vmhaddshs(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        s32 result = (s32)thread.vr[code.va]._s16[h] * (s32)thread.vr[code.vb]._s16[h] + (s32)thread.vr[code.vc]._s16[h];
        if (result > INT16_MAX) {
            thread.vr[code.vd]._s16[h] = (s16)INT16_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < INT16_MIN) {
            thread.vr[code.vd]._s16[h] = (s16)INT16_MIN;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[code.vd]._s16[h] = (s16)result;
        }
    }
}
void PPUInterpreter::vmhraddshs(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        s32 result = (s32)thread.vr[code.va]._s16[h] * (s32)thread.vr[code.vb]._s16[h] + (s32)thread.vr[code.vc]._s16[h] + 0x4000;

        if (result > INT16_MAX) {
            thread.vr[code.vd]._s16[h] = (s16)INT16_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < INT16_MIN) {
            thread.vr[code.vd]._s16[h] = (s16)INT16_MIN;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[code.vd]._s16[h] = (s16)result;
        }
    }
}
void PPUInterpreter::vminfp(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = std::min(thread.vr[code.va]._f32[w], thread.vr[code.vb]._f32[w]);
    }
}
void PPUInterpreter::vminsb(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._s8[b] = std::min(thread.vr[code.va]._s8[b], thread.vr[code.vb]._s8[b]);
    }
}
void PPUInterpreter::vminsh(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._s16[h] = std::min(thread.vr[code.va]._s16[h], thread.vr[code.vb]._s16[h]);
    }
}
void PPUInterpreter::vminsw(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._s32[w] = std::min(thread.vr[code.va]._s32[w], thread.vr[code.vb]._s32[w]);
    }
}
void PPUInterpreter::vminub(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = std::min(thread.vr[code.va]._u8[b], thread.vr[code.vb]._u8[b]);
    }
}
void PPUInterpreter::vminuh(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = std::min(thread.vr[code.va]._u16[h], thread.vr[code.vb]._u16[h]);
    }
}
void PPUInterpreter::vminuw(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = std::min(thread.vr[code.va]._u32[w], thread.vr[code.vb]._u32[w]);
    }
}
void PPUInterpreter::vmladduhm(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = thread.vr[code.va]._u16[h] * thread.vr[code.vb]._u16[h] + thread.vr[code.vc]._u16[h];
    }
}
void PPUInterpreter::vmrghb(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u8[15 - h*2 + 0] = thread.vr[code.va]._u8[15 - h];
        thread.vr[code.vd]._u8[15 - h*2 - 1] = thread.vr[code.vb]._u8[15 - h];
    }
}
void PPUInterpreter::vmrghh(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u16[7 - w*2 + 0] = thread.vr[code.va]._u16[7 - w];
        thread.vr[code.vd]._u16[7 - w*2 - 1] = thread.vr[code.vb]._u16[7 - w];
    }
}
void PPUInterpreter::vmrghw(Instruction code, PPUThread& thread)
{
    for (int d = 0; d < 2; d++) {
        thread.vr[code.vd]._u32[3 - d*2 + 0] = thread.vr[code.va]._u32[3 - d];
        thread.vr[code.vd]._u32[3 - d*2 - 1] = thread.vr[code.vb]._u32[3 - d];
    }
}
void PPUInterpreter::vmrglb(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u8[15 - h*2 + 0] = thread.vr[code.va]._u8[7 - h];
        thread.vr[code.vd]._u8[15 - h*2 - 1] = thread.vr[code.vb]._u8[7 - h];
    }
}
void PPUInterpreter::vmrglh(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u16[7 - w*2] = thread.vr[code.va]._u16[3 - w];
        thread.vr[code.vd]._u16[7 - w*2 - 1] = thread.vr[code.vb]._u16[3 - w];
    }
}
void PPUInterpreter::vmrglw(Instruction code, PPUThread& thread)
{
    for (int d = 0; d < 2; d++) {
        thread.vr[code.vd]._u32[3 - d*2 + 0] = thread.vr[code.va]._u32[1 - d];
        thread.vr[code.vd]._u32[3 - d*2 - 1] = thread.vr[code.vb]._u32[1 - d];
    }
}
void PPUInterpreter::vmsummbm(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        s32 result = 0;
        for (int b = 0; b < 4; b++) {
            result += thread.vr[code.va]._s8[w*4 + b] * thread.vr[code.vb]._u8[w*4 + b];
        }
        result += thread.vr[code.vc]._s32[w];
        thread.vr[code.vd]._s32[w] = result;
    }
}
void PPUInterpreter::vmsumshm(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        s32 result = 0;
        for (int h = 0; h < 2; h++) {
            result += thread.vr[code.va]._s16[w*2 + h] * thread.vr[code.vb]._s16[w*2 + h];
        }
        result += thread.vr[code.vc]._s32[w];
        thread.vr[code.vd]._s32[w] = result;
    }
}
void PPUInterpreter::vmsumshs(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        s64 result = 0;
        for (int h = 0; h < 2; h++) {
            result += thread.vr[code.va]._s16[w*2 + h] * thread.vr[code.vb]._s16[w*2 + h];
        }
        result += thread.vr[code.vc]._s32[w];

        if (result > 0x7FFFFFFF) {
            thread.vr[code.vd]._s32[w] = 0x7FFFFFFF;
            thread.vscr.SAT = 1;
        }
        else if (result < -0x80000000LL) {
            thread.vr[code.vd]._s32[w] = -0x80000000LL;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[code.vd]._s32[w] = (s32)result;
        }
    }
}
void PPUInterpreter::vmsumubm(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        u32 result = 0;
        for (int b = 0; b < 4; b++) {
            result += thread.vr[code.va]._u8[w*4 + b] * thread.vr[code.vb]._u8[w*4 + b];
        }
        result += thread.vr[code.vc]._u32[w];
        thread.vr[code.vd]._u32[w] = result;
    }
}
void PPUInterpreter::vmsumuhm(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        u32 result = 0;

        for (int h = 0; h < 2; h++) {
            result += thread.vr[code.va]._u16[w*2 + h] * thread.vr[code.vb]._u16[w*2 + h];
        }

        result += thread.vr[code.vc]._u32[w];
        thread.vr[code.vd]._u32[w] = result;
    }
}
void PPUInterpreter::vmsumuhs(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        u64 result = 0;
        for (int h = 0; h < 2; h++) {
            result += thread.vr[code.va]._u16[w*2 + h] * thread.vr[code.vb]._u16[w*2 + h];
        }
        result += thread.vr[code.vc]._u32[w];

        if (result > 0xFFFFFFFF) {
            thread.vr[code.vd]._u32[w] = 0xFFFFFFFF;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[code.vd]._u32[w] = (u32)result;
        }
    }
}
void PPUInterpreter::vmulesb(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._s16[h] = (s16)thread.vr[code.va]._s8[h*2+1] * (s16)thread.vr[code.vb]._s8[h*2+1];
    }
}
void PPUInterpreter::vmulesh(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._s32[w] = (s32)thread.vr[code.va]._s16[w*2+1] * (s32)thread.vr[code.vb]._s16[w*2+1];
    }
}
void PPUInterpreter::vmuleub(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = (u16)thread.vr[code.va]._u8[h*2+1] * (u16)thread.vr[code.vb]._u8[h*2+1];
    }
}
void PPUInterpreter::vmuleuh(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = (u32)thread.vr[code.va]._u16[w*2+1] * (u32)thread.vr[code.vb]._u16[w*2+1];
    }
}
void PPUInterpreter::vmulosb(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._s16[h] = (s16)thread.vr[code.va]._s8[h*2] * (s16)thread.vr[code.vb]._s8[h*2];
    }
}
void PPUInterpreter::vmulosh(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._s32[w] = (s32)thread.vr[code.va]._s16[w*2] * (s32)thread.vr[code.vb]._s16[w*2];
    }
}
void PPUInterpreter::vmuloub(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = (u16)thread.vr[code.va]._u8[h*2] * (u16)thread.vr[code.vb]._u8[h*2];
    }
}
void PPUInterpreter::vmulouh(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = (u32)thread.vr[code.va]._u16[w*2] * (u32)thread.vr[code.vb]._u16[w*2];
    }
}
void PPUInterpreter::vnmsubfp(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = -(thread.vr[code.va]._f32[w] * thread.vr[code.vc]._f32[w] - thread.vr[code.vb]._f32[w]);
    }
}
void PPUInterpreter::vnor(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = ~(thread.vr[code.va]._u32[w] | thread.vr[code.vb]._u32[w]);
    }
}
void PPUInterpreter::vor(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._u32[w] | thread.vr[code.vb]._u32[w];
    }
}
void PPUInterpreter::vperm(Instruction code, PPUThread& thread)
{
    u8 tmpSRC[32];
    memcpy(tmpSRC, thread.vr[code.vb]._u8, 16);
    memcpy(tmpSRC + 16, thread.vr[code.va]._u8, 16);

    for (int b = 0; b < 16; b++) {
        u8 index = thread.vr[code.vc]._u8[b] & 0x1f;

        thread.vr[code.vd]._u8[b] = tmpSRC[0x1f - index];
    }
}
void PPUInterpreter::vpkpx(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 4; h++) {
        const u16 bb7  = thread.vr[code.vb]._u8[15 - (h*4 + 0)] & 0x1;
        const u16 bb8  = thread.vr[code.vb]._u8[15 - (h*4 + 1)] >> 3;
        const u16 bb16 = thread.vr[code.vb]._u8[15 - (h*4 + 2)] >> 3;
        const u16 bb24 = thread.vr[code.vb]._u8[15 - (h*4 + 3)] >> 3;
        const u16 ab7  = thread.vr[code.va]._u8[15 - (h*4 + 0)] & 0x1;
        const u16 ab8  = thread.vr[code.va]._u8[15 - (h*4 + 1)] >> 3;
        const u16 ab16 = thread.vr[code.va]._u8[15 - (h*4 + 2)] >> 3;
        const u16 ab24 = thread.vr[code.va]._u8[15 - (h*4 + 3)] >> 3;

        thread.vr[code.vd]._u16[3 - h] = (bb7 << 15) | (bb8 << 10) | (bb16 << 5) | bb24;
        thread.vr[code.vd]._u16[7 - h] = (ab7 << 15) | (ab8 << 10) | (ab16 << 5) | ab24;
    }
}
void PPUInterpreter::vpkshss(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 8; b++) {
        s16 result = thread.vr[code.va]._s16[b];

        if (result > INT8_MAX) {
            result = INT8_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < INT8_MIN) {
            result = INT8_MIN;
            thread.vscr.SAT = 1;
        }

        thread.vr[code.vd]._s8[b+8] = result;

        result = thread.vr[code.vb]._s16[b];

        if (result > INT8_MAX) {
            result = INT8_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < INT8_MIN) {
            result = INT8_MIN;
            thread.vscr.SAT = 1;
        }

        thread.vr[code.vd]._s8[b] = result;
    }
}
void PPUInterpreter::vpkshus(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 8; b++) {
        s16 result = thread.vr[code.va]._s16[b];

        if (result > UINT8_MAX) {
            result = UINT8_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < 0) {
            result = 0;
            thread.vscr.SAT = 1;
        }

        thread.vr[code.vd]._u8[b+8] = result;

        result = thread.vr[code.vb]._s16[b];

        if (result > UINT8_MAX) {
            result = UINT8_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < 0) {
            result = 0;
            thread.vscr.SAT = 1;
        }

        thread.vr[code.vd]._u8[b] = result;
    }
}
void PPUInterpreter::vpkswss(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 4; h++) {
        s32 result = thread.vr[code.va]._s32[h];

        if (result > INT16_MAX) {
            result = INT16_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < INT16_MIN) {
            result = INT16_MIN;
            thread.vscr.SAT = 1;
        }

        thread.vr[code.vd]._s16[h+4] = result;

        result = thread.vr[code.vb]._s32[h];

        if (result > INT16_MAX) {
            result = INT16_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < INT16_MIN) {
            result = INT16_MIN;
            thread.vscr.SAT = 1;
        }

        thread.vr[code.vd]._s16[h] = result;
    }
}
void PPUInterpreter::vpkswus(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 4; h++) {
        s32 result = thread.vr[code.va]._s32[h];

        if (result > UINT16_MAX) {
            result = UINT16_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < 0) {
            result = 0;
            thread.vscr.SAT = 1;
        }

        thread.vr[code.vd]._u16[h+4] = result;

        result = thread.vr[code.vb]._s32[h];

        if (result > UINT16_MAX) {
            result = UINT16_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < 0) {
            result = 0;
            thread.vscr.SAT = 1;
        }

        thread.vr[code.vd]._u16[h] = result;
    }
}
void PPUInterpreter::vpkuhum(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 8; b++) {
        thread.vr[code.vd]._u8[b+8] = thread.vr[code.va]._u8[b*2];
        thread.vr[code.vd]._u8[b  ] = thread.vr[code.vb]._u8[b*2];
    }
}
void PPUInterpreter::vpkuhus(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 8; b++) {
        u16 result = thread.vr[code.va]._u16[b];

        if (result > UINT8_MAX) {
            result = UINT8_MAX;
            thread.vscr.SAT = 1;
        }

        thread.vr[code.vd]._u8[b+8] = result;

        result = thread.vr[code.vb]._u16[b];

        if (result > UINT8_MAX) {
            result = UINT8_MAX;
            thread.vscr.SAT = 1;
        }

        thread.vr[code.vd]._u8[b] = result;
    }
}
void PPUInterpreter::vpkuwum(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 4; h++) {
        thread.vr[code.vd]._u16[h+4] = thread.vr[code.va]._u16[h*2];
        thread.vr[code.vd]._u16[h  ] = thread.vr[code.vb]._u16[h*2];
    }
}
void PPUInterpreter::vpkuwus(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 4; h++) {
        u32 result = thread.vr[code.va]._u32[h];

        if (result > UINT16_MAX) {
            result = UINT16_MAX;
            thread.vscr.SAT = 1;
        }

        thread.vr[code.vd]._u16[h+4] = result;

        result = thread.vr[code.vb]._u32[h];

        if (result > UINT16_MAX) {
            result = UINT16_MAX;
            thread.vscr.SAT = 1;
        }

        thread.vr[code.vd]._u16[h] = result;
    }
}
void PPUInterpreter::vrefp(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = 1.0f / thread.vr[code.vb]._f32[w];
    }
}
void PPUInterpreter::vrfim(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = floor(thread.vr[code.vb]._f32[w]);
    }
}
void PPUInterpreter::vrfin(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = floor(thread.vr[code.vb]._f32[w] + 0.5f);
    }
}
void PPUInterpreter::vrfip(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = ceil(thread.vr[code.vb]._f32[w]);
    }
}
void PPUInterpreter::vrfiz(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        f32 f;
        modff(thread.vr[code.vb]._f32[w], &f);
        thread.vr[code.vd]._f32[w] = f;
    }
}
void PPUInterpreter::vrlb(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        int nRot = thread.vr[code.vb]._u8[b] & 0x7;

        thread.vr[code.vd]._u8[b] = (thread.vr[code.va]._u8[b] << nRot) | (thread.vr[code.va]._u8[b] >> (8 - nRot));
    }
}
void PPUInterpreter::vrlh(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = rotl16(thread.vr[code.va]._u16[h], thread.vr[code.vb]._u8[h*2] & 0xf);
    }
}
void PPUInterpreter::vrlw(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = rotl32(thread.vr[code.va]._u32[w], thread.vr[code.vb]._u8[w*4] & 0x1f);
    }
}
void PPUInterpreter::vrsqrtefp(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = 1.0f / sqrtf(thread.vr[code.vb]._f32[w]);
    }
}
void PPUInterpreter::vsel(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = (thread.vr[code.vb]._u8[b] & thread.vr[code.vc]._u8[b]) | (thread.vr[code.va]._u8[b] & (~thread.vr[code.vc]._u8[b]));
    }
}
void PPUInterpreter::vsl(Instruction code, PPUThread& thread)
{
    u8 sh = thread.vr[code.vb]._u8[0] & 0x7;

    u32 t = 1;

    for (int b = 0; b < 16; b++) {
        t &= (thread.vr[code.vb]._u8[b] & 0x7) == sh;
    }

    if (t) {
        thread.vr[code.vd]._u8[0] = thread.vr[code.va]._u8[0] << sh;

        for (int b = 1; b < 16; b++) {
            thread.vr[code.vd]._u8[b] = (thread.vr[code.va]._u8[b] << sh) | (thread.vr[code.va]._u8[b-1] >> (8 - sh));
        }
    }
}
void PPUInterpreter::vslb(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = thread.vr[code.va]._u8[b] << (thread.vr[code.vb]._u8[b] & 0x7);
    }
}
void PPUInterpreter::vsldoi(Instruction code, PPUThread& thread)
{
    u8 tmpSRC[32];
    memcpy(tmpSRC, thread.vr[code.vb]._u8, 16);
    memcpy(tmpSRC + 16, thread.vr[code.va]._u8, 16);
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[15 - b] = tmpSRC[31 - (b + code.sh)];
    }
}
void PPUInterpreter::vslh(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = thread.vr[code.va]._u16[h] << (thread.vr[code.vb]._u8[h*2] & 0xf);
    }
}
void PPUInterpreter::vslo(Instruction code, PPUThread& thread)
{
    u8 nShift = (thread.vr[code.vb]._u8[0] >> 3) & 0xf;

    thread.vr[code.vd].clear();

    for (u8 b = 0; b < 16 - nShift; b++) {
        thread.vr[code.vd]._u8[15 - b] = thread.vr[code.va]._u8[15 - (b + nShift)];
    }
}
void PPUInterpreter::vslw(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._u32[w] << (thread.vr[code.vb]._u32[w] & 0x1f);
    }
}
void PPUInterpreter::vspltb(Instruction code, PPUThread& thread)
{
    u8 byte = thread.vr[code.vb]._u8[15 - code.vuimm];

    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = byte;
    }
}
void PPUInterpreter::vsplth(Instruction code, PPUThread& thread)
{
    const u16 hword = thread.vr[code.vb]._u16[7 - code.vuimm];
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = hword;
    }
}
void PPUInterpreter::vspltisb(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = code.vsimm;
    }
}
void PPUInterpreter::vspltish(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = (s16)code.vsimm;
    }
}
void PPUInterpreter::vspltisw(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = (s32)code.vsimm;
    }
}
void PPUInterpreter::vspltw(Instruction code, PPUThread& thread)
{
    const u32 word = thread.vr[code.vb]._u32[3 - code.vuimm];
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = word;
    }
}
void PPUInterpreter::vsr(Instruction code, PPUThread& thread)
{
    u8 sh = thread.vr[code.vb]._u8[0] & 0x7;
    u32 t = 1;

    for (int b = 0; b < 16; b++) {
        t &= (thread.vr[code.vb]._u8[b] & 0x7) == sh;
    }
    if (t) {
        thread.vr[code.vd]._u8[15] = thread.vr[code.va]._u8[15] >> sh;

        for (int b = 0; b < 15; b++) {
            thread.vr[code.vd]._u8[14-b] = (thread.vr[code.va]._u8[14-b] >> sh) | (thread.vr[code.va]._u8[14-b+1] << (8 - sh));
        }
    }
}
void PPUInterpreter::vsrab(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._s8[b] = thread.vr[code.va]._s8[b] >> (thread.vr[code.vb]._u8[b] & 0x7);
    }
}
void PPUInterpreter::vsrah(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._s16[h] = thread.vr[code.va]._s16[h] >> (thread.vr[code.vb]._u8[h*2] & 0xf);
    }
}
void PPUInterpreter::vsraw(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._s32[w] = thread.vr[code.va]._s32[w] >> (thread.vr[code.vb]._u8[w*4] & 0x1f);
    }
}
void PPUInterpreter::vsrb(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = thread.vr[code.va]._u8[b] >> (thread.vr[code.vb]._u8[b] & 0x7);
    }
}
void PPUInterpreter::vsrh(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = thread.vr[code.va]._u16[h] >> (thread.vr[code.vb]._u8[h*2] & 0xf);
    }
}
void PPUInterpreter::vsro(Instruction code, PPUThread& thread)
{
    const u8 nShift = (thread.vr[code.vb]._u8[0] >> 3) & 0xf;
    thread.vr[code.vd].clear();

    for (u8 b = 0; b < 16 - nShift; b++) {
        thread.vr[code.vd]._u8[b] = thread.vr[code.va]._u8[b + nShift];
    }
}
void PPUInterpreter::vsrw(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._u32[w] >> (thread.vr[code.vb]._u8[w*4] & 0x1f);
    }
}
void PPUInterpreter::vsubcuw(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._u32[w] < thread.vr[code.vb]._u32[w] ? 0 : 1;
    }
}
void PPUInterpreter::vsubfp(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = thread.vr[code.va]._f32[w] - thread.vr[code.vb]._f32[w];
    }
}
void PPUInterpreter::vsubsbs(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        s16 result = (s16)thread.vr[code.va]._s8[b] - (s16)thread.vr[code.vb]._s8[b];

        if (result < INT8_MIN) {
            thread.vr[code.vd]._s8[b] = INT8_MIN;
            thread.vscr.SAT = 1;
        }
        else if (result > INT8_MAX) {
            thread.vr[code.vd]._s8[b] = INT8_MAX;
            thread.vscr.SAT = 1;
        }
        else
            thread.vr[code.vd]._s8[b] = (s8)result;
    }
}
void PPUInterpreter::vsubshs(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        s32 result = (s32)thread.vr[code.va]._s16[h] - (s32)thread.vr[code.vb]._s16[h];

        if (result < INT16_MIN) {
            thread.vr[code.vd]._s16[h] = (s16)INT16_MIN;
            thread.vscr.SAT = 1;
        }
        else if (result > INT16_MAX) {
            thread.vr[code.vd]._s16[h] = (s16)INT16_MAX;
            thread.vscr.SAT = 1;
        }
        else
            thread.vr[code.vd]._s16[h] = (s16)result;
    }
}
void PPUInterpreter::vsubsws(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        s64 result = (s64)thread.vr[code.va]._s32[w] - (s64)thread.vr[code.vb]._s32[w];

        if (result < INT32_MIN) {
            thread.vr[code.vd]._s32[w] = (s32)INT32_MIN;
            thread.vscr.SAT = 1;
        }
        else if (result > INT32_MAX) {
            thread.vr[code.vd]._s32[w] = (s32)INT32_MAX;
            thread.vscr.SAT = 1;
        }
        else
            thread.vr[code.vd]._s32[w] = (s32)result;
    }
}
void PPUInterpreter::vsububm(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = (u8)((thread.vr[code.va]._u8[b] - thread.vr[code.vb]._u8[b]) & 0xFF);
    }
}
void PPUInterpreter::vsububs(Instruction code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        s16 result = (s16)thread.vr[code.va]._u8[b] - (s16)thread.vr[code.vb]._u8[b];

        if (result < 0) {
            thread.vr[code.vd]._u8[b] = 0;
            thread.vscr.SAT = 1;
        } else {
            thread.vr[code.vd]._u8[b] = (u8)result;
        }
    }
}
void PPUInterpreter::vsubuhm(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = thread.vr[code.va]._u16[h] - thread.vr[code.vb]._u16[h];
    }
}
void PPUInterpreter::vsubuhs(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        s32 result = (s32)thread.vr[code.va]._u16[h] - (s32)thread.vr[code.vb]._u16[h];

        if (result < 0) {
            thread.vr[code.vd]._u16[h] = 0;
            thread.vscr.SAT = 1;
        }
        else
            thread.vr[code.vd]._u16[h] = (u16)result;
    }
}
void PPUInterpreter::vsubuwm(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._u32[w] - thread.vr[code.vb]._u32[w];
    }
}
void PPUInterpreter::vsubuws(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        s64 result = (s64)thread.vr[code.va]._u32[w] - (s64)thread.vr[code.vb]._u32[w];

        if (result < 0) {
            thread.vr[code.vd]._u32[w] = 0;
            thread.vscr.SAT = 1;
        }
        else
            thread.vr[code.vd]._u32[w] = (u32)result;
    }
}
void PPUInterpreter::vsum2sws(Instruction code, PPUThread& thread)
{
    for (int n = 0; n < 2; n++) {
        s64 sum = (s64)thread.vr[code.va]._s32[n*2] + thread.vr[code.va]._s32[n*2 + 1] + thread.vr[code.vb]._s32[n*2];

        if (sum > INT32_MAX) {
            thread.vr[code.vd]._s32[n*2] = (s32)INT32_MAX;
            thread.vscr.SAT = 1;
        }
        else if (sum < INT32_MIN) {
            thread.vr[code.vd]._s32[n*2] = (s32)INT32_MIN;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[code.vd]._s32[n*2] = (s32)sum;
        }
    }
    thread.vr[code.vd]._s32[1] = 0;
    thread.vr[code.vd]._s32[3] = 0;
}
void PPUInterpreter::vsum4sbs(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        s64 sum = thread.vr[code.vb]._s32[w];

        for (int b = 0; b < 4; b++) {
            sum += thread.vr[code.va]._s8[w*4 + b];
        }

        if (sum > INT32_MAX) {
            thread.vr[code.vd]._s32[w] = (s32)INT32_MAX;
            thread.vscr.SAT = 1;
        }
        else if (sum < INT32_MIN) {
            thread.vr[code.vd]._s32[w] = (s32)INT32_MIN;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[code.vd]._s32[w] = (s32)sum;
        }
    }
}
void PPUInterpreter::vsum4shs(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        s64 sum = thread.vr[code.vb]._s32[w];

        for (int h = 0; h < 2; h++) {
            sum += thread.vr[code.va]._s16[w*2 + h];
        }

        if (sum > INT32_MAX) {
            thread.vr[code.vd]._s32[w] = (s32)INT32_MAX;
            thread.vscr.SAT = 1;
        }
        else if (sum < INT32_MIN) {
            thread.vr[code.vd]._s32[w] = (s32)INT32_MIN;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[code.vd]._s32[w] = (s32)sum;
        }
    }
}
void PPUInterpreter::vsum4ubs(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        u64 sum = thread.vr[code.vb]._u32[w];

        for (int b = 0; b < 4; b++) {
            sum += thread.vr[code.va]._u8[w*4 + b];
        }

        if (sum > UINT32_MAX) {
            thread.vr[code.vd]._u32[w] = (u32)UINT32_MAX;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[code.vd]._u32[w] = (u32)sum;
        }
    }
}
void PPUInterpreter::vsumsws(Instruction code, PPUThread& thread)
{
    thread.vr[code.vd].clear();

    s64 sum = thread.vr[code.vb]._s32[3];

    for (int w = 0; w < 4; w++) {
        sum += thread.vr[code.va]._s32[w];
    }

    if (sum > INT32_MAX) {
        thread.vr[code.vd]._s32[0] = (s32)INT32_MAX;
        thread.vscr.SAT = 1;
    }
    else if (sum < INT32_MIN) {
        thread.vr[code.vd]._s32[0] = (s32)INT32_MIN;
        thread.vscr.SAT = 1;
    }
    else
        thread.vr[code.vd]._s32[0] = (s32)sum;
}
void PPUInterpreter::vupkhpx(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._s8[(3 - w)*4 + 3] = thread.vr[code.vb]._s8[w*2 + 0] >> 7;
        thread.vr[code.vd]._u8[(3 - w)*4 + 2] = (thread.vr[code.vb]._u8[w*2 + 0] >> 2) & 0x1f;
        thread.vr[code.vd]._u8[(3 - w)*4 + 1] = ((thread.vr[code.vb]._u8[w*2 + 0] & 0x3) << 3) | ((thread.vr[code.vb]._u8[w*2 + 1] >> 5) & 0x7);
        thread.vr[code.vd]._u8[(3 - w)*4 + 0] = thread.vr[code.vb]._u8[w*2 + 1] & 0x1f;
    }
}
void PPUInterpreter::vupkhsb(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._s16[h] = thread.vr[code.vb]._s8[h];
    }
}
void PPUInterpreter::vupkhsh(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._s32[w] = thread.vr[code.vb]._s16[w];
    }
}
void PPUInterpreter::vupklpx(Instruction code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._s8[(3 - w)*4 + 3] = thread.vr[code.vb]._s8[8 + w*2 + 0] >> 7;
        thread.vr[code.vd]._u8[(3 - w)*4 + 2] = (thread.vr[code.vb]._u8[8 + w*2 + 0] >> 2) & 0x1f;
        thread.vr[code.vd]._u8[(3 - w)*4 + 1] = ((thread.vr[code.vb]._u8[8 + w*2 + 0] & 0x3) << 3) | ((thread.vr[code.vb]._u8[8 + w*2 + 1] >> 5) & 0x7);
        thread.vr[code.vd]._u8[(3 - w)*4 + 0] = thread.vr[code.vb]._u8[8 + w*2 + 1] & 0x1f;
    }
}
void PPUInterpreter::vupklsb(Instruction code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._s16[h] = thread.vr[code.vb]._s8[8 + h];
    }
}
void PPUInterpreter::vupklsh(Instruction code, PPUThread& thread)
{
    thread.vr[code.vd]._s32[0] = thread.vr[code.vb]._s16[4 + 0];
    thread.vr[code.vd]._s32[1] = thread.vr[code.vb]._s16[4 + 1];
    thread.vr[code.vd]._s32[2] = thread.vr[code.vb]._s16[4 + 2];
    thread.vr[code.vd]._s32[3] = thread.vr[code.vb]._s16[4 + 3];
}
void PPUInterpreter::vxor(Instruction code, PPUThread& thread)
{
    thread.vr[code.vd]._u64[0] = thread.vr[code.va]._u64[0] ^ thread.vr[code.vb]._u64[0];
    thread.vr[code.vd]._u32[1] = thread.vr[code.va]._u64[1] ^ thread.vr[code.vb]._u64[1];
}

// Unknown instruction
void PPUInterpreter::unknown(Instruction code, PPUThread& thread)
{
    nucleus.log.error(LOG_CPU, "Unknown instruction: %08X (PC: %08X)", code.instruction, thread.pc);
}
void PPUInterpreter::unknown(const char* instruction)
{
    nucleus.log.error(LOG_CPU, "Unknown instruction: %s", instruction);
}
