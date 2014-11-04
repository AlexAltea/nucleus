/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_interpreter.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/ppu/ppu_tables.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>

#if defined(NUCLEUS_WIN)
#include <Windows.h>
#undef max
#undef min
#elif defined(NUCLEUS_LINUX)
#define InterlockedCompareExchange(ptr,new_val,old_val)  __sync_val_compare_and_swap(ptr,old_val,new_val)
#define InterlockedCompareExchange64(ptr,new_val,old_val)  __sync_val_compare_and_swap(ptr,old_val,new_val)
#endif

// Instruction tables
static bool b_tablesInitialized = false;

// Instruction callers
void PPUInterpreter::callTable4  (PPUFields code, PPUThread& thread) { s_table4[code.op4].interpreter(code, thread); }
void PPUInterpreter::callTable4_ (PPUFields code, PPUThread& thread) { s_table4_[code.op4_].interpreter(code, thread); }
void PPUInterpreter::callTable19 (PPUFields code, PPUThread& thread) { s_table19[code.op19].interpreter(code, thread); }
void PPUInterpreter::callTable30 (PPUFields code, PPUThread& thread) { s_table30[code.op30].interpreter(code, thread); }
void PPUInterpreter::callTable31 (PPUFields code, PPUThread& thread) { s_table31[code.op31].interpreter(code, thread); }
void PPUInterpreter::callTable58 (PPUFields code, PPUThread& thread) { s_table58[code.op58].interpreter(code, thread); }
void PPUInterpreter::callTable59 (PPUFields code, PPUThread& thread) { s_table59[code.op59].interpreter(code, thread); }
void PPUInterpreter::callTable62 (PPUFields code, PPUThread& thread) { s_table62[code.op62].interpreter(code, thread); }
void PPUInterpreter::callTable63 (PPUFields code, PPUThread& thread) { s_table63[code.op63].interpreter(code, thread); }
void PPUInterpreter::callTable63_(PPUFields code, PPUThread& thread) { s_table63_[code.op63_].interpreter(code, thread); }

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
u32 getFPRFlags(f64 value)
{
    switch (std::fpclassify(value)) {
    case FP_NAN:        return FPR_FPRF_QNAN;
    case FP_INFINITE:   return std::signbit(value) ? FPR_FPRF_NINF : FPR_FPRF_PINF;
    case FP_SUBNORMAL:  return std::signbit(value) ? FPR_FPRF_ND : FPR_FPRF_PD;
    case FP_ZERO:       return std::signbit(value) ? FPR_FPRF_NZ : FPR_FPRF_PZ;
    default:            return std::signbit(value) ? FPR_FPRF_NN : FPR_FPRF_PN;
    }
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

    // Initialize PPU Instruction tables (Remove once designated initializers are available in C++)
    if (!b_tablesInitialized) {
        initTables();
        b_tablesInitialized = true;
    }
}

void PPUInterpreter::step()
{
    // Callback finished
    if (thread.pc == 0) {
        thread.finishCallback();
        return;
    }

    const PPUFields code = { nucleus.memory.read32(thread.pc) };
    const PPUInstruction& instruction = s_tablePrimary[code.opcode];

    // Display the current instruction
    //const std::string& disasm = instruction.disassembler(code);
    //printf("%08X : %08X %s\n", thread.pc, code.instruction, disasm.c_str());

    instruction.interpreter(code, thread);
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
void PPUInterpreter::add(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.rd] = thread.gpr[code.ra] + thread.gpr[code.rb];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
    if (code.oe) unknown("addo");
}
void PPUInterpreter::addc(PPUFields code, PPUThread& thread)
{
    const s64 gpra = thread.gpr[code.ra];
    const s64 gprb = thread.gpr[code.rb];
    thread.gpr[code.rd] = gpra + gprb;
    thread.xer.CA = isCarry(gpra, gprb);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
    if (code.oe) unknown("addco");
}
void PPUInterpreter::adde(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::addi(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.rd] = code.ra ? ((s64)thread.gpr[code.ra] + code.simm) : code.simm;
}
void PPUInterpreter::addic(PPUFields code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    thread.gpr[code.rd] = gpra + code.simm;
    thread.xer.CA = isCarry(gpra, code.simm);
}
void PPUInterpreter::addic_(PPUFields code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    thread.gpr[code.rd] = gpra + code.simm;
    thread.xer.CA = isCarry(gpra, code.simm);
    thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0);
}
void PPUInterpreter::addis(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.rd] = code.ra ? ((s64)thread.gpr[code.ra] + (code.simm << 16)) : (code.simm << 16);
}
void PPUInterpreter::addme(PPUFields code, PPUThread& thread)
{
    const s64 gpra = thread.gpr[code.ra];
    thread.gpr[code.rd] = gpra + thread.xer.CA - 1;
    thread.xer.CA |= gpra != 0;
    if (code.oe) unknown("addmeo");
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}
void PPUInterpreter::addze(PPUFields code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    thread.gpr[code.rd] = gpra + thread.xer.CA;
    thread.xer.CA = isCarry(gpra, thread.xer.CA);
    if (code.oe) unknown("addzeo");
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}
void PPUInterpreter::andx(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] & thread.gpr[code.rb];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::andc(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] & ~thread.gpr[code.rb];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::andi_(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] & code.uimm;
    thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0);
}
void PPUInterpreter::andis_(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] & (code.uimm << 16);
    thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0);
}
void PPUInterpreter::b(PPUFields code, PPUThread& thread)
{
    if (code.lk) thread.lr = thread.pc + 4;
    thread.pc = (code.aa ? (code.li << 2) : thread.pc + (code.li << 2)) & ~0x3ULL;
    thread.pc -= 4;
}
void PPUInterpreter::bc(PPUFields code, PPUThread& thread)
{
    if (CheckCondition(thread, code.bo, code.bi)) {
        if (code.lk) thread.lr = thread.pc + 4;
        thread.pc = (code.aa ? code.bd : thread.pc + code.bd) & ~0x3ULL;
        thread.pc -= 4;
    }
}
void PPUInterpreter::bcctr(PPUFields code, PPUThread& thread)
{
    if (code.bo & 0x10 || thread.cr.getBit(code.bi) == ((code.bo >> 3) & 1)) {
        if (code.lk) thread.lr = thread.pc + 4;
        thread.pc = thread.ctr & ~0x3ULL;
        thread.pc -= 4;
    }
}
void PPUInterpreter::bclr(PPUFields code, PPUThread& thread)
{
    if (CheckCondition(thread, code.bo, code.bi)) {
        const u32 newLR = thread.pc + 4;
        thread.pc = thread.lr & ~0x3ULL;
        thread.pc -= 4;
        if (code.lk) thread.lr = newLR;
    }
}
void PPUInterpreter::cmp(PPUFields code, PPUThread& thread)
{
    if (code.l10) {
        thread.cr.updateField(code.crfd, (s64)thread.gpr[code.ra], (s64)thread.gpr[code.rb]);
    } else {
        thread.cr.updateField(code.crfd, (s32)thread.gpr[code.ra], (s32)thread.gpr[code.rb]);
    }
}
void PPUInterpreter::cmpi(PPUFields code, PPUThread& thread)
{
    if (code.l10) {
        thread.cr.updateField(code.crfd, (s64)thread.gpr[code.ra], (s64)code.simm);
    } else {
        thread.cr.updateField(code.crfd, (s32)thread.gpr[code.ra], (s32)code.simm);
    }
}
void PPUInterpreter::cmpl(PPUFields code, PPUThread& thread)
{
    if (code.l10) {
        thread.cr.updateField(code.crfd, (u64)thread.gpr[code.ra], (u64)thread.gpr[code.rb]);
    } else {
        thread.cr.updateField(code.crfd, (u32)thread.gpr[code.ra], (u32)thread.gpr[code.rb]);
    }
}
void PPUInterpreter::cmpli(PPUFields code, PPUThread& thread)
{
    if (code.l10) {
        thread.cr.updateField(code.crfd, (u64)thread.gpr[code.ra], (u64)code.uimm);
    } else {
        thread.cr.updateField(code.crfd, (u32)thread.gpr[code.ra], (u32)code.uimm);
    }
}
void PPUInterpreter::cntlzd(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::cntlzw(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::crand(PPUFields code, PPUThread& thread)
{
    const u8 value = thread.cr.getBit(code.crba) & thread.cr.getBit(code.crbb);
    thread.cr.setBit(code.crbd, value);
}
void PPUInterpreter::crandc(PPUFields code, PPUThread& thread)
{
    const u8 value = thread.cr.getBit(code.crba) & (1 ^ thread.cr.getBit(code.crbb));
    thread.cr.setBit(code.crbd, value);
}
void PPUInterpreter::creqv(PPUFields code, PPUThread& thread)
{
    const u8 value = 1 ^ (thread.cr.getBit(code.crba) ^ thread.cr.getBit(code.crbb));
    thread.cr.setBit(code.crbd, value);
}
void PPUInterpreter::crnand(PPUFields code, PPUThread& thread)
{
    const u8 value = 1 ^ (thread.cr.getBit(code.crba) & thread.cr.getBit(code.crbb));
    thread.cr.setBit(code.crbd, value);
}
void PPUInterpreter::crnor(PPUFields code, PPUThread& thread)
{
    const u8 value = 1 ^ (thread.cr.getBit(code.crba) | thread.cr.getBit(code.crbb));
    thread.cr.setBit(code.crbd, value);
}
void PPUInterpreter::cror(PPUFields code, PPUThread& thread)
{
    const u8 value = thread.cr.getBit(code.crba) | thread.cr.getBit(code.crbb);
    thread.cr.setBit(code.crbd, value);
}
void PPUInterpreter::crorc(PPUFields code, PPUThread& thread)
{
    const u8 value = thread.cr.getBit(code.crba) | (1 ^ thread.cr.getBit(code.crbb));
    thread.cr.setBit(code.crbd, value);
}
void PPUInterpreter::crxor(PPUFields code, PPUThread& thread)
{
    const u8 value = thread.cr.getBit(code.crba) ^ thread.cr.getBit(code.crbb);
    thread.cr.setBit(code.crbd, value);
}
void PPUInterpreter::dcbf(PPUFields code, PPUThread& thread)
{
    //unknown("dcbf", false);
    // TODO: _mm_fence();
}
void PPUInterpreter::dcbst(PPUFields code, PPUThread& thread)
{
    //unknown("dcbst", false);
    // TODO: _mm_fence();
}
void PPUInterpreter::dcbt(PPUFields code, PPUThread& thread)
{
    //unknown("dcbt", false);
    // TODO: _mm_fence();
}
void PPUInterpreter::dcbtst(PPUFields code, PPUThread& thread)
{
    //unknown("dcbtst", false);
    // TODO: _mm_fence();
}
void PPUInterpreter::dcbz(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    void* cache_line = nucleus.memory.ptr(addr & ~127);
    if (cache_line) {
        memset(cache_line, 0, 128);
    }
    // TODO: _mm_fence();
}
void PPUInterpreter::divd(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::divdu(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::divw(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::divwu(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::dss(PPUFields code, PPUThread& thread)
{
    // TODO: _mm_fence();
}
void PPUInterpreter::dst(PPUFields code, PPUThread& thread)
{
    // TODO: _mm_fence();
}
void PPUInterpreter::dstst(PPUFields code, PPUThread& thread)
{
    // TODO: _mm_fence();
}
void PPUInterpreter::eciwx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read32(addr);
}
void PPUInterpreter::ecowx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write32(addr, thread.gpr[code.rs]);
}
void PPUInterpreter::eieio(PPUFields code, PPUThread& thread)
{
    // TODO: _mm_fence();
}
void PPUInterpreter::eqv(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.ra] = ~(thread.gpr[code.rs] ^ thread.gpr[code.rb]);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::extsb(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.ra] = (s64)(s8)thread.gpr[code.rs];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::extsh(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.ra] = (s64)(s16)thread.gpr[code.rs];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::extsw(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.ra] = (s64)(s32)thread.gpr[code.rs];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::icbi(PPUFields code, PPUThread& thread)
{
    // Nothing to do in the interpreter.
}
void PPUInterpreter::isync(PPUFields code, PPUThread& thread)
{
    // TODO: _mm_fence();
}
void PPUInterpreter::lbz(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.rd] = nucleus.memory.read8(code.ra ? thread.gpr[code.ra] + code.d : code.d);
}
void PPUInterpreter::lbzu(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    thread.gpr[code.rd] = nucleus.memory.read8(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lbzux(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read8(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lbzx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read8(addr);
}
void PPUInterpreter::ld(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + (code.ds << 2) : (code.ds << 2);
    thread.gpr[code.rd] = nucleus.memory.read64(addr);
}
void PPUInterpreter::ldarx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read64(addr);
    thread.reserve_addr = addr;
    thread.reserve_value = re64(thread.gpr[code.rd]);
}
void PPUInterpreter::ldbrx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = re64(nucleus.memory.read64(addr));
}
void PPUInterpreter::ldu(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + (code.ds << 2);
    thread.gpr[code.rd] = nucleus.memory.read64(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::ldux(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read64(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::ldx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read64(addr);
}
void PPUInterpreter::lha(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    thread.gpr[code.rd] = (s64)(s16)nucleus.memory.read16(addr);
}
void PPUInterpreter::lhau(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    thread.gpr[code.rd] = (s64)(s16)nucleus.memory.read16(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lhaux(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = (s64)(s16)nucleus.memory.read16(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lhax(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = (s64)(s16)nucleus.memory.read16(addr);
}
void PPUInterpreter::lhbrx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = re16(nucleus.memory.read16(addr));
}
void PPUInterpreter::lhz(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    thread.gpr[code.rd] = nucleus.memory.read16(addr);
}
void PPUInterpreter::lhzu(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    thread.gpr[code.rd] = nucleus.memory.read16(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lhzux(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read16(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lhzx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read16(addr);
}
void PPUInterpreter::lmw(PPUFields code, PPUThread& thread)
{
    u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    for (u32 i = code.rd; i < 32; i++) {
        thread.gpr[i] = nucleus.memory.read32(addr);
        addr += 4;
    }
}
void PPUInterpreter::lswi(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::lswx(PPUFields code, PPUThread& thread)
{
    unknown("lswx");
}
void PPUInterpreter::lwa(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.rd] = (s64)(s32)nucleus.memory.read32(code.ra ? thread.gpr[code.ra] + (code.ds << 2) : (code.ds << 2));
}
void PPUInterpreter::lwarx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read32(addr);
    thread.reserve_addr = addr;
    thread.reserve_value = re32(thread.gpr[code.rd]);
}
void PPUInterpreter::lwaux(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = (s64)(s32)nucleus.memory.read32(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lwax(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = (s64)(s32)nucleus.memory.read32(addr);
}
void PPUInterpreter::lwbrx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = re32(nucleus.memory.read32(addr));
}
void PPUInterpreter::lwz(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    thread.gpr[code.rd] = nucleus.memory.read32(addr);
}
void PPUInterpreter::lwzu(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    thread.gpr[code.rd] = nucleus.memory.read32(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lwzux(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read32(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lwzx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read32(addr);
}
void PPUInterpreter::mcrf(PPUFields code, PPUThread& thread)
{
    thread.cr.setField(code.crfd, thread.cr.getField(code.crfs));
}
void PPUInterpreter::mcrfs(PPUFields code, PPUThread& thread)
{
    u64 mask = (1ULL << code.crbd);
    thread.cr.CR &= ~mask;
    thread.cr.CR |= thread.fpscr.FPSCR & mask;
}
void PPUInterpreter::mfocrf(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.rd] = bitReverse32(thread.cr.CR);
}
void PPUInterpreter::mfspr(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.rd] = GetRegBySPR(thread, code.spr);
}
void PPUInterpreter::mftb(PPUFields code, PPUThread& thread)
{
    const u32 n = (code.spr >> 5) | ((code.spr & 0x1f) << 5);
    switch (n) {
    case 0x10C: thread.gpr[code.rd] = thread.tbl; break;
    case 0x10D: thread.gpr[code.rd] = thread.tbu; break;
    default: unknown("mftb"); break;
    }
}
void PPUInterpreter::mtfsb0(PPUFields code, PPUThread& thread)
{
    thread.fpscr.FPSCR &= ~(1ULL << code.crbd);
    if (code.rc) unknown("mtfsb0.");
}
void PPUInterpreter::mtfsb1(PPUFields code, PPUThread& thread)
{
    thread.fpscr.FPSCR |= (1ULL << code.crbd);
    if (code.rc) unknown("mtfsb1.");
}
void PPUInterpreter::mtfsfi(PPUFields code, PPUThread& thread)
{
    const u64 mask = (0x1ULL << code.crfd);
    if (code.imm) {
        thread.fpscr.FPSCR |= mask;
    }
    else {
        thread.fpscr.FPSCR &= ~mask;
    }
    if (code.rc) unknown("mtfsfi.");
}
void PPUInterpreter::mtocrf(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::mtspr(PPUFields code, PPUThread& thread)
{
    GetRegBySPR(thread, code.spr) = thread.gpr[code.rs];
}
void PPUInterpreter::mulhd(PPUFields code, PPUThread& thread)
{
    const s64 a = thread.gpr[code.ra];
    const s64 b = thread.gpr[code.rb];
#if defined(NUCLEUS_WIN)
    thread.gpr[code.rd] = __mulh(a, b);
#elif defined(NUCLEUS_LINUX) || defined(NUCLEUS_MACOS)
    __asm__("mulq %[b]" : "=d" (thread.gpr[code.rd]) : [a] "a" (a), [b] "rm" (b));
#endif
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}
void PPUInterpreter::mulhdu(PPUFields code, PPUThread& thread)
{
    const u64 a = thread.gpr[code.ra];
    const u64 b = thread.gpr[code.rb];
#if defined(NUCLEUS_WIN)
    thread.gpr[code.rd] = __umulh(a, b);
#elif defined(NUCLEUS_LINUX) || defined(NUCLEUS_MACOS)
    __asm__("imulq %[b]" : "=d" (thread.gpr[code.rd]) : [a] "a" (a), [b] "rm" (b));
#endif
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}
void PPUInterpreter::mulhw(PPUFields code, PPUThread& thread)
{
    s32 a = thread.gpr[code.ra];
    s32 b = thread.gpr[code.rb];
    thread.gpr[code.rd] = ((s64)a * (s64)b) >> 32;
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}
void PPUInterpreter::mulhwu(PPUFields code, PPUThread& thread)
{
    u32 a = thread.gpr[code.ra];
    u32 b = thread.gpr[code.rb];
    thread.gpr[code.rd] = ((u64)a * (u64)b) >> 32;
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}
void PPUInterpreter::mulld(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.rd] = (s64)((s64)thread.gpr[code.ra] * (s64)thread.gpr[code.rb]);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
    if (code.oe) unknown("mulldo");
}
void PPUInterpreter::mulli(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.rd] = (s64)thread.gpr[code.ra] * code.simm;
}
void PPUInterpreter::mullw(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.rd] = (s64)((s64)(s32)thread.gpr[code.ra] * (s64)(s32)thread.gpr[code.rb]);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
    if (code.oe) unknown("mullwo");
}
void PPUInterpreter::nand(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.ra] = ~(thread.gpr[code.rs] & thread.gpr[code.rb]);

    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::neg(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.rd] = 0 - thread.gpr[code.ra];
    if (code.oe) unknown("nego");
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}
void PPUInterpreter::nop(PPUFields code, PPUThread& thread)
{
}
void PPUInterpreter::nor(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.ra] = ~(thread.gpr[code.rs] | thread.gpr[code.rb]);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::orx(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] | thread.gpr[code.rb];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::orc(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] | ~thread.gpr[code.rb];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::ori(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] | code.uimm;
}
void PPUInterpreter::oris(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] | (code.uimm << 16);
}
void PPUInterpreter::rldc_lr(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::rldicx(PPUFields code, PPUThread& thread)
{
    const u32 sh = code.sh | (code.sh_ << 5);
    const u32 mb = code.mb | (code.mb_ << 5);
    thread.gpr[code.ra] = rotl64(thread.gpr[code.rs], sh) & rotateMask[mb][63-sh];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::rldiclx(PPUFields code, PPUThread& thread)
{
    const u32 sh = code.sh | (code.sh_ << 5);
    const u32 mb = code.mb | (code.mb_ << 5);
    thread.gpr[code.ra] = rotl64(thread.gpr[code.rs], sh) & rotateMask[mb][63];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::rldicrx(PPUFields code, PPUThread& thread)
{
    const u32 sh = code.sh | (code.sh_ << 5);
    const u32 me = code.me_ | (code.me__ << 5);
    thread.gpr[code.ra] = rotl64(thread.gpr[code.rs], sh) & rotateMask[0][me];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::rldimix(PPUFields code, PPUThread& thread)
{
    const u32 sh = code.sh | (code.sh_ << 5);
    const u32 mb = code.mb | (code.mb_ << 5);
    const u64 mask = rotateMask[mb][63-sh];
    thread.gpr[code.ra] = (thread.gpr[code.ra] & ~mask) | (rotl64(thread.gpr[code.rs], sh) & mask);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::rlwimix(PPUFields code, PPUThread& thread)
{
    const u64 r = rotl32(thread.gpr[code.rs], code.sh);
    const u64 m = rotateMask[32 + code.mb][32 + code.me];
    thread.gpr[code.ra] = (r & m) | (thread.gpr[code.ra] & ~m);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::rlwinmx(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.ra] = rotl32(thread.gpr[code.rs], code.sh) & rotateMask[32 + code.mb][32 + code.me];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::rlwnmx(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.ra] = rotl32(thread.gpr[code.rs], thread.gpr[code.rb] & 0x1f) & rotateMask[32 + code.mb][32 + code.me];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::sc(PPUFields code, PPUThread& thread)
{
    if (code.sys == 2) {
        nucleus.lv2.call(thread);
    }
    else {
        unknown("sc");
    }
}
void PPUInterpreter::sld(PPUFields code, PPUThread& thread)
{
    const u64 shift = thread.gpr[code.rb] & 0x7F;
    thread.gpr[code.ra] = (shift & 0x40) ? 0 : (thread.gpr[code.rs] << shift);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::slw(PPUFields code, PPUThread& thread)
{
    u32 n = thread.gpr[code.rb] & 0x1f;
    u32 r = rotl32((u32)thread.gpr[code.rs], n);
    u32 m = (thread.gpr[code.rb] & 0x20) ? 0 : rotateMask[32][63 - n];
    thread.gpr[code.ra] = r & m;
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::srad(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::sradi(PPUFields code, PPUThread& thread)
{
    const s64 rs = thread.gpr[code.rs];
    const u32 sh = code.sh | (code.sh_ << 5);
    thread.gpr[code.ra] = rs >> sh;
    thread.xer.CA = (rs < 0) & ((thread.gpr[code.ra] << sh) != rs);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::sraw(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::srawi(PPUFields code, PPUThread& thread)
{
    s32 gprs = (u32)thread.gpr[code.rs];
    thread.gpr[code.ra] = gprs >> code.sh;
    thread.xer.CA = (gprs < 0) & ((u32)(thread.gpr[code.ra] << code.sh) != gprs);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::srd(PPUFields code, PPUThread& thread)
{
    const u64 shift = thread.gpr[code.rb] & 0x7F;
    thread.gpr[code.ra] = (shift & 0x40) ? 0 : (thread.gpr[code.rs] >> shift);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::srw(PPUFields code, PPUThread& thread)
{
    u32 n = thread.gpr[code.rb] & 0x1f;
    u32 m = (thread.gpr[code.rb] & 0x20) ? 0 : rotateMask[32 + n][63];
    u32 r = rotl32((u32)thread.gpr[code.rs], 64 - n);
    thread.gpr[code.ra] = r & m;
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::stb(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    nucleus.memory.write8(addr, thread.gpr[code.rs]);
}
void PPUInterpreter::stbu(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    nucleus.memory.write8(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::stbux(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    nucleus.memory.write8(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::stbx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write8(addr, thread.gpr[code.rs]);
}
void PPUInterpreter::std(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    nucleus.memory.write64(addr, thread.gpr[code.rs]);
}
void PPUInterpreter::stdcx_(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    if (thread.reserve_addr == addr) {
        bool value = InterlockedCompareExchange((volatile u64*)(nucleus.memory.ptr(addr)), re64(thread.gpr[code.rs]), thread.reserve_value) == thread.reserve_value;
        thread.cr.setBit(thread.cr.CR_EQ, value);
    }
    else {
        thread.cr.setBit(thread.cr.CR_EQ, false);
    }
}
void PPUInterpreter::stdu(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + (code.ds << 2);
    nucleus.memory.write64(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::stdux(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    nucleus.memory.write64(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::stdx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write64(addr, thread.gpr[code.rs]);
}
void PPUInterpreter::sth(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    nucleus.memory.write16(addr, thread.gpr[code.rs]);
}
void PPUInterpreter::sthbrx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write16(addr, re16((u16)thread.gpr[code.rs]));
}
void PPUInterpreter::sthu(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    nucleus.memory.write16(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::sthux(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    nucleus.memory.write16(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::sthx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write16(addr, thread.gpr[code.rs]);
}
void PPUInterpreter::stmw(PPUFields code, PPUThread& thread)
{
    u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    for (int i = code.rs; i < 32; i++) {
        nucleus.memory.write32(addr, thread.gpr[i]);
        addr += 4;
    }
}
void PPUInterpreter::stswi(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::stswx(PPUFields code, PPUThread& thread)
{
    unknown("stwsx");
}
void PPUInterpreter::stw(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    nucleus.memory.write32(addr, thread.gpr[code.rs]);
}
void PPUInterpreter::stwbrx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write32(addr, re32(thread.gpr[code.rs]));
}
void PPUInterpreter::stwcx_(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    if (thread.reserve_addr == addr) {
        bool value = InterlockedCompareExchange((volatile u32*)(nucleus.memory.ptr(addr)), re32((u32)thread.gpr[code.rs]), (u32)thread.reserve_value) == (u32)thread.reserve_value;
        thread.cr.setBit(thread.cr.CR_EQ, value);
    }
    else {
        thread.cr.setBit(thread.cr.CR_EQ, false);
    }
}
void PPUInterpreter::stwu(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    nucleus.memory.write32(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::stwux(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    nucleus.memory.write32(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::stwx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write32(addr, thread.gpr[code.rs]);
}
void PPUInterpreter::subf(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.rd] = thread.gpr[code.rb] - thread.gpr[code.ra];
    if (code.oe) unknown("subfo");
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}
void PPUInterpreter::subfc(PPUFields code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    const s64 gprb = thread.gpr[code.rb];
    thread.gpr[code.rd] = ~gpra + gprb + 1;
    thread.xer.CA = isCarry(~gpra, gprb, 1);
    if (code.oe) unknown("subfco");
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}
void PPUInterpreter::subfe(PPUFields code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    const s64 gprb = thread.gpr[code.rb];
    thread.gpr[code.rd] = ~gpra + gprb + thread.xer.CA;
    thread.xer.CA = isCarry(~gpra, gprb, thread.xer.CA);
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
    if (code.oe) unknown("subfeo");
}
void PPUInterpreter::subfic(PPUFields code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    const u64 IMM = (s64)code.simm;
    thread.gpr[code.rd] = ~gpra + IMM + 1;

    thread.xer.CA = isCarry(~gpra, IMM, 1);
}
void PPUInterpreter::subfme(PPUFields code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    thread.gpr[code.rd] = ~gpra + thread.xer.CA + ~0ULL;
    thread.xer.CA = isCarry(~gpra, thread.xer.CA, ~0ULL);
    if (code.oe) unknown("subfmeo");
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}
void PPUInterpreter::subfze(PPUFields code, PPUThread& thread)
{
    const u64 gpra = thread.gpr[code.ra];
    thread.gpr[code.rd] = ~gpra + thread.xer.CA;
    thread.xer.CA = isCarry(~gpra, thread.xer.CA);
    if (code.oe) unknown("subfzeo");
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.rd], (s64)0); }
}
void PPUInterpreter::sync(PPUFields code, PPUThread& thread)
{
    // TODO: _mm_fence();
}
void PPUInterpreter::td(PPUFields code, PPUThread& thread)
{
    unknown("td");
}
void PPUInterpreter::tdi(PPUFields code, PPUThread& thread)
{
    s64 a = thread.gpr[code.ra];

    if ((a < (s64)code.simm  && (code.to & 0x10)) ||
        (a > (s64)code.simm  && (code.to & 0x8))  ||
        (a == (s64)code.simm && (code.to & 0x4))  ||
        ((u64)a < (u64)code.simm && (code.to & 0x2)) ||
        ((u64)a > (u64)code.simm && (code.to & 0x1))) {
        unknown("tdi");
    }
}

void PPUInterpreter::tw(PPUFields code, PPUThread& thread)
{
    s32 a = thread.gpr[code.ra];
    s32 b = thread.gpr[code.rb];

    if ((a < b  && (code.to & 0x10)) ||
        (a > b  && (code.to & 0x8))  ||
        (a == b && (code.to & 0x4))  ||
        ((u32)a < (u32)b && (code.to & 0x2)) ||
        ((u32)a > (u32)b && (code.to & 0x1))) {
        unknown("tw");
    }
}
void PPUInterpreter::twi(PPUFields code, PPUThread& thread)
{
    s32 a = thread.gpr[code.ra];

    if ((a < code.simm  && (code.to & 0x10)) ||
        (a > code.simm  && (code.to & 0x8))  ||
        (a == code.simm && (code.to & 0x4))  ||
        ((u32)a < (u32)code.simm && (code.to & 0x2)) ||
        ((u32)a > (u32)code.simm && (code.to & 0x1))) {
        unknown("twi");
    }
}
void PPUInterpreter::xorx(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] ^ thread.gpr[code.rb];
    if (code.rc) { thread.cr.updateField(0, (s64)thread.gpr[code.ra], (s64)0); }
}
void PPUInterpreter::xori(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] ^ code.uimm;
}
void PPUInterpreter::xoris(PPUFields code, PPUThread& thread)
{
    thread.gpr[code.ra] = thread.gpr[code.rs] ^ (code.uimm << 16);
}

// Floating-Point Unit instructions
void PPUInterpreter::lfd(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    (u64&)thread.fpr[code.frd] = nucleus.memory.read64(addr);
}
void PPUInterpreter::lfdu(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + (code.ds << 2);
    (u64&)thread.fpr[code.frd] = nucleus.memory.read64(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lfdux(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    (u64&)thread.fpr[code.frd] = nucleus.memory.read64(addr);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lfdx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    (u64&)thread.fpr[code.frd] = nucleus.memory.read64(addr);
}
void PPUInterpreter::lfs(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    const u32 value = nucleus.memory.read32(addr);
    thread.fpr[code.frd] = (f32&)value;
}
void PPUInterpreter::lfsu(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + (code.ds << 2);
    const u32 v = nucleus.memory.read32(addr);
    thread.fpr[code.frd] = (f32&)v;
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lfsux(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    (u64&)thread.fpr[code.frd] = nucleus.memory.read32(addr);
    thread.fpr[code.frd] = (f32&)thread.fpr[code.frd];
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::lfsx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    (u32&)thread.fpr[code.frd] = nucleus.memory.read32(addr);
    thread.fpr[code.frd] = (f32&)thread.fpr[code.frd];
}
void PPUInterpreter::fabs(PPUFields code, PPUThread& thread)
{
    const f32 value = thread.fpr[code.frb];
    thread.fpr[code.frd] = (value < 0) ? -value : value;
    if (code.rc) { unknown("fabs."); }
}
void PPUInterpreter::fadd(PPUFields code, PPUThread& thread)
{
    thread.fpr[code.frd] = thread.fpr[code.fra] + thread.fpr[code.frb];
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPRF & 0xF); }
}
void PPUInterpreter::fadds(PPUFields code, PPUThread& thread)
{
    thread.fpr[code.frd] = static_cast<f32>(thread.fpr[code.fra] + thread.fpr[code.frb]);
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { unknown("fadds."); }
}
void PPUInterpreter::fcfid(PPUFields code, PPUThread& thread)
{
    const s64 bi = (s64&)thread.fpr[code.frb];
    const f64 bf = (f64)bi;
    const s64 bfi = (s64)bf;
    if (bi == bfi) {
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    } else {
        thread.fpscr.FI = 1;
        thread.fpscr.FR = abs(bfi) > abs(bi);
    }
    thread.fpr[code.frd] = bf;
    if (code.rc) { unknown("fcfid."); }
}
void PPUInterpreter::fcmpo(PPUFields code, PPUThread& thread)
{/*
    int cmp_res = FPRdouble::Cmp(thread.fpr[code.fra], thread.fpr[code.frb]);

    if (cmp_res == CR_SO) {
        if (FPRdouble::IsSNaN(thread.fpr[code.fra]) || FPRdouble::IsSNaN(thread.fpr[code.frb])) {
            thread.SetFPSCRException(FPSCR_VXSNAN);
            if (!thread.fpscr.VE) thread.SetFPSCRException(FPSCR_VXVC);
        }
        else {
            thread.SetFPSCRException(FPSCR_VXVC);
        }

        thread.fpscr.FX = 1;
    }

    thread.fpscr.FPRF = cmp_res;
    thread.cr.setField(code.crfd, cmp_res);
*/}
void PPUInterpreter::fcmpu(PPUFields code, PPUThread& thread)
{
    thread.cr.updateField(code.crfd, thread.fpr[code.fra], thread.fpr[code.frb]);
    if (thread.cr.getField(code.crfd) & (1 << thread.cr.CR_SO)) {
        if (std::isnan(thread.fpr[code.fra]) || std::isnan(thread.fpr[code.frb])) {
            thread.fpscr.setException(FPSCR_VXSNAN);
        }
    }
}
void PPUInterpreter::fctid(PPUFields code, PPUThread& thread)
{
    const f64 b = thread.fpr[code.frb];
    u64 r;
    if (b > (f64)0x7FFFFFFFFFFFFFFF) {
        r = 0x7FFFFFFFFFFFFFFF;
        if (thread.fpscr.VXCVI) thread.fpscr.FX = 1;
        thread.fpscr.VXCVI = 1;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    }
    else if (b < -(f64)0x8000000000000000) {
        r = 0x8000000000000000;
        if (thread.fpscr.VXCVI) thread.fpscr.FX = 1;
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
            thread.fpscr.FR = ::fabs(di) > ::fabs(b);
        }
    }

    (u64&)thread.fpr[code.frd] = 0xFFF8000000000000ULL | r;
    if (r == 0 && ((u64&)b & DOUBLE_SIGN)) (u64&)thread.fpr[code.frd] |= 0x100000000ULL;

    if (code.rc) { unknown("fctid."); }
}
void PPUInterpreter::fctidz(PPUFields code, PPUThread& thread)
{
    const f64 b = thread.fpr[code.frb];
    u64 r;
    if (b > (f64)0x7FFFFFFFFFFFFFFF) {
        r = 0x7FFFFFFFFFFFFFFF;
        if (thread.fpscr.VXCVI) thread.fpscr.FX = 1;
        thread.fpscr.VXCVI = 1;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    }
    else if (b < -(f64)0x8000000000000000) {
        r = 0x8000000000000000;
        if (thread.fpscr.VXCVI) thread.fpscr.FX = 1;
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
            thread.fpscr.FR = ::fabs(di) > ::fabs(b);
        }
        r = (u64)i;
    }

    (u64&)thread.fpr[code.frd] = 0xFFF8000000000000ULL | r;
    if (r == 0 && ((u64&)b & DOUBLE_SIGN)) {
        (u64&)thread.fpr[code.frd] |= 0x100000000ULL;
    }
    if (code.rc) { unknown("fctidz."); }
}
void PPUInterpreter::fctiw(PPUFields code, PPUThread& thread)
{
    const f64 b = thread.fpr[code.frb];
    u32 r;
    if (b > (f64)0x7FFFFFFF) {
        r = 0x7FFFFFFF;
        if (thread.fpscr.VXCVI) thread.fpscr.FX = 1;
        thread.fpscr.VXCVI = 1;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    }
    else if (b < -(f64)0x80000000) {
        r = 0x80000000;
        if (thread.fpscr.VXCVI) thread.fpscr.FX = 1;
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
            thread.fpscr.FR = ::fabs(di) > ::fabs(b);
        }
    }

    (u64&)thread.fpr[code.frd] = 0xFFF8000000000000ULL | r;
    if (r == 0 && ((u64&)b & DOUBLE_SIGN)) {
        (u64&)thread.fpr[code.frd] |= 0x100000000ULL;
    }
    if (code.rc) { unknown("fctiw."); }
}
void PPUInterpreter::fctiwz(PPUFields code, PPUThread& thread)
{
    const f64 b = thread.fpr[code.frb];
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
            thread.fpscr.FR = ::fabs(di) > ::fabs(b);
        }
        value = (u32)i;
    }

    (u64&)thread.fpr[code.frd] = 0xFFF8000000000000ULL | value;
    if (value == 0 && ((u64&)b & DOUBLE_SIGN)) {
        (u64&)thread.fpr[code.frd] |= 0x100000000ULL;
    }
    if (code.rc) unknown("fctiwz.");
}
void PPUInterpreter::fdiv(PPUFields code, PPUThread& thread)
{
    if (std::isnan(thread.fpr[code.fra])) {
        thread.fpr[code.frd] = thread.fpr[code.fra];
    }
    else if (std::isnan(thread.fpr[code.frb])) {
        thread.fpr[code.frd] = thread.fpr[code.frb];
    }
    /*else {
        if (thread.fpr[code.frb] == 0.0) {
            if (thread.fpr[code.fra] == 0.0) {
                thread.fpscr.VXZDZ = 1;
                thread.fpr[code.frd] = FPR_NAN;
            }
            else {
                thread.fpr[code.frd] = thread.fpr[code.fra] / thread.fpr[code.frb];
            }

            thread.SetFPSCRException(FPSCR_ZX);
        }
        else if (FPRdouble::IsINF(thread.fpr[code.fra]) && FPRdouble::IsINF(thread.fpr[code.frb])) {
            thread.fpscr.VXIDI = 1;
            thread.fpr[code.frd] = FPR_NAN;
        }
        else {
            thread.fpr[code.frd] = thread.fpr[code.fra] / thread.fpr[code.frb];
        }
    }*/
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { unknown("fdiv."); }
}
void PPUInterpreter::fdivs(PPUFields code, PPUThread& thread)
{
    if (std::isnan(thread.fpr[code.fra])) {
        thread.fpr[code.frd] = thread.fpr[code.fra];
    }
    else if (std::isnan(thread.fpr[code.frb])) {
        thread.fpr[code.frd] = thread.fpr[code.frb];
    }
    /*else {
        if (thread.fpr[code.frb] == 0.0) {
            if (thread.fpr[code.fra] == 0.0) {
                thread.fpscr.VXZDZ = true;
                thread.fpr[code.frd] = FPR_NAN;
            }
            else {
                thread.fpr[code.frd] = (f32)(thread.fpr[code.fra] / thread.fpr[code.frb]);
            }

            thread.fpscr.ZX = true;
        }
        else if (FPRdouble::IsINF(thread.fpr[code.fra]) && FPRdouble::IsINF(thread.fpr[code.frb])) {
            thread.fpscr.VXIDI = true;
            thread.fpr[code.frd] = FPR_NAN;
        }
        else {
            thread.fpr[code.frd] = (f32)(thread.fpr[code.fra] / thread.fpr[code.frb]);
        }
    }*/

    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { unknown("fdivs."); }
}
void PPUInterpreter::fmadd(PPUFields code, PPUThread& thread)
{
    thread.fpr[code.frd] = thread.fpr[code.fra] * thread.fpr[code.frc] + thread.fpr[code.frb];
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { unknown("fmadd."); }
}
void PPUInterpreter::fmadds(PPUFields code, PPUThread& thread)
{
    thread.fpr[code.frd] = static_cast<f32>(thread.fpr[code.fra] * thread.fpr[code.frc] + thread.fpr[code.frb]);
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { unknown("fmadds."); }
}
void PPUInterpreter::fmr(PPUFields code, PPUThread& thread)
{
    thread.fpr[code.frd] = thread.fpr[code.frb];
    if (code.rc) { unknown("fmr."); }
}
void PPUInterpreter::fmsub(PPUFields code, PPUThread& thread)
{
    thread.fpr[code.frd] = thread.fpr[code.fra] * thread.fpr[code.frc] - thread.fpr[code.frb];
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { unknown("fmsub."); }
}
void PPUInterpreter::fmsubs(PPUFields code, PPUThread& thread)
{
    thread.fpr[code.frd] = static_cast<f32>(thread.fpr[code.fra] * thread.fpr[code.frc] - thread.fpr[code.frb]);
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { unknown("fmsubs."); }
}
void PPUInterpreter::fmul(PPUFields code, PPUThread& thread)
{/*
    if ((FPRdouble::IsINF(thread.fpr[code.fra]) && thread.fpr[code.frc] == 0.0) || (FPRdouble::IsINF(thread.fpr[code.frc]) && thread.fpr[code.fra] == 0.0)) {
        thread.SetFPSCRException(FPSCR_VXIMZ);
        thread.fpr[code.frd] = FPR_NAN;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
        thread.fpscr.FPRF = FPR_QNAN;
    }
    else {
        if (FPRdouble::IsSNaN(thread.fpr[code.fra]) || FPRdouble::IsSNaN(thread.fpr[code.frc])) {
            thread.SetFPSCRException(FPSCR_VXSNAN);
        }

        thread.fpr[code.frd] = thread.fpr[code.fra] * thread.fpr[code.frc];
        thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    }

    if (code.rc) { unknown("fmul."); }
*/}
void PPUInterpreter::fmuls(PPUFields code, PPUThread& thread)
{
    thread.fpr[code.frd] = static_cast<f32>(thread.fpr[code.fra] * thread.fpr[code.frc]);
    thread.fpscr.FI = 0;
    thread.fpscr.FR = 0;
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { unknown("fmuls."); }
}
void PPUInterpreter::fnabs(PPUFields code, PPUThread& thread)
{
    thread.fpr[code.frd] = -::fabs(thread.fpr[code.frb]);
    if (code.rc) { unknown("fnabs."); }
}
void PPUInterpreter::fneg(PPUFields code, PPUThread& thread)
{
    thread.fpr[code.frd] = -thread.fpr[code.frb];
    if (code.rc) { unknown("fneg."); }
}
void PPUInterpreter::fnmadd(PPUFields code, PPUThread& thread)
{
    thread.fpr[code.frd] = -(thread.fpr[code.fra] * thread.fpr[code.frc] + thread.fpr[code.frb]);
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { unknown("fnmadd."); }
}
void PPUInterpreter::fnmadds(PPUFields code, PPUThread& thread)
{
    thread.fpr[code.frd] = static_cast<f32>(-(thread.fpr[code.fra] * thread.fpr[code.frc] + thread.fpr[code.frb]));
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { unknown("fnmadds."); }
}
void PPUInterpreter::fnmsub(PPUFields code, PPUThread& thread)
{
    thread.fpr[code.frd] = -(thread.fpr[code.fra] * thread.fpr[code.frc] - thread.fpr[code.frb]);
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { unknown("fnmsub."); }
}
void PPUInterpreter::fnmsubs(PPUFields code, PPUThread& thread)
{
    thread.fpr[code.frd] = static_cast<f32>(-(thread.fpr[code.fra] * thread.fpr[code.frc] - thread.fpr[code.frb]));
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { unknown("fnmsubs."); }
}
void PPUInterpreter::fres(PPUFields code, PPUThread& thread)
{
    if (thread.fpr[code.frb] == 0.0) {
        thread.fpscr.setException(FPSCR_ZX);
    }
    thread.fpr[code.frd] = static_cast<f32>(1.0 / thread.fpr[code.frb]);
    if (code.rc) { unknown("fres."); }
}
void PPUInterpreter::frsp(PPUFields code, PPUThread& thread)
{/*
    const f64 b = thread.fpr[code.frb];
    f64 b0 = b;
    if (thread.fpscr.NI) {
        if (((u64&)b0 & DOUBLE_EXP) < 0x3800000000000000ULL) (u64&)b0 &= DOUBLE_SIGN;
    }
    const f64 r = static_cast<f32>(b0);
    thread.fpscr.FR = fabs(r) > fabs(b);
    thread.SetFPSCR_FI(b != r);
    thread.fpscr.FPRF = getFPRFlags(r);
    thread.fpr[code.frd] = r;
*/}
void PPUInterpreter::frsqrte(PPUFields code, PPUThread& thread)
{
    if (thread.fpr[code.frb] == 0.0) {
        thread.fpscr.setException(FPSCR_ZX);
    }
    thread.fpr[code.frd] = static_cast<f32>(1.0 / sqrt(thread.fpr[code.frb]));
    if (code.rc) { unknown("frsqrte."); }
}
void PPUInterpreter::fsel(PPUFields code, PPUThread& thread)
{
    thread.fpr[code.frd] = thread.fpr[code.fra] >= 0.0 ? thread.fpr[code.frc] : thread.fpr[code.frb];
    if (code.rc) { unknown("fsel."); }
}
void PPUInterpreter::fsqrt(PPUFields code, PPUThread& thread)
{
    thread.fpr[code.frd] = sqrt(thread.fpr[code.frb]);
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { unknown("fsqrt."); }
}
void PPUInterpreter::fsqrts(PPUFields code, PPUThread& thread)
{
    thread.fpr[code.frd] = static_cast<f32>(sqrt(thread.fpr[code.frb]));
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { unknown("fsqrts."); }
}
void PPUInterpreter::fsub(PPUFields code, PPUThread& thread)
{
    thread.fpr[code.frd] = thread.fpr[code.fra] - thread.fpr[code.frb];
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { unknown("fsub."); }
}
void PPUInterpreter::fsubs(PPUFields code, PPUThread& thread)
{
    thread.fpr[code.frd] = static_cast<f32>(thread.fpr[code.fra] - thread.fpr[code.frb]);
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { unknown("fsubs."); }
}
void PPUInterpreter::mffs(PPUFields code, PPUThread& thread)
{
    (u64&)thread.fpr[code.frd] = thread.fpscr.FPSCR;
    if (code.rc) { unknown("mffs."); }
}
void PPUInterpreter::mtfsf(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::stfd(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    nucleus.memory.write64(addr, (u64&)thread.fpr[code.frs]);
}
void PPUInterpreter::stfdu(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    nucleus.memory.write64(addr, (u64&)thread.fpr[code.frs]);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::stfdux(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    nucleus.memory.write64(addr, (u64&)thread.fpr[code.frs]);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::stfdx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write64(addr, (u64&)thread.fpr[code.frs]);
}
void PPUInterpreter::stfiwx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write32(addr, (u32&)thread.fpr[code.frs]);
}
void PPUInterpreter::stfs(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    const f32 value = thread.fpr[code.frs];
    nucleus.memory.write32(addr, (u32&)value);
}
void PPUInterpreter::stfsu(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    const f32 value = thread.fpr[code.frs];
    nucleus.memory.write32(addr, (u32&)value);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::stfsux(PPUFields code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    const f32 value = thread.fpr[code.frs];
    nucleus.memory.write32(addr, (u32&)value);
    thread.gpr[code.ra] = addr;
}
void PPUInterpreter::stfsx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const f32 value = thread.fpr[code.frs];
    nucleus.memory.write32(addr, (u32&)value);
}

// Vector/SIMD instructions
void PPUInterpreter::lvebx(PPUFields code, PPUThread& thread)
{
    const u32 addr = (code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb]) & ~0xfULL;
    thread.vr[code.vd]._u128 = nucleus.memory.read128(addr);
}
void PPUInterpreter::lvehx(PPUFields code, PPUThread& thread)
{
    const u32 addr = (code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb]) & ~0xfULL;
    thread.vr[code.vd]._u128 = nucleus.memory.read128(addr);
}
void PPUInterpreter::lvewx(PPUFields code, PPUThread& thread)
{
    const u32 addr = (code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb]) & ~0xfULL;
    thread.vr[code.vd]._u128 = nucleus.memory.read128(addr);
}
void PPUInterpreter::lvlx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.readLeft(thread.vr[code.vd]._u8 + eb, addr, 16 - eb);
}
void PPUInterpreter::lvlxl(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.readLeft(thread.vr[code.vd]._u8 + eb, addr, 16 - eb);
}
void PPUInterpreter::lvrx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.readRight(thread.vr[code.vd]._u8, addr & ~0xf, eb);
}
void PPUInterpreter::lvrxl(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.readRight(thread.vr[code.vd]._u8, addr & ~0xf, eb);
}
void PPUInterpreter::lvsl(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::lvsr(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::lvx(PPUFields code, PPUThread& thread)
{
    const u32 addr = (code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb]) & ~0xfULL;
    thread.vr[code.vd]._u128 = nucleus.memory.read128(addr);
}
void PPUInterpreter::lvxl(PPUFields code, PPUThread& thread)
{
    const u32 addr = (code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb]) & ~0xfULL;
    thread.vr[code.vd]._u128 = nucleus.memory.read128(addr);
}
void PPUInterpreter::mfvscr(PPUFields code, PPUThread& thread)
{
    thread.vr[code.vd].clear();
    thread.vr[code.vd]._u32[0] = thread.vscr.VSCR;
}
void PPUInterpreter::mtvscr(PPUFields code, PPUThread& thread)
{
    thread.vscr.VSCR = thread.vr[code.vb]._u32[0];
}
void PPUInterpreter::stvebx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.write8(addr, thread.vr[code.vs]._u8[15 - eb]);
}
void PPUInterpreter::stvehx(PPUFields code, PPUThread& thread)
{
    const u32 addr = (code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb]) & ~1ULL;
    const u8 eb = (addr & 0xf) >> 1;
    nucleus.memory.write16(addr, thread.vr[code.vs]._u16[7 - eb]);
}
void PPUInterpreter::stvewx(PPUFields code, PPUThread& thread)
{
    const u32 addr = (code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb]) & ~3ULL;
    const u8 eb = (addr & 0xf) >> 2;
    nucleus.memory.write32(addr, thread.vr[code.vs]._u32[3 - eb]);
}
void PPUInterpreter::stvlx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.writeLeft(addr, thread.vr[code.vs]._u8 + eb, 16 - eb);
}
void PPUInterpreter::stvlxl(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.writeLeft(addr, thread.vr[code.vs]._u8 + eb, 16 - eb);
}
void PPUInterpreter::stvrx(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.writeRight(addr - eb, thread.vr[code.vs]._u8, eb);
}
void PPUInterpreter::stvrxl(PPUFields code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.writeRight(addr - eb, thread.vr[code.vs]._u8, eb);
}
void PPUInterpreter::stvx(PPUFields code, PPUThread& thread)
{
    const u32 addr = (code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb]) & ~0xfULL;
    nucleus.memory.write128(addr, thread.vr[code.vs]._u128);
}
void PPUInterpreter::stvxl(PPUFields code, PPUThread& thread)
{
    const u32 addr = (code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb]) & ~0xfULL;
    nucleus.memory.write128(addr, thread.vr[code.vs]._u128);
}
void PPUInterpreter::vaddcuw(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = ~thread.vr[code.va]._u32[w] < thread.vr[code.vb]._u32[w];
    }
}
void PPUInterpreter::vaddfp(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = thread.vr[code.va]._f32[w] + thread.vr[code.vb]._f32[w];
    }
}
void PPUInterpreter::vaddsbs(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vaddshs(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vaddsws(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vaddubm(PPUFields code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = thread.vr[code.va]._u8[b] + thread.vr[code.vb]._u8[b];
    }
}
void PPUInterpreter::vaddubs(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vadduhm(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = thread.vr[code.va]._u16[h] + thread.vr[code.vb]._u16[h];
    }
}
void PPUInterpreter::vadduhs(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vadduwm(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._u32[w] + thread.vr[code.vb]._u32[w];
    }
}
void PPUInterpreter::vadduws(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vand(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._u32[w] & thread.vr[code.vb]._u32[w];
    }
}
void PPUInterpreter::vandc(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._u32[w] & (~thread.vr[code.vb]._u32[w]);
    }
}
void PPUInterpreter::vavgsb(PPUFields code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._s8[b] = (thread.vr[code.va]._s8[b] + thread.vr[code.vb]._s8[b] + 1) >> 1;
    }
}
void PPUInterpreter::vavgsh(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._s16[h] = (thread.vr[code.va]._s16[h] + thread.vr[code.vb]._s16[h] + 1) >> 1;
    }
}
void PPUInterpreter::vavgsw(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._s32[w] = ((s64)thread.vr[code.va]._s32[w] + (s64)thread.vr[code.vb]._s32[w] + 1) >> 1;
    }
}
void PPUInterpreter::vavgub(PPUFields code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = (thread.vr[code.va]._u8[b] + thread.vr[code.vb]._u8[b] + 1) >> 1;
    }
}
void PPUInterpreter::vavguh(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = (thread.vr[code.va]._u16[h] + thread.vr[code.vb]._u16[h] + 1) >> 1;
    }
}
void PPUInterpreter::vavguw(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = ((u64)thread.vr[code.va]._u32[w] + (u64)thread.vr[code.vb]._u32[w] + 1) >> 1;
    }
}
void PPUInterpreter::vcfsx(PPUFields code, PPUThread& thread)
{
    const u32 scale = 1 << code.vuimm;
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = ((f32)thread.vr[code.vb]._s32[w]) / scale;
    }
}
void PPUInterpreter::vcfux(PPUFields code, PPUThread& thread)
{
    const u32 scale = 1 << code.vuimm;
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = ((f32)thread.vr[code.vb]._u32[w]) / scale;
    }
}
void PPUInterpreter::vcmpbfp(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vcmpbfp_(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vcmpeqfp(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._f32[w] == thread.vr[code.vb]._f32[w] ? 0xFFFFFFFF : 0;
    }
}
void PPUInterpreter::vcmpeqfp_(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vcmpequb(PPUFields code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = thread.vr[code.va]._u8[b] == thread.vr[code.vb]._u8[b] ? 0xFF : 0;
    }
}
void PPUInterpreter::vcmpequb_(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vcmpequh(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = thread.vr[code.va]._u16[h] == thread.vr[code.vb]._u16[h] ? 0xFFFF : 0;
    }
}
void PPUInterpreter::vcmpequh_(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vcmpequw(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._u32[w] == thread.vr[code.vb]._u32[w] ? 0xFFFFFFFF : 0;
    }
}
void PPUInterpreter::vcmpequw_(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vcmpgefp(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._f32[w] >= thread.vr[code.vb]._f32[w] ? 0xFFFFFFFF : 0;
    }
}
void PPUInterpreter::vcmpgefp_(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vcmpgtfp(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._f32[w] > thread.vr[code.vb]._f32[w] ? 0xFFFFFFFF : 0;
    }
}
void PPUInterpreter::vcmpgtfp_(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vcmpgtsb(PPUFields code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = thread.vr[code.va]._s8[b] > thread.vr[code.vb]._s8[b] ? 0xFF : 0;
    }
}
void PPUInterpreter::vcmpgtsb_(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vcmpgtsh(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = thread.vr[code.va]._s16[h] > thread.vr[code.vb]._s16[h] ? 0xFFFF : 0;
    }
}
void PPUInterpreter::vcmpgtsh_(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vcmpgtsw(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._s32[w] > thread.vr[code.vb]._s32[w] ? 0xFFFFFFFF : 0;
    }
}
void PPUInterpreter::vcmpgtsw_(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vcmpgtub(PPUFields code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = thread.vr[code.va]._u8[b] > thread.vr[code.vb]._u8[b] ? 0xFF : 0;
    }
}
void PPUInterpreter::vcmpgtub_(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vcmpgtuh(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = thread.vr[code.va]._u16[h] > thread.vr[code.vb]._u16[h] ? 0xFFFF : 0;
    }
}
void PPUInterpreter::vcmpgtuh_(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vcmpgtuw(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._u32[w] > thread.vr[code.vb]._u32[w] ? 0xFFFFFFFF : 0;
    }
}
void PPUInterpreter::vcmpgtuw_(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vctsxs(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vctuxs(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vexptefp(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = exp(thread.vr[code.vb]._f32[w] * log(2.0f));
    }
}
void PPUInterpreter::vlogefp(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = log(thread.vr[code.vb]._f32[w]) / log(2.0f);
    }
}
void PPUInterpreter::vmaddfp(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = thread.vr[code.va]._f32[w] * thread.vr[code.vc]._f32[w] + thread.vr[code.vb]._f32[w];
    }
}
void PPUInterpreter::vmaxfp(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = std::max(thread.vr[code.va]._f32[w], thread.vr[code.vb]._f32[w]);
    }
}
void PPUInterpreter::vmaxsb(PPUFields code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++)
        thread.vr[code.vd]._s8[b] = std::max(thread.vr[code.va]._s8[b], thread.vr[code.vb]._s8[b]);
}
void PPUInterpreter::vmaxsh(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._s16[h] = std::max(thread.vr[code.va]._s16[h], thread.vr[code.vb]._s16[h]);
    }
}
void PPUInterpreter::vmaxsw(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._s32[w] = std::max(thread.vr[code.va]._s32[w], thread.vr[code.vb]._s32[w]);
    }
}
void PPUInterpreter::vmaxub(PPUFields code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++)
        thread.vr[code.vd]._u8[b] = std::max(thread.vr[code.va]._u8[b], thread.vr[code.vb]._u8[b]);
}
void PPUInterpreter::vmaxuh(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = std::max(thread.vr[code.va]._u16[h], thread.vr[code.vb]._u16[h]);
    }
}
void PPUInterpreter::vmaxuw(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = std::max(thread.vr[code.va]._u32[w], thread.vr[code.vb]._u32[w]);
    }
}
void PPUInterpreter::vmhaddshs(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vmhraddshs(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vminfp(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = std::min(thread.vr[code.va]._f32[w], thread.vr[code.vb]._f32[w]);
    }
}
void PPUInterpreter::vminsb(PPUFields code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._s8[b] = std::min(thread.vr[code.va]._s8[b], thread.vr[code.vb]._s8[b]);
    }
}
void PPUInterpreter::vminsh(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._s16[h] = std::min(thread.vr[code.va]._s16[h], thread.vr[code.vb]._s16[h]);
    }
}
void PPUInterpreter::vminsw(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._s32[w] = std::min(thread.vr[code.va]._s32[w], thread.vr[code.vb]._s32[w]);
    }
}
void PPUInterpreter::vminub(PPUFields code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = std::min(thread.vr[code.va]._u8[b], thread.vr[code.vb]._u8[b]);
    }
}
void PPUInterpreter::vminuh(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = std::min(thread.vr[code.va]._u16[h], thread.vr[code.vb]._u16[h]);
    }
}
void PPUInterpreter::vminuw(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = std::min(thread.vr[code.va]._u32[w], thread.vr[code.vb]._u32[w]);
    }
}
void PPUInterpreter::vmladduhm(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = thread.vr[code.va]._u16[h] * thread.vr[code.vb]._u16[h] + thread.vr[code.vc]._u16[h];
    }
}
void PPUInterpreter::vmrghb(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u8[15 - h*2 + 0] = thread.vr[code.va]._u8[15 - h];
        thread.vr[code.vd]._u8[15 - h*2 - 1] = thread.vr[code.vb]._u8[15 - h];
    }
}
void PPUInterpreter::vmrghh(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u16[7 - w*2 + 0] = thread.vr[code.va]._u16[7 - w];
        thread.vr[code.vd]._u16[7 - w*2 - 1] = thread.vr[code.vb]._u16[7 - w];
    }
}
void PPUInterpreter::vmrghw(PPUFields code, PPUThread& thread)
{
    for (int d = 0; d < 2; d++) {
        thread.vr[code.vd]._u32[3 - d*2 + 0] = thread.vr[code.va]._u32[3 - d];
        thread.vr[code.vd]._u32[3 - d*2 - 1] = thread.vr[code.vb]._u32[3 - d];
    }
}
void PPUInterpreter::vmrglb(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u8[15 - h*2 + 0] = thread.vr[code.va]._u8[7 - h];
        thread.vr[code.vd]._u8[15 - h*2 - 1] = thread.vr[code.vb]._u8[7 - h];
    }
}
void PPUInterpreter::vmrglh(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u16[7 - w*2] = thread.vr[code.va]._u16[3 - w];
        thread.vr[code.vd]._u16[7 - w*2 - 1] = thread.vr[code.vb]._u16[3 - w];
    }
}
void PPUInterpreter::vmrglw(PPUFields code, PPUThread& thread)
{
    for (int d = 0; d < 2; d++) {
        thread.vr[code.vd]._u32[3 - d*2 + 0] = thread.vr[code.va]._u32[1 - d];
        thread.vr[code.vd]._u32[3 - d*2 - 1] = thread.vr[code.vb]._u32[1 - d];
    }
}
void PPUInterpreter::vmsummbm(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vmsumshm(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vmsumshs(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vmsumubm(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vmsumuhm(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vmsumuhs(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vmulesb(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._s16[h] = (s16)thread.vr[code.va]._s8[h*2+1] * (s16)thread.vr[code.vb]._s8[h*2+1];
    }
}
void PPUInterpreter::vmulesh(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._s32[w] = (s32)thread.vr[code.va]._s16[w*2+1] * (s32)thread.vr[code.vb]._s16[w*2+1];
    }
}
void PPUInterpreter::vmuleub(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = (u16)thread.vr[code.va]._u8[h*2+1] * (u16)thread.vr[code.vb]._u8[h*2+1];
    }
}
void PPUInterpreter::vmuleuh(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = (u32)thread.vr[code.va]._u16[w*2+1] * (u32)thread.vr[code.vb]._u16[w*2+1];
    }
}
void PPUInterpreter::vmulosb(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._s16[h] = (s16)thread.vr[code.va]._s8[h*2] * (s16)thread.vr[code.vb]._s8[h*2];
    }
}
void PPUInterpreter::vmulosh(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._s32[w] = (s32)thread.vr[code.va]._s16[w*2] * (s32)thread.vr[code.vb]._s16[w*2];
    }
}
void PPUInterpreter::vmuloub(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = (u16)thread.vr[code.va]._u8[h*2] * (u16)thread.vr[code.vb]._u8[h*2];
    }
}
void PPUInterpreter::vmulouh(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = (u32)thread.vr[code.va]._u16[w*2] * (u32)thread.vr[code.vb]._u16[w*2];
    }
}
void PPUInterpreter::vnmsubfp(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = -(thread.vr[code.va]._f32[w] * thread.vr[code.vc]._f32[w] - thread.vr[code.vb]._f32[w]);
    }
}
void PPUInterpreter::vnor(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = ~(thread.vr[code.va]._u32[w] | thread.vr[code.vb]._u32[w]);
    }
}
void PPUInterpreter::vor(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._u32[w] | thread.vr[code.vb]._u32[w];
    }
}
void PPUInterpreter::vperm(PPUFields code, PPUThread& thread)
{
    u8 tmpSRC[32];
    memcpy(tmpSRC, thread.vr[code.vb]._u8, 16);
    memcpy(tmpSRC + 16, thread.vr[code.va]._u8, 16);

    for (int b = 0; b < 16; b++) {
        u8 index = thread.vr[code.vc]._u8[b] & 0x1f;

        thread.vr[code.vd]._u8[b] = tmpSRC[0x1f - index];
    }
}
void PPUInterpreter::vpkpx(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vpkshss(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vpkshus(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vpkswss(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vpkswus(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vpkuhum(PPUFields code, PPUThread& thread)
{
    for (int b = 0; b < 8; b++) {
        thread.vr[code.vd]._u8[b+8] = thread.vr[code.va]._u8[b*2];
        thread.vr[code.vd]._u8[b  ] = thread.vr[code.vb]._u8[b*2];
    }
}
void PPUInterpreter::vpkuhus(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vpkuwum(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 4; h++) {
        thread.vr[code.vd]._u16[h+4] = thread.vr[code.va]._u16[h*2];
        thread.vr[code.vd]._u16[h  ] = thread.vr[code.vb]._u16[h*2];
    }
}
void PPUInterpreter::vpkuwus(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vrefp(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = 1.0f / thread.vr[code.vb]._f32[w];
    }
}
void PPUInterpreter::vrfim(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = floor(thread.vr[code.vb]._f32[w]);
    }
}
void PPUInterpreter::vrfin(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = floor(thread.vr[code.vb]._f32[w] + 0.5f);
    }
}
void PPUInterpreter::vrfip(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = ceil(thread.vr[code.vb]._f32[w]);
    }
}
void PPUInterpreter::vrfiz(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        f32 f;
        modff(thread.vr[code.vb]._f32[w], &f);
        thread.vr[code.vd]._f32[w] = f;
    }
}
void PPUInterpreter::vrlb(PPUFields code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        int nRot = thread.vr[code.vb]._u8[b] & 0x7;

        thread.vr[code.vd]._u8[b] = (thread.vr[code.va]._u8[b] << nRot) | (thread.vr[code.va]._u8[b] >> (8 - nRot));
    }
}
void PPUInterpreter::vrlh(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = rotl16(thread.vr[code.va]._u16[h], thread.vr[code.vb]._u8[h*2] & 0xf);
    }
}
void PPUInterpreter::vrlw(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = rotl32(thread.vr[code.va]._u32[w], thread.vr[code.vb]._u8[w*4] & 0x1f);
    }
}
void PPUInterpreter::vrsqrtefp(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = 1.0f / sqrtf(thread.vr[code.vb]._f32[w]);
    }
}
void PPUInterpreter::vsel(PPUFields code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = (thread.vr[code.vb]._u8[b] & thread.vr[code.vc]._u8[b]) | (thread.vr[code.va]._u8[b] & (~thread.vr[code.vc]._u8[b]));
    }
}
void PPUInterpreter::vsl(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vslb(PPUFields code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = thread.vr[code.va]._u8[b] << (thread.vr[code.vb]._u8[b] & 0x7);
    }
}
void PPUInterpreter::vsldoi(PPUFields code, PPUThread& thread)
{
    u8 tmpSRC[32];
    memcpy(tmpSRC, thread.vr[code.vb]._u8, 16);
    memcpy(tmpSRC + 16, thread.vr[code.va]._u8, 16);
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[15 - b] = tmpSRC[31 - (b + code.sh)];
    }
}
void PPUInterpreter::vslh(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = thread.vr[code.va]._u16[h] << (thread.vr[code.vb]._u8[h*2] & 0xf);
    }
}
void PPUInterpreter::vslo(PPUFields code, PPUThread& thread)
{
    u8 nShift = (thread.vr[code.vb]._u8[0] >> 3) & 0xf;

    thread.vr[code.vd].clear();

    for (u8 b = 0; b < 16 - nShift; b++) {
        thread.vr[code.vd]._u8[15 - b] = thread.vr[code.va]._u8[15 - (b + nShift)];
    }
}
void PPUInterpreter::vslw(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._u32[w] << (thread.vr[code.vb]._u32[w] & 0x1f);
    }
}
void PPUInterpreter::vspltb(PPUFields code, PPUThread& thread)
{
    u8 byte = thread.vr[code.vb]._u8[15 - code.vuimm];

    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = byte;
    }
}
void PPUInterpreter::vsplth(PPUFields code, PPUThread& thread)
{
    const u16 hword = thread.vr[code.vb]._u16[7 - code.vuimm];
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = hword;
    }
}
void PPUInterpreter::vspltisb(PPUFields code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = code.vsimm;
    }
}
void PPUInterpreter::vspltish(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = (s16)code.vsimm;
    }
}
void PPUInterpreter::vspltisw(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = (s32)code.vsimm;
    }
}
void PPUInterpreter::vspltw(PPUFields code, PPUThread& thread)
{
    const u32 word = thread.vr[code.vb]._u32[3 - code.vuimm];
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = word;
    }
}
void PPUInterpreter::vsr(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vsrab(PPUFields code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._s8[b] = thread.vr[code.va]._s8[b] >> (thread.vr[code.vb]._u8[b] & 0x7);
    }
}
void PPUInterpreter::vsrah(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._s16[h] = thread.vr[code.va]._s16[h] >> (thread.vr[code.vb]._u8[h*2] & 0xf);
    }
}
void PPUInterpreter::vsraw(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._s32[w] = thread.vr[code.va]._s32[w] >> (thread.vr[code.vb]._u8[w*4] & 0x1f);
    }
}
void PPUInterpreter::vsrb(PPUFields code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = thread.vr[code.va]._u8[b] >> (thread.vr[code.vb]._u8[b] & 0x7);
    }
}
void PPUInterpreter::vsrh(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = thread.vr[code.va]._u16[h] >> (thread.vr[code.vb]._u8[h*2] & 0xf);
    }
}
void PPUInterpreter::vsro(PPUFields code, PPUThread& thread)
{
    const u8 nShift = (thread.vr[code.vb]._u8[0] >> 3) & 0xf;
    thread.vr[code.vd].clear();

    for (u8 b = 0; b < 16 - nShift; b++) {
        thread.vr[code.vd]._u8[b] = thread.vr[code.va]._u8[b + nShift];
    }
}
void PPUInterpreter::vsrw(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._u32[w] >> (thread.vr[code.vb]._u8[w*4] & 0x1f);
    }
}
void PPUInterpreter::vsubcuw(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._u32[w] < thread.vr[code.vb]._u32[w] ? 0 : 1;
    }
}
void PPUInterpreter::vsubfp(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._f32[w] = thread.vr[code.va]._f32[w] - thread.vr[code.vb]._f32[w];
    }
}
void PPUInterpreter::vsubsbs(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vsubshs(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vsubsws(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vsububm(PPUFields code, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[code.vd]._u8[b] = (u8)((thread.vr[code.va]._u8[b] - thread.vr[code.vb]._u8[b]) & 0xFF);
    }
}
void PPUInterpreter::vsububs(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vsubuhm(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._u16[h] = thread.vr[code.va]._u16[h] - thread.vr[code.vb]._u16[h];
    }
}
void PPUInterpreter::vsubuhs(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vsubuwm(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._u32[w] = thread.vr[code.va]._u32[w] - thread.vr[code.vb]._u32[w];
    }
}
void PPUInterpreter::vsubuws(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vsum2sws(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vsum4sbs(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vsum4shs(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vsum4ubs(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vsumsws(PPUFields code, PPUThread& thread)
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
void PPUInterpreter::vupkhpx(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._s8[(3 - w)*4 + 3] = thread.vr[code.vb]._s8[w*2 + 0] >> 7;
        thread.vr[code.vd]._u8[(3 - w)*4 + 2] = (thread.vr[code.vb]._u8[w*2 + 0] >> 2) & 0x1f;
        thread.vr[code.vd]._u8[(3 - w)*4 + 1] = ((thread.vr[code.vb]._u8[w*2 + 0] & 0x3) << 3) | ((thread.vr[code.vb]._u8[w*2 + 1] >> 5) & 0x7);
        thread.vr[code.vd]._u8[(3 - w)*4 + 0] = thread.vr[code.vb]._u8[w*2 + 1] & 0x1f;
    }
}
void PPUInterpreter::vupkhsb(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._s16[h] = thread.vr[code.vb]._s8[h];
    }
}
void PPUInterpreter::vupkhsh(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._s32[w] = thread.vr[code.vb]._s16[w];
    }
}
void PPUInterpreter::vupklpx(PPUFields code, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[code.vd]._s8[(3 - w)*4 + 3] = thread.vr[code.vb]._s8[8 + w*2 + 0] >> 7;
        thread.vr[code.vd]._u8[(3 - w)*4 + 2] = (thread.vr[code.vb]._u8[8 + w*2 + 0] >> 2) & 0x1f;
        thread.vr[code.vd]._u8[(3 - w)*4 + 1] = ((thread.vr[code.vb]._u8[8 + w*2 + 0] & 0x3) << 3) | ((thread.vr[code.vb]._u8[8 + w*2 + 1] >> 5) & 0x7);
        thread.vr[code.vd]._u8[(3 - w)*4 + 0] = thread.vr[code.vb]._u8[8 + w*2 + 1] & 0x1f;
    }
}
void PPUInterpreter::vupklsb(PPUFields code, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[code.vd]._s16[h] = thread.vr[code.vb]._s8[8 + h];
    }
}
void PPUInterpreter::vupklsh(PPUFields code, PPUThread& thread)
{
    thread.vr[code.vd]._s32[0] = thread.vr[code.vb]._s16[4 + 0];
    thread.vr[code.vd]._s32[1] = thread.vr[code.vb]._s16[4 + 1];
    thread.vr[code.vd]._s32[2] = thread.vr[code.vb]._s16[4 + 2];
    thread.vr[code.vd]._s32[3] = thread.vr[code.vb]._s16[4 + 3];
}
void PPUInterpreter::vxor(PPUFields code, PPUThread& thread)
{
    thread.vr[code.vd]._u64[0] = thread.vr[code.va]._u64[0] ^ thread.vr[code.vb]._u64[0];
    thread.vr[code.vd]._u32[1] = thread.vr[code.va]._u64[1] ^ thread.vr[code.vb]._u64[1];
}

// Unknown instruction
void PPUInterpreter::unknown(PPUFields code, PPUThread& thread)
{
    nucleus.log.error(LOG_CPU, "Unknown instruction: %08X (PC: %08X)", code.instruction, thread.pc);
}
void PPUInterpreter::unknown(const char* instruction)
{
    nucleus.log.error(LOG_CPU, "Unknown instruction: %s", instruction);
}
