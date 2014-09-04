/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_interpreter.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/ppu/interpreter/ppu_interpreter_tables.h"

#include <cmath>
#include <iostream>

// Instruction tables
static bool b_tablesInitialized = false;
typedef void (*func_t)(PPUInstruction, PPUThread&);
func_t s_tablePrimary[0x40];
func_t s_table4[0x40];
func_t s_table4_[0x800];
func_t s_table19[0x400];
func_t s_table30[0x8];
func_t s_table31[0x400];
func_t s_table58[0x4];
func_t s_table59[0x20];
func_t s_table62[0x4];
func_t s_table63[0x20];
func_t s_table63_[0x400];

// Instruction callers
void PPUInterpreter::callTable4  (PPUInstruction instr, PPUThread& thread) { return s_table4[instr.op4](instr, thread); }
void PPUInterpreter::callTable4_ (PPUInstruction instr, PPUThread& thread) { return s_table4_[instr.op4_](instr, thread); }
void PPUInterpreter::callTable19 (PPUInstruction instr, PPUThread& thread) { return s_table19[instr.op19](instr, thread); }
void PPUInterpreter::callTable30 (PPUInstruction instr, PPUThread& thread) { return s_table30[instr.op30](instr, thread); }
void PPUInterpreter::callTable31 (PPUInstruction instr, PPUThread& thread) { return s_table31[instr.op31](instr, thread); }
void PPUInterpreter::callTable58 (PPUInstruction instr, PPUThread& thread) { return s_table58[instr.op58](instr, thread); }
void PPUInterpreter::callTable59 (PPUInstruction instr, PPUThread& thread) { return s_table59[instr.op59](instr, thread); }
void PPUInterpreter::callTable62 (PPUInstruction instr, PPUThread& thread) { return s_table62[instr.op62](instr, thread); }
void PPUInterpreter::callTable63 (PPUInstruction instr, PPUThread& thread) { return s_table63[instr.op63](instr, thread); }
void PPUInterpreter::callTable63_(PPUInstruction instr, PPUThread& thread) { return s_table63_[instr.op63_](instr, thread); }

/**
 * Rotation-related functions
 */
inline u8 rotl8(const u8 x, const u8 n) { return (x << n) | (x >> (8 - n)); }
inline u8 rotr8(const u8 x, const u8 n) { return (x >> n) | (x << (8 - n)); }
inline u16 rotl16(const u16 x, const u8 n) { return (x << n) | (x >> (16 - n)); }
inline u16 rotr16(const u16 x, const u8 n) { return (x >> n) | (x << (16 - n)); }
inline u32 rotl32(const u32 x, const u8 n) { return (x << n) | (x >> (32 - n)); }
inline u32 rotr32(const u32 x, const u8 n) { return (x >> n) | (x << (32 - n)); }
inline u64 rotl64(const u64 x, const u8 n) { return (x << n) | (x >> (64 - n)); }
inline u64 rotr64(const u64 x, const u8 n) { return (x >> n) | (x << (64 - n)); }

static u64 rotateMask[64][64];
void initRotateMask()
{
    static bool initialized = false;
    if (initialized) {
        return;
    }
    for (int mb = 0; mb < 64; mb++) {
        for(u32 me = 0; me < 64; me++) {
            const u64 mask = (~1ULL >> mb) ^ ((me >= 63) ? 0 : ~1ULL >> (me + 1));    
            rotateMask[mb][me] = mb > me ? ~mask : mask;
        }
    }
    initialized = true;
}

PPUInterpreter::PPUInterpreter(PPUThread& thr) : thread(thr)
{
    initRotateMask();

    if (!b_tablesInitialized) {
        initTables();
    }
}

void PPUInterpreter::step()
{
    const PPUInstruction instr = { nucleus.memory.read32(thread.pc) };
    s_tablePrimary[instr.opcode](instr, thread);
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
    if (!thread.vscr.NJ) return v;

    const int fpc = _fpclass(v);
#ifdef __GNUG__
    if (fpc == FP_SUBNORMAL)
        return std::signbit(v) ? -0.0f : 0.0f;
#else
    if (fpc & _FPCLASS_ND) return -0.0f;
    if (fpc & _FPCLASS_PD) return  0.0f;
#endif

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

    //unknown2("GetRegBySPR error: Unknown SPR!");
    return thread.xer.XER;
}

/**
 * PPU Instructions
 */
void PPUInterpreter::add(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.rd] = thread.gpr[instr.ra] + thread.gpr[instr.rb];
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(CPU.GPR[instr.rd]);
    if (instr.oe) unknown2("addo");
}
void PPUInterpreter::addc(PPUInstruction instr, PPUThread& thread)
{
    const s64 gpra = thread.gpr[instr.ra];
    const s64 gprb = thread.gpr[instr.rb];
    thread.gpr[instr.rd] = gpra + gprb;
    thread.xer.CA = isCarry(gpra, gprb);
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(CPU.GPR[instr.rd]);
    if (instr.oe) unknown2("addco"); 
}
void PPUInterpreter::adde(PPUInstruction instr, PPUThread& thread)
{
    const u64 gpra = thread.gpr[instr.ra];
    const u64 gprb = thread.gpr[instr.rb];
    if (thread.xer.CA) {
        if (gpra == ~0ULL) {
            thread.gpr[instr.rd] = gprb;
            thread.xer.CA = 1;
        }
        else {
            thread.gpr[instr.rd] = gpra + 1 + gprb;
            thread.xer.CA = isCarry(gpra + 1, gprb);
        }
    }
    else {
        thread.gpr[instr.rd] = gpra + gprb;
        thread.xer.CA = isCarry(gpra, gprb);
    }
    if (instr.rc) // TODO CPU.UpdateCR0<s64>(CPU.GPR[instr.rd]);
    if (instr.oe) unknown2("addeo");
}
void PPUInterpreter::addi(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.rd] = instr.ra ? ((s64)thread.gpr[instr.ra] + instr.simm) : instr.simm;
}
void PPUInterpreter::addic(PPUInstruction instr, PPUThread& thread)
{
    const u64 gpra = thread.gpr[instr.ra];
    thread.gpr[instr.rd] = gpra + instr.simm;
    thread.xer.CA = isCarry(gpra, instr.simm);
}
void PPUInterpreter::addic_(PPUInstruction instr, PPUThread& thread)
{
    const u64 gpra = thread.gpr[instr.ra];
    thread.gpr[instr.rd] = gpra + instr.simm;
    thread.xer.CA = isCarry(gpra, instr.simm);
    // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.rd]);
}
void PPUInterpreter::addis(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.rd] = instr.ra ? ((s64)thread.gpr[instr.ra] + (instr.simm << 16)) : (instr.simm << 16);
}
void PPUInterpreter::addme(PPUInstruction instr, PPUThread& thread)
{
    const s64 gpra = thread.gpr[instr.ra];
    thread.gpr[instr.rd] = gpra + thread.xer.CA - 1;
    thread.xer.CA |= gpra != 0;
    if (instr.oe) unknown2("addmeo");
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.rd]);
}
void PPUInterpreter::addze(PPUInstruction instr, PPUThread& thread)
{
    const u64 gpra = thread.gpr[instr.ra];
    thread.gpr[instr.rd] = gpra + thread.xer.CA;
    thread.xer.CA = isCarry(gpra, thread.xer.CA);
    if (instr.oe) unknown2("addzeo");
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.rd]);
}
void PPUInterpreter::and(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.ra] = thread.gpr[instr.rs] & thread.gpr[instr.rb];
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::andc(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.ra] = thread.gpr[instr.rs] & ~thread.gpr[instr.rb];
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::andi_(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.ra] = thread.gpr[instr.rs] & instr.uimm;
    // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::andis_(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.ra] = thread.gpr[instr.rs] & (instr.uimm << 16);
    // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::b(PPUInstruction instr, PPUThread& thread)
{
    if (instr.lk) thread.lr = thread.pc + 4;
    thread.pc = (instr.aa ? (instr.ll << 2) : thread.pc + (instr.ll << 2)) & ~0x3ULL;
    thread.pc -= 4;
}
void PPUInterpreter::bc(PPUInstruction instr, PPUThread& thread)
{
    if (CheckCondition(thread, instr.bo, instr.bi)) {
        if (instr.lk) thread.lr = thread.pc + 4;
        thread.pc = (instr.aa ? instr.bd : thread.pc + instr.bd) & ~0x3ULL;
        thread.pc -= 4;
    }
}
void PPUInterpreter::bcctr(PPUInstruction instr, PPUThread& thread)
{
    if (instr.bo & 0x10 || thread.cr.getBit(instr.bi) == ((instr.bo >> 3) & 1)) {
        if (instr.lk) thread.lr = thread.pc + 4;
        thread.pc = thread.ctr & ~0x3ULL;
        thread.pc -= 4;
    }
}    
void PPUInterpreter::bclr(PPUInstruction instr, PPUThread& thread)
{
    if (CheckCondition(thread, instr.bo, instr.bi)) {
        const u32 newLR = thread.pc + 4;
        thread.pc = thread.lr & ~0x3ULL;
        thread.pc -= 4;
        if (instr.lk) thread.lr = newLR;
    }
}
void PPUInterpreter::cmp(PPUInstruction instr, PPUThread& thread)
{
    // TODO: CPU.UpdateCRnS(l, instr.crfd, thread.gpr[instr.ra], thread.gpr[instr.rb]);
}
void PPUInterpreter::cmpi(PPUInstruction instr, PPUThread& thread)
{
    // TODO: CPU.UpdateCRnS(l, instr.crfd, thread.gpr[instr.ra], instr.simm);
}
void PPUInterpreter::cmpl(PPUInstruction instr, PPUThread& thread)
{
    // TODO: CPU.UpdateCRnU(l, instr.crfd, thread.gpr[instr.ra], thread.gpr[instr.rb]);
}
void PPUInterpreter::cmpli(PPUInstruction instr, PPUThread& thread)
{
    // TODO: CPU.UpdateCRnU(l, instr.crfd, thread.gpr[instr.ra], instr.uimm);
}
void PPUInterpreter::cntlzd(PPUInstruction instr, PPUThread& thread)
{
    for (int i = 0; i < 64; i++) {
        if (thread.gpr[instr.rs] & (1ULL << (63 - i))) {
            thread.gpr[instr.ra] = i;
            break;
        }
    }   
    if (instr.rc) thread.cr.setBit(thread.cr.CR_LT, false);
}
void PPUInterpreter::cntlzw(PPUInstruction instr, PPUThread& thread)
{
    for (int i = 0; i < 32; i++) {
        if (thread.gpr[instr.rs] & (1ULL << (31 - i))) {
            thread.gpr[instr.ra] = i;
            break;
        }
    }
    if (instr.rc) thread.cr.setBit(thread.cr.CR_LT, false);
}
void PPUInterpreter::crand(PPUInstruction instr, PPUThread& thread)
{
    const u8 value = thread.cr.getBit(instr.crba) & thread.cr.getBit(instr.crbb);
    thread.cr.setBit(instr.crbd, value);
}
void PPUInterpreter::crandc(PPUInstruction instr, PPUThread& thread)
{
    const u8 value = thread.cr.getBit(instr.crba) & (1 ^ thread.cr.getBit(instr.crbb));
    thread.cr.setBit(instr.crbd, value);
}
void PPUInterpreter::creqv(PPUInstruction instr, PPUThread& thread)
{
    const u8 value = 1 ^ (thread.cr.getBit(instr.crba) ^ thread.cr.getBit(instr.crbb));
    thread.cr.setBit(instr.crbd, value);
}
void PPUInterpreter::crnand(PPUInstruction instr, PPUThread& thread)
{
    const u8 value = 1 ^ (thread.cr.getBit(instr.crba) & thread.cr.getBit(instr.crbb));
    thread.cr.setBit(instr.crbd, value);
}
void PPUInterpreter::crnor(PPUInstruction instr, PPUThread& thread)
{
    const u8 value = 1 ^ (thread.cr.getBit(instr.crba) | thread.cr.getBit(instr.crbb));
    thread.cr.setBit(instr.crbd, value);
}
void PPUInterpreter::cror(PPUInstruction instr, PPUThread& thread)
{
    const u8 value = thread.cr.getBit(instr.crba) | thread.cr.getBit(instr.crbb);
    thread.cr.setBit(instr.crbd, value);
}
void PPUInterpreter::crorc(PPUInstruction instr, PPUThread& thread)
{
    const u8 value = thread.cr.getBit(instr.crba) | (1 ^ thread.cr.getBit(instr.crbb));
    thread.cr.setBit(instr.crbd, value);
}
void PPUInterpreter::crxor(PPUInstruction instr, PPUThread& thread)
{
    const u8 value = thread.cr.getBit(instr.crba) ^ thread.cr.getBit(instr.crbb);
    thread.cr.setBit(instr.crbd, value);
}
void PPUInterpreter::dcbf(PPUInstruction instr, PPUThread& thread)
{
    //unknown2("dcbf", false);
    _mm_mfence();
}
void PPUInterpreter::dcbst(PPUInstruction instr, PPUThread& thread)
{
    //unknown2("dcbst", false);
    _mm_mfence();
}
void PPUInterpreter::dcbt(PPUInstruction instr, PPUThread& thread)
{
    //unknown2("dcbt", false);
    _mm_mfence();
}
void PPUInterpreter::dcbtst(PPUInstruction instr, PPUThread& thread)
{
    //unknown2("dcbtst", false);
    _mm_mfence();
}
void PPUInterpreter::dcbz(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    void* cache_line = nucleus.memory + (addr & ~127);
    if (cache_line) {
        memset(cache_line, 0, 128);
    }
    _mm_mfence();
}
void PPUInterpreter::divd(PPUInstruction instr, PPUThread& thread)
{
    const s64 gpra = thread.gpr[instr.ra];
    const s64 gprb = thread.gpr[instr.rb];
    if (gprb == 0 || ((u64)gpra == (1ULL << 63) && gprb == -1)) {
        if (instr.oe) unknown2("divdo");
        thread.gpr[instr.rd] = 0;
    }
    else {
        thread.gpr[instr.rd] = gpra / gprb;
    }
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.rd]);
}
void PPUInterpreter::divdu(PPUInstruction instr, PPUThread& thread)
{
    const u64 gpra = thread.gpr[instr.ra];
    const s64 gprb = thread.gpr[instr.rb];
    if (gprb == 0) {
        if (instr.oe) unknown2("divduo");
        thread.gpr[instr.rd] = 0;
    }
    else {
        thread.gpr[instr.rd] = gpra / gprb;
    }
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.rd]);
}
void PPUInterpreter::divw(PPUInstruction instr, PPUThread& thread)
{
    const s32 gpra = thread.gpr[instr.ra];
    const s32 gprb = thread.gpr[instr.rb];
    if (gprb == 0 || ((u32)gpra == (1 << 31) && gprb == -1)) {
        if (instr.oe) unknown2("divwo");
        thread.gpr[instr.rd] = 0;
    }
    else {
        thread.gpr[instr.rd] = (u32)(gpra / gprb);
    }
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.rd]);
}
void PPUInterpreter::divwu(PPUInstruction instr, PPUThread& thread)
{
    const u32 gpra = thread.gpr[instr.ra];
    const u32 gprb = thread.gpr[instr.rb];
    if (gprb == 0) {
        if (instr.oe) unknown2("divwuo");
        thread.gpr[instr.rd] = 0;
    }
    else {
        thread.gpr[instr.rd] = gpra / gprb;
    }
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.rd]);
}
void PPUInterpreter::dss(PPUInstruction instr, PPUThread& thread)
{
    _mm_mfence();
}
void PPUInterpreter::dst(PPUInstruction instr, PPUThread& thread)
{
    _mm_mfence();
}
void PPUInterpreter::dstst(PPUInstruction instr, PPUThread& thread)
{
    _mm_mfence();
}
void PPUInterpreter::eciwx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    thread.gpr[instr.rd] = nucleus.memory.read32(addr);
}
void PPUInterpreter::ecowx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    nucleus.memory.write32(addr, thread.gpr[instr.rs]);
}
void PPUInterpreter::eieio(PPUInstruction instr, PPUThread& thread)
{
    _mm_mfence();
}
void PPUInterpreter::eqv(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.ra] = ~(thread.gpr[instr.rs] ^ thread.gpr[instr.rb]);
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::extsb(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.ra] = (s64)(s8)thread.gpr[instr.rs];
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::extsh(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.ra] = (s64)(s16)thread.gpr[instr.rs];
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::extsw(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.ra] = (s64)(s32)thread.gpr[instr.rs];
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::icbi(PPUInstruction instr, PPUThread& thread)
{
    // Nothing to do in the interpreter.
}
void PPUInterpreter::isync(PPUInstruction instr, PPUThread& thread)
{
    _mm_mfence();
}
void PPUInterpreter::lbz(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.rd] = nucleus.memory.read8(instr.ra ? thread.gpr[instr.ra] + instr.d : instr.d);
}
void PPUInterpreter::lbzu(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + instr.d;
    thread.gpr[instr.rd] = nucleus.memory.read8(addr);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::lbzux(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + thread.gpr[instr.rb];
    thread.gpr[instr.rd] = nucleus.memory.read8(addr);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::lbzx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    thread.gpr[instr.rd] = nucleus.memory.read8(addr);
}
void PPUInterpreter::ld(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + (instr.ds << 2) : (instr.ds << 2);
    thread.gpr[instr.rd] = nucleus.memory.read64(addr);
}
void PPUInterpreter::ldarx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    thread.gpr[instr.rd] = nucleus.memory.read64(addr);
    thread.reserve_addr = addr;
    thread.reserve_value = re64(thread.gpr[instr.rd]);
    
}
void PPUInterpreter::ldbrx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    thread.gpr[instr.rd] = re64(nucleus.memory.read64(addr));
}
void PPUInterpreter::ldu(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + (instr.ds << 2);
    thread.gpr[instr.rd] = nucleus.memory.read64(addr);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::ldux(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + thread.gpr[instr.rb];
    thread.gpr[instr.rd] = nucleus.memory.read64(addr);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::ldx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    thread.gpr[instr.rd] = nucleus.memory.read64(addr);
}
void PPUInterpreter::lha(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + instr.d : instr.d;
    thread.gpr[instr.rd] = (s64)(s16)nucleus.memory.read16(addr);
}
void PPUInterpreter::lhau(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + instr.d;
    thread.gpr[instr.rd] = (s64)(s16)nucleus.memory.read16(addr);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::lhaux(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    thread.gpr[instr.rd] = (s64)(s16)nucleus.memory.read16(addr);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::lhax(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    thread.gpr[instr.rd] = (s64)(s16)nucleus.memory.read16(addr);
}
void PPUInterpreter::lhbrx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    thread.gpr[instr.rd] = re16(nucleus.memory.read16(addr));
}
void PPUInterpreter::lhz(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + instr.d : instr.d;
    thread.gpr[instr.rd] = nucleus.memory.read16(addr);
}
void PPUInterpreter::lhzu(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + instr.d;
    thread.gpr[instr.rd] = nucleus.memory.read16(addr);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::lhzux(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    thread.gpr[instr.rd] = nucleus.memory.read16(addr);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::lhzx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    thread.gpr[instr.rd] = nucleus.memory.read16(addr);
}
void PPUInterpreter::lmw(PPUInstruction instr, PPUThread& thread)
{
    u32 addr = instr.ra ? thread.gpr[instr.ra] + instr.d : instr.d;
    for (u32 i = instr.rd; i < 32; i++) {
        thread.gpr[i] = nucleus.memory.read32(addr);
        addr += 4;
    }
}
void PPUInterpreter::lswi(PPUInstruction instr, PPUThread& thread)
{
    u64 ea = instr.ra ? thread.gpr[instr.ra] : 0;
    u64 n = instr.nb ? instr.nb : 32;
    u8 reg = thread.gpr[instr.rd];
    while (n > 0) {
        if (n > 3) {
            thread.gpr[reg] = nucleus.memory.read32(ea);
            ea += 4;
            n -= 4;
        }
        else {
            u32 buf = 0;
            while (n > 0) {
                n--;
                buf |= nucleus.memory.read8(ea) << (n*8);
                ea++;
            }
            thread.gpr[reg] = buf;
        }
        reg = (reg + 1) % 32;
    }
}
void PPUInterpreter::lswx(PPUInstruction instr, PPUThread& thread)
{
    unknown2("lswx");
}
void PPUInterpreter::lwa(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.rd] = (s64)(s32)nucleus.memory.read32(instr.ra ? thread.gpr[instr.ra] + (instr.ds << 2) : (instr.ds << 2));
}
void PPUInterpreter::lwarx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    thread.gpr[instr.rd] = nucleus.memory.read32(addr);
    thread.reserve_addr = addr;
    thread.reserve_value = re32(thread.gpr[instr.rd]);
}
void PPUInterpreter::lwaux(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    thread.gpr[instr.rd] = (s64)(s32)nucleus.memory.read32(addr);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::lwax(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    thread.gpr[instr.rd] = (s64)(s32)nucleus.memory.read32(addr);
}
void PPUInterpreter::lwbrx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    thread.gpr[instr.rd] = re32(nucleus.memory.read32(addr));
}
void PPUInterpreter::lwz(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + instr.d : instr.d;
    thread.gpr[instr.rd] = nucleus.memory.read32(addr);
}
void PPUInterpreter::lwzu(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + instr.d;
    thread.gpr[instr.rd] = nucleus.memory.read32(addr);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::lwzux(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + thread.gpr[instr.rb];
    thread.gpr[instr.rd] = nucleus.memory.read32(addr);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::lwzx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    thread.gpr[instr.rd] = nucleus.memory.read32(addr);
}
void PPUInterpreter::mcrf(PPUInstruction instr, PPUThread& thread)
{
    thread.cr.setField(instr.crfd, thread.cr.getField(instr.crfs));
}
void PPUInterpreter::mcrfs(PPUInstruction instr, PPUThread& thread)
{
    u64 mask = (1ULL << instr.crbd);
    thread.cr.CR &= ~mask;
    thread.cr.CR |= thread.fpscr.FPSCR & mask;
}
void PPUInterpreter::mfocrf(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.rd] = thread.cr.CR;
}
void PPUInterpreter::mfspr(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.rd] = GetRegBySPR(thread, instr.spr);
}
void PPUInterpreter::mftb(PPUInstruction instr, PPUThread& thread)
{
    const u32 n = (instr.spr >> 5) | ((instr.spr & 0x1f) << 5);
    switch (n) {
    case 0x10C: thread.gpr[instr.rd] = thread.tbl; break;
    case 0x10D: thread.gpr[instr.rd] = thread.tbu; break;
    default: unknown2("mftb"); break;
    }
}
void PPUInterpreter::mtfsb0(PPUInstruction instr, PPUThread& thread)
{
    thread.fpscr.FPSCR &= ~(1ULL << instr.crbd);
    if (instr.rc) unknown2("mtfsb0.");
}
void PPUInterpreter::mtfsb1(PPUInstruction instr, PPUThread& thread)
{
    thread.fpscr.FPSCR |= (1ULL << instr.crbd);
    if (instr.rc) unknown2("mtfsb1.");
}
void PPUInterpreter::mtfsfi(PPUInstruction instr, PPUThread& thread)
{
    const u64 mask = (0x1ULL << instr.crfd);
    if (instr.i) {
        thread.fpscr.FPSCR |= mask;
    }
    else {
        thread.fpscr.FPSCR &= ~mask;
    }
    if (instr.rc) unknown2("mtfsfi.");
}
void PPUInterpreter::mtocrf(PPUInstruction instr, PPUThread& thread)
{
    if (instr.l11) {
        u32 n = 0, count = 0;
        for (int i = 0; i < 8; i++) {
            if (instr.crm & (1 << i)) {
                n = i;
                count++;
            }
        }
        if (count == 1) {
            thread.cr.setField(7 - n, (thread.gpr[instr.rs] >> (4*n)) & 0xf);
        }
        else {
            thread.cr.CR = 0;
        }
    }
    else {
        for (u32 i=0; i<8; ++i) {
            if (instr.crm & (1 << i)) {
                thread.cr.setField(7 - i, thread.gpr[instr.rs] & (0xf << (i * 4)));
            }
        }
    }
}
void PPUInterpreter::mtspr(PPUInstruction instr, PPUThread& thread)
{
    GetRegBySPR(thread, instr.spr) = thread.gpr[instr.rs];
}
void PPUInterpreter::mulhd(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.rd] = __mulh(thread.gpr[instr.ra], thread.gpr[instr.rb]);
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.rd]);
}
void PPUInterpreter::mulhdu(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.rd] = __umulh(thread.gpr[instr.ra], thread.gpr[instr.rb]);
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.rd]);
}
void PPUInterpreter::mulhw(PPUInstruction instr, PPUThread& thread)
{
    s32 a = thread.gpr[instr.ra];
    s32 b = thread.gpr[instr.rb];
    thread.gpr[instr.rd] = ((s64)a * (s64)b) >> 32;
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.rd]);
}
void PPUInterpreter::mulhwu(PPUInstruction instr, PPUThread& thread)
{
    u32 a = thread.gpr[instr.ra];
    u32 b = thread.gpr[instr.rb];
    thread.gpr[instr.rd] = ((u64)a * (u64)b) >> 32;
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.rd]);
}
void PPUInterpreter::mulld(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.rd] = (s64)((s64)thread.gpr[instr.ra] * (s64)thread.gpr[instr.rb]);
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.rd]);
    if (instr.oe) unknown2("mulldo");
}
void PPUInterpreter::mulli(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.rd] = (s64)thread.gpr[instr.ra] * instr.simm;
}
void PPUInterpreter::mullw(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.rd] = (s64)((s64)(s32)thread.gpr[instr.ra] * (s64)(s32)thread.gpr[instr.rb]);
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.rd]);
    if (instr.oe) unknown2("mullwo");
}
void PPUInterpreter::nand(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.ra] = ~(thread.gpr[instr.rs] & thread.gpr[instr.rb]);

    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::neg(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.rd] = 0 - thread.gpr[instr.ra];
    if (instr.oe) unknown2("nego");
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.rd]);
}
void PPUInterpreter::nop(PPUInstruction instr, PPUThread& thread)
{
}
void PPUInterpreter::nor(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.ra] = ~(thread.gpr[instr.rs] | thread.gpr[instr.rb]);
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::or(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.ra] = thread.gpr[instr.rs] | thread.gpr[instr.rb];
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::orc(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.ra] = thread.gpr[instr.rs] | ~thread.gpr[instr.rb];
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::ori(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.ra] = thread.gpr[instr.rs] | instr.uimm;
}
void PPUInterpreter::oris(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.ra] = thread.gpr[instr.rs] | (instr.uimm << 16);
}
void PPUInterpreter::rldc_lr(PPUInstruction instr, PPUThread& thread)
{
    if (instr.aa) {
        rldicr(instr, thread);
    }
    else {
        rldicl(instr, thread);
    }
}
void PPUInterpreter::rldic(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.ra] = rotl64(thread.gpr[instr.rs], instr.sh) & rotateMask[instr.mb][63-instr.sh];
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::rldicl(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.ra] = rotl64(thread.gpr[instr.rs], instr.sh) & rotateMask[instr.mb][63];
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::rldicr(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.ra] = rotl64(thread.gpr[instr.rs], instr.sh) & rotateMask[0][instr.me];
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::rldimi(PPUInstruction instr, PPUThread& thread)
{
    const u64 mask = rotateMask[instr.mb][63-instr.sh];
    thread.gpr[instr.ra] = (thread.gpr[instr.ra] & ~mask) | (rotl64(thread.gpr[instr.rs], instr.sh) & mask);
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::rlwimi(PPUInstruction instr, PPUThread& thread)
{
    const u64 mask = rotateMask[32 + instr.mb][32 + instr.me];
    thread.gpr[instr.ra] = (thread.gpr[instr.ra] & ~mask) | (rotl32(thread.gpr[instr.rs], instr.sh) & mask);
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::rlwinm(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.ra] = rotl32(thread.gpr[instr.rs], instr.sh) & rotateMask[32 + instr.mb][32 + instr.me];
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::rlwnm(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.ra] = rotl32(thread.gpr[instr.rs], thread.gpr[instr.rb] & 0x1f) & rotateMask[32 + instr.mb][32 + instr.me];
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::sc(PPUInstruction instr, PPUThread& thread)
{
    if (instr.sys == 2) {
        unknown2("sc");//TODO: SysCall();
    }
    else {
        unknown2("sc");
    }
}
void PPUInterpreter::sld(PPUInstruction instr, PPUThread& thread)
{
    u32 n = thread.gpr[instr.rb] & 0x3f;
    u64 r = rotl64(thread.gpr[instr.rs], n);
    u64 m = (thread.gpr[instr.rb] & 0x40) ? 0 : rotateMask[0][63 - n];
    thread.gpr[instr.ra] = r & m;
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::slw(PPUInstruction instr, PPUThread& thread)
{
    u32 n = thread.gpr[instr.rb] & 0x1f;
    u32 r = rotl32((u32)thread.gpr[instr.rs], n);
    u32 m = (thread.gpr[instr.rb] & 0x20) ? 0 : rotateMask[32][63 - n];
    thread.gpr[instr.ra] = r & m;
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::srad(PPUInstruction instr, PPUThread& thread)
{
    s64 RS = thread.gpr[instr.rs];
    u8 shift = thread.gpr[instr.rb] & 127;
    if (shift > 63) {
        thread.gpr[instr.ra] = 0 - (RS < 0);
        thread.xer.CA = (RS < 0);
    }
    else {
        thread.gpr[instr.ra] = RS >> shift;
        thread.xer.CA = (RS < 0) & ((thread.gpr[instr.ra] << shift) != RS);
    }
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::sradi1(PPUInstruction instr, PPUThread& thread)
{
    s64 RS = thread.gpr[instr.rs];
    thread.gpr[instr.ra] = RS >> instr.sh;
    thread.xer.CA = (RS < 0) & ((thread.gpr[instr.ra] << instr.sh) != RS);
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::sradi2(PPUInstruction instr, PPUThread& thread)
{
    sradi1(instr, thread);
}
void PPUInterpreter::sraw(PPUInstruction instr, PPUThread& thread)
{
    s32 gprs = thread.gpr[instr.rs];
    u8 shift = thread.gpr[instr.rb] & 63;
    if (shift > 31) {
        thread.gpr[instr.ra] = 0 - (gprs < 0);
        thread.xer.CA = (gprs < 0);
    }
    else {
        thread.gpr[instr.ra] = gprs >> shift;
        thread.xer.CA = (gprs < 0) & ((thread.gpr[instr.ra] << shift) != gprs);
    }
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::srawi(PPUInstruction instr, PPUThread& thread)
{
    s32 gprs = (u32)thread.gpr[instr.rs];
    thread.gpr[instr.ra] = gprs >> instr.sh;
    thread.xer.CA = (gprs < 0) & ((u32)(thread.gpr[instr.ra] << instr.sh) != gprs);
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::srd(PPUInstruction instr, PPUThread& thread)
{
    u32 n = thread.gpr[instr.rb] & 0x3f;
    u64 m = (thread.gpr[instr.rb] & 0x40) ? 0 : rotateMask[n][63];
    u64 r = rotl64(thread.gpr[instr.rs], 64 - n);
    thread.gpr[instr.ra] = r & m;
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::srw(PPUInstruction instr, PPUThread& thread)
{
    u32 n = thread.gpr[instr.rb] & 0x1f;
    u32 m = (thread.gpr[instr.rb] & 0x20) ? 0 : rotateMask[32 + n][63];
    u32 r = rotl32((u32)thread.gpr[instr.rs], 64 - n);
    thread.gpr[instr.ra] = r & m;
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::stb(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + instr.d : instr.d;
    nucleus.memory.write8(addr, thread.gpr[instr.rs]);
}
void PPUInterpreter::stbu(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + instr.d;
    nucleus.memory.write8(addr, thread.gpr[instr.rs]);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::stbux(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + thread.gpr[instr.rb];
    nucleus.memory.write8(addr, thread.gpr[instr.rs]);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::stbx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    nucleus.memory.write8(addr, thread.gpr[instr.rs]);
}
void PPUInterpreter::std(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + instr.d : instr.d;
    nucleus.memory.write64(addr, thread.gpr[instr.rs]);
}
void PPUInterpreter::stdcx_(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    if (thread.reserve_addr == addr) {
        bool value = false;// TODO: InterlockedCompareExchange((volatile u64*)(nucleus.memory + addr), re64(thread.gpr[instr.rs]), thread.reserve_value) == thread.reserve_value;
        thread.cr.setBit(thread.cr.CR_EQ, value);
    }
    else {
        thread.cr.setBit(thread.cr.CR_EQ, false);
    }
}
void PPUInterpreter::stdu(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + (instr.ds << 2);
    nucleus.memory.write64(addr, thread.gpr[instr.rs]);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::stdux(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + thread.gpr[instr.rb];
    nucleus.memory.write64(addr, thread.gpr[instr.rs]);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::stdx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    nucleus.memory.write64(addr, thread.gpr[instr.rs]);
}
void PPUInterpreter::sth(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + instr.d : instr.d;
    nucleus.memory.write16(addr, thread.gpr[instr.rs]);
}
void PPUInterpreter::sthbrx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    nucleus.memory.write16(addr, re16((u16)thread.gpr[instr.rs]));
}
void PPUInterpreter::sthu(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + instr.d;
    nucleus.memory.write16(addr, thread.gpr[instr.rs]);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::sthux(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + thread.gpr[instr.rb];
    nucleus.memory.write16(addr, thread.gpr[instr.rs]);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::sthx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    nucleus.memory.write16(addr, thread.gpr[instr.rs]);
}
void PPUInterpreter::stmw(PPUInstruction instr, PPUThread& thread)
{
    u32 addr = instr.ra ? thread.gpr[instr.ra] + instr.d : instr.d;
    for (int i = instr.rs; i < 32; i++) {
        nucleus.memory.write32(addr, thread.gpr[i]);
        addr += 4;
    }
}
void PPUInterpreter::stswi(PPUInstruction instr, PPUThread& thread)
{
    u64 ea = instr.ra ? thread.gpr[instr.ra] : 0;
    u64 n = instr.nb ? instr.nb : 32;
    u8 reg = thread.gpr[instr.rd];

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
void PPUInterpreter::stswx(PPUInstruction instr, PPUThread& thread)
{
    unknown2("stwsx");
}
void PPUInterpreter::stw(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + instr.d : instr.d;
    nucleus.memory.write32(addr, thread.gpr[instr.rs]);
}
void PPUInterpreter::stwbrx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    nucleus.memory.write32(addr, re32(thread.gpr[instr.rs]));
}
void PPUInterpreter::stwcx_(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    if (thread.reserve_addr == addr) {
        bool value = false;// TODO: InterlockedCompareExchange((volatile u32*)(nucleus.memory + addr), re32((u32)thread.gpr[instr.rs]), (u32)thread.reserve_value) == (u32)thread.reserve_value;
        thread.cr.setBit(thread.cr.CR_EQ, value);
    }
    else {
        thread.cr.setBit(thread.cr.CR_EQ, false);
    }
}
void PPUInterpreter::stwu(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + instr.d;
    nucleus.memory.write32(addr, thread.gpr[instr.rs]);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::stwux(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + thread.gpr[instr.rb];
    nucleus.memory.write32(addr, thread.gpr[instr.rs]);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::stwx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    nucleus.memory.write32(addr, thread.gpr[instr.rs]);
}
void PPUInterpreter::subf(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.rd] = thread.gpr[instr.rb] - thread.gpr[instr.ra];
    if (instr.oe) unknown2("subfo");
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.rd]);
}
void PPUInterpreter::subfc(PPUInstruction instr, PPUThread& thread)
{
    const u64 gpra = thread.gpr[instr.ra];
    const s64 gprb = thread.gpr[instr.rb];
    thread.gpr[instr.rd] = ~gpra + gprb + 1;
    thread.xer.CA = isCarry(~gpra, gprb, 1);
    if (instr.oe) unknown2("subfco");
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.rd]);
}
void PPUInterpreter::subfe(PPUInstruction instr, PPUThread& thread)
{
    const u64 gpra = thread.gpr[instr.ra];
    const s64 gprb = thread.gpr[instr.rb];
    thread.gpr[instr.rd] = ~gpra + gprb + thread.xer.CA;
    thread.xer.CA = isCarry(~gpra, gprb, thread.xer.CA);
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.rd]);
    if (instr.oe) unknown2("subfeo");
}
void PPUInterpreter::subfic(PPUInstruction instr, PPUThread& thread)
{
    const u64 gpra = thread.gpr[instr.ra];
    const u64 IMM = (s64)instr.simm;
    thread.gpr[instr.rd] = ~gpra + IMM + 1;

    thread.xer.CA = isCarry(~gpra, IMM, 1);
}
void PPUInterpreter::subfme(PPUInstruction instr, PPUThread& thread)
{
    const u64 gpra = thread.gpr[instr.ra];
    thread.gpr[instr.rd] = ~gpra + thread.xer.CA + ~0ULL;
    thread.xer.CA = isCarry(~gpra, thread.xer.CA, ~0ULL);
    if (instr.oe) unknown2("subfmeo");
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.rd]);
}
void PPUInterpreter::subfze(PPUInstruction instr, PPUThread& thread)
{
    const u64 gpra = thread.gpr[instr.ra];
    thread.gpr[instr.rd] = ~gpra + thread.xer.CA;
    thread.xer.CA = isCarry(~gpra, thread.xer.CA);
    if (instr.oe) unknown2("subfzeo");
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.rd]);
}
void PPUInterpreter::sync(PPUInstruction instr, PPUThread& thread)
{
    _mm_mfence();
}
void PPUInterpreter::td(PPUInstruction instr, PPUThread& thread)
{
    unknown2("td");
}
void PPUInterpreter::tdi(PPUInstruction instr, PPUThread& thread)
{
    s64 a = thread.gpr[instr.ra];

    if ((a < (s64)instr.simm  && (instr.to & 0x10)) ||
        (a > (s64)instr.simm  && (instr.to & 0x8))  ||
        (a == (s64)instr.simm && (instr.to & 0x4))  ||
        ((u64)a < (u64)instr.simm && (instr.to & 0x2)) ||
        ((u64)a > (u64)instr.simm && (instr.to & 0x1))) {
        unknown2("tdi");
    }
}

void PPUInterpreter::tw(PPUInstruction instr, PPUThread& thread)
{
    s32 a = thread.gpr[instr.ra];
    s32 b = thread.gpr[instr.rb];

    if ((a < b  && (instr.to & 0x10)) ||
        (a > b  && (instr.to & 0x8))  ||
        (a == b && (instr.to & 0x4))  ||
        ((u32)a < (u32)b && (instr.to & 0x2)) ||
        ((u32)a > (u32)b && (instr.to & 0x1))) {
        unknown2("tw");
    }
}
void PPUInterpreter::twi(PPUInstruction instr, PPUThread& thread)
{
    s32 a = thread.gpr[instr.ra];

    if ((a < instr.simm  && (instr.to & 0x10)) ||
        (a > instr.simm  && (instr.to & 0x8))  ||
        (a == instr.simm && (instr.to & 0x4))  ||
        ((u32)a < (u32)instr.simm && (instr.to & 0x2)) ||
        ((u32)a > (u32)instr.simm && (instr.to & 0x1))) {
        unknown2("twi");
    }
}
void PPUInterpreter::xor(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.ra] = thread.gpr[instr.rs] ^ thread.gpr[instr.rb];
    if (instr.rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[instr.ra]);
}
void PPUInterpreter::xori(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.ra] = thread.gpr[instr.rs] ^ instr.uimm;
}
void PPUInterpreter::xoris(PPUInstruction instr, PPUThread& thread)
{
    thread.gpr[instr.ra] = thread.gpr[instr.rs] ^ (instr.uimm << 16);
}

// Floating-Point Unit instructions
void PPUInterpreter::lfd(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + instr.d : instr.d;
    (u64&)thread.fpr[instr.frd] = nucleus.memory.read64(addr);
}
void PPUInterpreter::lfdu(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + (instr.ds << 2);
    (u64&)thread.fpr[instr.frd] = nucleus.memory.read64(addr);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::lfdux(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + thread.gpr[instr.rb];
    (u64&)thread.fpr[instr.frd] = nucleus.memory.read64(addr);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::lfdx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    (u64&)thread.fpr[instr.frd] = nucleus.memory.read64(addr);
}
void PPUInterpreter::lfs(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + instr.d : instr.d;
    const u32 value = nucleus.memory.read32(addr);
    thread.fpr[instr.frd] = (f32&)value;
}
void PPUInterpreter::lfsu(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + (instr.ds << 2);
    const u32 v = nucleus.memory.read32(addr);
    thread.fpr[instr.frd] = (f32&)v;
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::lfsux(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + thread.gpr[instr.rb];
    (u64&)thread.fpr[instr.frd] = nucleus.memory.read32(addr);
    thread.fpr[instr.frd] = (f32&)thread.fpr[instr.frd];
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::lfsx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    (u32&)thread.fpr[instr.frd] = nucleus.memory.read32(addr);
    thread.fpr[instr.frd] = (f32&)thread.fpr[instr.frd];
}
void PPUInterpreter::fabs(PPUInstruction instr, PPUThread& thread)
{
    const f32 value = thread.fpr[instr.frb];
    thread.fpr[instr.frd] = (value < 0) ? -value : value;
    if (instr.rc) unknown2("fabs.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
}
void PPUInterpreter::fadd(PPUInstruction instr, PPUThread& thread)
{
    thread.fpr[instr.frd] = thread.fpr[instr.fra] + thread.fpr[instr.frb];
    if (instr.rc) unknown2("fadd.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
}
void PPUInterpreter::fadds(PPUInstruction instr, PPUThread& thread)
{
    thread.fpr[instr.frd] = static_cast<f32>(thread.fpr[instr.fra] + thread.fpr[instr.frb]);
    if (instr.rc) unknown2("fadds.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
}
void PPUInterpreter::fcfid(PPUInstruction instr, PPUThread& thread)
{
    const s64 bi = (s64&)thread.fpr[instr.frb];
    const f64 bf = (f64)bi;
    const s64 bfi = (s64)bf;
    if (bi == bfi) {
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    }
    else {
        thread.fpscr.FI = 1;
        thread.fpscr.FR = abs(bfi) > abs(bi);
    }
    thread.fpr[instr.frd] = bf;
    if (instr.rc) unknown2("fcfid.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
}
void PPUInterpreter::fcmpo(PPUInstruction instr, PPUThread& thread)
{/*
    int cmp_res = FPRdouble::Cmp(thread.fpr[instr.fra], thread.fpr[instr.frb]);

    if (cmp_res == CR_SO) {
        if (FPRdouble::IsSNaN(thread.fpr[instr.fra]) || FPRdouble::IsSNaN(thread.fpr[instr.frb])) {
            thread.SetFPSCRException(FPSCR_VXSNAN);
            if (!thread.fpscr.VE) thread.SetFPSCRException(FPSCR_VXVC);
        }
        else {
            thread.SetFPSCRException(FPSCR_VXVC);
        }

        thread.fpscr.FX = 1;
    }

    thread.fpscr.FPRF = cmp_res;
    thread.cr.setField(instr.crfd, cmp_res);
*/}
void PPUInterpreter::fcmpu(PPUInstruction instr, PPUThread& thread)
{/*
    int cmp_res = FPRdouble::Cmp(thread.fpr[instr.fra], thread.fpr[instr.frb]);
    if (cmp_res == CR_SO) {
        if (FPRdouble::IsSNaN(thread.fpr[instr.fra]) || FPRdouble::IsSNaN(thread.fpr[instr.frb])) {
            thread.SetFPSCRException(FPSCR_VXSNAN);
        }
    }
    thread.fpscr.FPRF = cmp_res;
    thread.cr.setField(instr.crfd, cmp_res);
*/}
void PPUInterpreter::fctid(PPUInstruction instr, PPUThread& thread)
{/*
    const f64 b = thread.fpr[instr.frb];
    u64 r;
    if (b > (f64)0x7fffffffffffffff) {
        r = 0x7fffffffffffffff;
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
            thread.fpscr.FR = fabs(di) > fabs(b);
        }
    }

    (u64&)thread.fpr[instr.frd] = 0xfff8000000000000ull | r;
    if (r == 0 && ((u64&)b & DOUBLE_SIGN)) (u64&)thread.fpr[instr.frd] |= 0x100000000ull;

    if (instr.rc) unknown2("fctid.");
*/}
void PPUInterpreter::fctidz(PPUInstruction instr, PPUThread& thread)
{/*
    const f64 b = thread.fpr[instr.frb];
    u64 r;
    if (b > (f64)0x7fffffffffffffff) {
        r = 0x7fffffffffffffff;
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
            thread.fpscr.FR = fabs(di) > fabs(b);
        }
        r = (u64)i;
    }

    (u64&)thread.fpr[instr.frd] = 0xfff8000000000000ull | r;
    if (r == 0 && ((u64&)b & DOUBLE_SIGN)) (u64&)thread.fpr[instr.frd] |= 0x100000000ull;

    if (instr.rc) unknown2("fctidz.");
*/}
void PPUInterpreter::fctiw(PPUInstruction instr, PPUThread& thread)
{/*
    const f64 b = thread.fpr[instr.frb];
    u32 r;
    if (b > (f64)0x7fffffff) {
        r = 0x7fffffff;
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
            thread.fpscr.FR = fabs(di) > fabs(b);
        }
    }

    (u64&)thread.fpr[instr.frd] = 0xfff8000000000000ull | r;
    if (r == 0 && ((u64&)b & DOUBLE_SIGN)) (u64&)thread.fpr[instr.frd] |= 0x100000000ull;
    if (instr.rc) unknown2("fctiw.");
*/}
void PPUInterpreter::fctiwz(PPUInstruction instr, PPUThread& thread)
{/*
    const f64 b = thread.fpr[instr.frb];
    u32 value;
    if (b > (f64)0x7fffffff) {
        value = 0x7fffffff;
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

    (u64&)thread.fpr[instr.frd] = 0xfff8000000000000ull | value;
    if (value == 0 && ((u64&)b & DOUBLE_SIGN)) {
        (u64&)thread.fpr[instr.frd] |= 0x100000000ull;
    }
    if (instr.rc) unknown2("fctiwz.");
*/}
void PPUInterpreter::fdiv(PPUInstruction instr, PPUThread& thread)
{/*
    f64 res;

    if (FPRdouble::IsNaN(thread.fpr[instr.fra])) {
        res = thread.fpr[instr.fra];
    }
    else if (FPRdouble::IsNaN(thread.fpr[instr.frb])) {
        res = thread.fpr[instr.frb];
    }
    else {
        if (thread.fpr[instr.frb] == 0.0) {
            if (thread.fpr[instr.fra] == 0.0) {
                thread.fpscr.VXZDZ = 1;
                res = FPR_NAN;
            }
            else {
                res = thread.fpr[instr.fra] / thread.fpr[instr.frb];
            }

            thread.SetFPSCRException(FPSCR_ZX);
        }
        else if (FPRdouble::IsINF(thread.fpr[instr.fra]) && FPRdouble::IsINF(thread.fpr[instr.frb])) {
            thread.fpscr.VXIDI = 1;
            res = FPR_NAN;
        }
        else {
            res = thread.fpr[instr.fra] / thread.fpr[instr.frb];
        }
    }

    thread.fpr[instr.frd] = res;
    thread.fpscr.FPRF = thread.fpr[instr.frd].GetType();
    if (instr.rc) unknown2("fdiv.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
*/}
void PPUInterpreter::fdivs(PPUInstruction instr, PPUThread& thread)
{/*
    if (FPRdouble::IsNaN(thread.fpr[instr.fra])) {
        thread.fpr[instr.frd] = thread.fpr[instr.fra];
    }
    else if (FPRdouble::IsNaN(thread.fpr[instr.frb])) {
        thread.fpr[instr.frd] = thread.fpr[instr.frb];
    }
    else {
        if (thread.fpr[instr.frb] == 0.0) {
            if (thread.fpr[instr.fra] == 0.0) {
                thread.fpscr.VXZDZ = true;
                thread.fpr[instr.frd] = FPR_NAN;
            }
            else {
                thread.fpr[instr.frd] = (f32)(thread.fpr[instr.fra] / thread.fpr[instr.frb]);
            }

            thread.fpscr.ZX = true;
        }
        else if (FPRdouble::IsINF(thread.fpr[instr.fra]) && FPRdouble::IsINF(thread.fpr[instr.frb])) {
            thread.fpscr.VXIDI = true;
            thread.fpr[instr.frd] = FPR_NAN;
        }
        else {
            thread.fpr[instr.frd] = (f32)(thread.fpr[instr.fra] / thread.fpr[instr.frb]);
        }
    }

    thread.fpscr.FPRF = thread.fpr[instr.frd].GetType();
    if (instr.rc) unknown2("fdivs.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
*/}
void PPUInterpreter::fmadd(PPUInstruction instr, PPUThread& thread)
{/*
    thread.fpr[instr.frd] = thread.fpr[instr.fra] * thread.fpr[instr.frc] + thread.fpr[instr.frb];
    thread.fpscr.FPRF = thread.fpr[instr.frd].GetType();
    if (instr.rc) unknown2("fmadd.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
*/}
void PPUInterpreter::fmadds(PPUInstruction instr, PPUThread& thread)
{/*
    thread.fpr[instr.frd] = static_cast<f32>(thread.fpr[instr.fra] * thread.fpr[instr.frc] + thread.fpr[instr.frb]);
    thread.fpscr.FPRF = thread.fpr[instr.frd].GetType();
    if (instr.rc) unknown2("fmadds.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
*/}
void PPUInterpreter::fmr(PPUInstruction instr, PPUThread& thread)
{/*
    thread.fpr[instr.frd] = thread.fpr[instr.frb];
    if (instr.rc) unknown2("fmr.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
*/}
void PPUInterpreter::fmsub(PPUInstruction instr, PPUThread& thread)
{/*
    thread.fpr[instr.frd] = thread.fpr[instr.fra] * thread.fpr[instr.frc] - thread.fpr[instr.frb];
    thread.fpscr.FPRF = thread.fpr[instr.frd].GetType();
    if (instr.rc) unknown2("fmsub.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
*/}
void PPUInterpreter::fmsubs(PPUInstruction instr, PPUThread& thread)
{/*
    thread.fpr[instr.frd] = static_cast<f32>(thread.fpr[instr.fra] * thread.fpr[instr.frc] - thread.fpr[instr.frb]);
    thread.fpscr.FPRF = thread.fpr[instr.frd].GetType();
    if (instr.rc) unknown2("fmsubs.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
*/}
void PPUInterpreter::fmul(PPUInstruction instr, PPUThread& thread)
{/*
    if ((FPRdouble::IsINF(thread.fpr[instr.fra]) && thread.fpr[instr.frc] == 0.0) || (FPRdouble::IsINF(thread.fpr[instr.frc]) && thread.fpr[instr.fra] == 0.0)) {
        thread.SetFPSCRException(FPSCR_VXIMZ);
        thread.fpr[instr.frd] = FPR_NAN;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
        thread.fpscr.FPRF = FPR_QNAN;
    }
    else {
        if (FPRdouble::IsSNaN(thread.fpr[instr.fra]) || FPRdouble::IsSNaN(thread.fpr[instr.frc])) {
            thread.SetFPSCRException(FPSCR_VXSNAN);
        }

        thread.fpr[instr.frd] = thread.fpr[instr.fra] * thread.fpr[instr.frc];
        thread.fpscr.FPRF = thread.fpr[instr.frd].GetType();
    }

    if (instr.rc) unknown2("fmul.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
*/}
void PPUInterpreter::fmuls(PPUInstruction instr, PPUThread& thread)
{/*
    thread.fpr[instr.frd] = static_cast<f32>(thread.fpr[instr.fra] * thread.fpr[instr.frc]);
    thread.fpscr.FI = 0;
    thread.fpscr.FR = 0;
    thread.fpscr.FPRF = thread.fpr[instr.frd].GetType();
    if (instr.rc) unknown2("fmuls.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
*/}
void PPUInterpreter::fnabs(PPUInstruction instr, PPUThread& thread)
{/*
    thread.fpr[instr.frd] = -fabs(thread.fpr[instr.frb]);
    if (instr.rc) unknown2("fnabs.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
*/}
void PPUInterpreter::fneg(PPUInstruction instr, PPUThread& thread)
{/*
    thread.fpr[instr.frd] = -thread.fpr[instr.frb];
    if (instr.rc) unknown2("fneg.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
*/}
void PPUInterpreter::fnmadd(PPUInstruction instr, PPUThread& thread)
{/*
    thread.fpr[instr.frd] = -(thread.fpr[instr.fra] * thread.fpr[instr.frc] + thread.fpr[instr.frb]);
    thread.fpscr.FPRF = thread.fpr[instr.frd].GetType();
    if (instr.rc) unknown2("fnmadd.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
*/}
void PPUInterpreter::fnmadds(PPUInstruction instr, PPUThread& thread)
{/*
    thread.fpr[instr.frd] = static_cast<f32>(-(thread.fpr[instr.fra] * thread.fpr[instr.frc] + thread.fpr[instr.frb]));
    thread.fpscr.FPRF = thread.fpr[instr.frd].GetType();
    if (instr.rc) unknown2("fnmadds.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
*/}
void PPUInterpreter::fnmsub(PPUInstruction instr, PPUThread& thread)
{/*
    thread.fpr[instr.frd] = -(thread.fpr[instr.fra] * thread.fpr[instr.frc] - thread.fpr[instr.frb]);
    thread.fpscr.FPRF = thread.fpr[instr.frd].GetType();
    if (instr.rc) unknown2("fnmsub.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
*/}
void PPUInterpreter::fnmsubs(PPUInstruction instr, PPUThread& thread)
{/*
    thread.fpr[instr.frd] = static_cast<f32>(-(thread.fpr[instr.fra] * thread.fpr[instr.frc] - thread.fpr[instr.frb]));
    thread.fpscr.FPRF = thread.fpr[instr.frd].GetType();
    if (instr.rc) unknown2("fnmsubs.");////{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
*/}
void PPUInterpreter::fres(PPUInstruction instr, PPUThread& thread)
{/*
    if (thread.fpr[instr.frb] == 0.0) {
        thread.SetFPSCRException(FPSCR_ZX);
    }
    thread.fpr[instr.frd] = static_cast<f32>(1.0 / thread.fpr[instr.frb]);
    if (instr.rc) unknown2("fres.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
*/}
void PPUInterpreter::frsp(PPUInstruction instr, PPUThread& thread)
{/*
    const f64 b = thread.fpr[instr.frb];
    f64 b0 = b;
    if (thread.fpscr.NI) {
        if (((u64&)b0 & DOUBLE_EXP) < 0x3800000000000000ULL) (u64&)b0 &= DOUBLE_SIGN;
    }
    const f64 r = static_cast<f32>(b0);
    thread.fpscr.FR = fabs(r) > fabs(b);
    thread.SetFPSCR_FI(b != r);
    thread.fpscr.FPRF = PPCdouble(r).GetType();
    thread.fpr[instr.frd] = r;
*/}
void PPUInterpreter::frsqrte(PPUInstruction instr, PPUThread& thread)
{/*
    if (thread.fpr[instr.frb] == 0.0) {
        thread.SetFPSCRException(FPSCR_ZX);
    }
    thread.fpr[instr.frd] = static_cast<f32>(1.0 / sqrt(thread.fpr[instr.frb]));
    if (instr.rc) unknown2("frsqrte.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
*/}
void PPUInterpreter::fsel(PPUInstruction instr, PPUThread& thread)
{/*
    thread.fpr[instr.frd] = thread.fpr[instr.fra] >= 0.0 ? thread.fpr[instr.frc] : thread.fpr[instr.frb];
    if (instr.rc) unknown2("fsel.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
*/}
void PPUInterpreter::fsqrt(PPUInstruction instr, PPUThread& thread)
{/*
    thread.fpr[instr.frd] = sqrt(thread.fpr[instr.frb]);
    thread.fpscr.FPRF = thread.fpr[instr.frd].GetType();
    if (instr.rc) unknown2("fsqrt.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
*/}
void PPUInterpreter::fsqrts(PPUInstruction instr, PPUThread& thread)
{/*
    thread.fpr[instr.frd] = static_cast<f32>(sqrt(thread.fpr[instr.frb]));
    thread.fpscr.FPRF = thread.fpr[instr.frd].GetType();
    if (instr.rc) unknown2("fsqrts.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
*/}
void PPUInterpreter::fsub(PPUInstruction instr, PPUThread& thread)
{/*
    thread.fpr[instr.frd] = thread.fpr[instr.fra] - thread.fpr[instr.frb];
    thread.fpscr.FPRF = thread.fpr[instr.frd].GetType();
    if (instr.rc) unknown2("fsub.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
*/}
void PPUInterpreter::fsubs(PPUInstruction instr, PPUThread& thread)
{/*
    thread.fpr[instr.frd] = static_cast<f32>(thread.fpr[instr.fra] - thread.fpr[instr.frb]);
    thread.fpscr.FPRF = thread.fpr[instr.frd].GetType();
    if (instr.rc) unknown2("fsubs.");//{} // TODO: CPU.UpdateCR1(thread.fpr[instr.frd]);
*/}
void PPUInterpreter::mffs(PPUInstruction instr, PPUThread& thread)
{
    (u64&)thread.fpr[instr.frd] = thread.fpscr.FPSCR;
    if (instr.rc) unknown2("mffs.");
}
void PPUInterpreter::mtfsf(PPUInstruction instr, PPUThread& thread)
{
    u32 mask = 0;
    for (int i = 0; i < 8; i++) {
        if (instr.fm & (1 << i)) {
            mask |= 0xf << (i * 4);
        }
    }
    thread.fpscr.FPSCR = (thread.fpscr.FPSCR & ~mask) | ((u32&)thread.fpr[instr.frb] & mask);
    if (instr.rc) unknown2("mtfsf.");
}
void PPUInterpreter::stfd(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + instr.d : instr.d;
    nucleus.memory.write64(addr, (u64&)thread.fpr[instr.frs]);
}
void PPUInterpreter::stfdu(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + instr.d;
    nucleus.memory.write64(addr, (u64&)thread.fpr[instr.frs]);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::stfdux(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + thread.gpr[instr.rb];
    nucleus.memory.write64(addr, (u64&)thread.fpr[instr.frs]);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::stfdx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    nucleus.memory.write64(addr, (u64&)thread.fpr[instr.frs]);
}
void PPUInterpreter::stfiwx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    nucleus.memory.write32(addr, (u32&)thread.fpr[instr.frs]);
}
void PPUInterpreter::stfs(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + instr.d : instr.d;
    const f32 value = thread.fpr[instr.frs];
    nucleus.memory.write32(addr, (u32&)value);
}
void PPUInterpreter::stfsu(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + instr.d;
    const f32 value = thread.fpr[instr.frs];
    nucleus.memory.write32(addr, (u32&)value);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::stfsux(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = thread.gpr[instr.ra] + thread.gpr[instr.rb];
    const f32 value = thread.fpr[instr.frs];
    nucleus.memory.write32(addr, (u32&)value);
    thread.gpr[instr.ra] = addr;
}
void PPUInterpreter::stfsx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    const f32 value = thread.fpr[instr.frs];
    nucleus.memory.write32(addr, (u32&)value);
}

// Vector/SIMD instructions
void PPUInterpreter::lvebx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = (instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb]) & ~0xfULL;
    thread.vr[instr.vd]._u128 = nucleus.memory.read128(addr);
}
void PPUInterpreter::lvehx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = (instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb]) & ~0xfULL;
    thread.vr[instr.vd]._u128 = nucleus.memory.read128(addr);
}
void PPUInterpreter::lvewx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = (instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb]) & ~0xfULL;
    thread.vr[instr.vd]._u128 = nucleus.memory.read128(addr);
}
void PPUInterpreter::lvlx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.readLeft(thread.vr[instr.vd]._u8 + eb, addr, 16 - eb);
}
void PPUInterpreter::lvlxl(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.readLeft(thread.vr[instr.vd]._u8 + eb, addr, 16 - eb);
}
void PPUInterpreter::lvrx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.readRight(thread.vr[instr.vd]._u8, addr & ~0xf, eb);
}
void PPUInterpreter::lvrxl(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.readRight(thread.vr[instr.vd]._u8, addr & ~0xf, eb);
}
void PPUInterpreter::lvsl(PPUInstruction instr, PPUThread& thread)
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
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    thread.vr[instr.vd]._u64[0] = lvsl_values[addr & 0xf][0];
    thread.vr[instr.vd]._u64[1] = lvsl_values[addr & 0xf][1];
}
void PPUInterpreter::lvsr(PPUInstruction instr, PPUThread& thread)
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
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    thread.vr[instr.vd]._u64[0] = lvsr_values[addr & 0xf][0];
    thread.vr[instr.vd]._u64[1] = lvsr_values[addr & 0xf][1];
}
void PPUInterpreter::lvx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = (instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb]) & ~0xfULL;
    thread.vr[instr.vd]._u128 = nucleus.memory.read128(addr);
}
void PPUInterpreter::lvxl(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = (instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb]) & ~0xfULL;
    thread.vr[instr.vd]._u128 = nucleus.memory.read128(addr);
}
void PPUInterpreter::mfvscr(PPUInstruction instr, PPUThread& thread)
{
    thread.vr[instr.vd].clear();
    thread.vr[instr.vd]._u32[0] = thread.vscr.VSCR;
}
void PPUInterpreter::mtvscr(PPUInstruction instr, PPUThread& thread)
{
    thread.vscr.VSCR = thread.vr[instr.vb]._u32[0];
}
void PPUInterpreter::stvebx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.write8(addr, thread.vr[instr.vs]._u8[15 - eb]);
}
void PPUInterpreter::stvehx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = (instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb]) & ~1ULL;
    const u8 eb = (addr & 0xf) >> 1;
    nucleus.memory.write16(addr, thread.vr[instr.vs]._u16[7 - eb]);
}
void PPUInterpreter::stvewx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = (instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb]) & ~3ULL;
    const u8 eb = (addr & 0xf) >> 2;
    nucleus.memory.write32(addr, thread.vr[instr.vs]._u32[3 - eb]);
}
void PPUInterpreter::stvlx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.writeLeft(addr, thread.vr[instr.vs]._u8 + eb, 16 - eb);
}
void PPUInterpreter::stvlxl(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.writeLeft(addr, thread.vr[instr.vs]._u8 + eb, 16 - eb);
}
void PPUInterpreter::stvrx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.writeRight(addr - eb, thread.vr[instr.vs]._u8, eb);
}
void PPUInterpreter::stvrxl(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.writeRight(addr - eb, thread.vr[instr.vs]._u8, eb);
}
void PPUInterpreter::stvx(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = (instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb]) & ~0xfULL;
    nucleus.memory.write128(addr, thread.vr[instr.vs]._u128);
}
void PPUInterpreter::stvxl(PPUInstruction instr, PPUThread& thread)
{
    const u32 addr = (instr.ra ? thread.gpr[instr.ra] + thread.gpr[instr.rb] : thread.gpr[instr.rb]) & ~0xfULL;
    nucleus.memory.write128(addr, thread.vr[instr.vs]._u128);
}
void PPUInterpreter::vaddcuw(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = ~thread.vr[instr.va]._u32[w] < thread.vr[instr.vb]._u32[w];
    }
}
void PPUInterpreter::vaddfp(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._f32[w] = thread.vr[instr.va]._f32[w] + thread.vr[instr.vb]._f32[w];
    }
}
void PPUInterpreter::vaddsbs(PPUInstruction instr, PPUThread& thread)
{
    for (u32 b=0; b<16; ++b) {
        const s16 result = (s16)thread.vr[instr.va]._s8[b] + (s16)thread.vr[instr.vb]._s8[b];
        if (result > 0x7f) {
            thread.vr[instr.vd]._s8[b] = 0x7f;
            thread.vscr.SAT = 1;
        }
        else if (result < -0x80) {
            thread.vr[instr.vd]._s8[b] = -0x80;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[instr.vd]._s8[b] = result;
        }
    }
}
void PPUInterpreter::vaddshs(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        const s32 result = (s32)thread.vr[instr.va]._s16[h] + (s32)thread.vr[instr.vb]._s16[h];
        if (result > 0x7fff) {
            thread.vr[instr.vd]._s16[h] = 0x7fff;
            thread.vscr.SAT = 1;
        }
        else if (result < -0x8000) {
            thread.vr[instr.vd]._s16[h] = -0x8000;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[instr.vd]._s16[h] = result;
        }
    }
}
void PPUInterpreter::vaddsws(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        const s64 result = (s64)thread.vr[instr.va]._s32[w] + (s64)thread.vr[instr.vb]._s32[w];
        if (result > 0x7fffffff) {
            thread.vr[instr.vd]._s32[w] = 0x7fffffff;
            thread.vscr.SAT = 1;
        }
        else if (result < (s32)0x80000000) {
            thread.vr[instr.vd]._s32[w] = 0x80000000;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[instr.vd]._s32[w] = result;
        }
    }
}
void PPUInterpreter::vaddubm(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[instr.vd]._u8[b] = thread.vr[instr.va]._u8[b] + thread.vr[instr.vb]._u8[b];
    }
}
void PPUInterpreter::vaddubs(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        const u16 result = (u16)thread.vr[instr.va]._u8[b] + (u16)thread.vr[instr.vb]._u8[b];
        if (result > 0xff) {
            thread.vr[instr.vd]._u8[b] = 0xff;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[instr.vd]._u8[b] = result;
        }
    }
}
void PPUInterpreter::vadduhm(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._u16[h] = thread.vr[instr.va]._u16[h] + thread.vr[instr.vb]._u16[h];
    }
}
void PPUInterpreter::vadduhs(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        const u32 result = (u32)thread.vr[instr.va]._u16[h] + (u32)thread.vr[instr.vb]._u16[h];
        if (result > 0xffff) {
            thread.vr[instr.vd]._u16[h] = 0xffff;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[instr.vd]._u16[h] = result;
        }
    }
}
void PPUInterpreter::vadduwm(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = thread.vr[instr.va]._u32[w] + thread.vr[instr.vb]._u32[w];
    }
}
void PPUInterpreter::vadduws(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        const u64 result = (u64)thread.vr[instr.va]._u32[w] + (u64)thread.vr[instr.vb]._u32[w];
        if (result > 0xffffffff) {
            thread.vr[instr.vd]._u32[w] = 0xffffffff;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[instr.vd]._u32[w] = result;
        }
    }
}
void PPUInterpreter::vand(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = thread.vr[instr.va]._u32[w] & thread.vr[instr.vb]._u32[w];
    }
}
void PPUInterpreter::vandc(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = thread.vr[instr.va]._u32[w] & (~thread.vr[instr.vb]._u32[w]);
    }
}
void PPUInterpreter::vavgsb(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[instr.vd]._s8[b] = (thread.vr[instr.va]._s8[b] + thread.vr[instr.vb]._s8[b] + 1) >> 1;
    }
}
void PPUInterpreter::vavgsh(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._s16[h] = (thread.vr[instr.va]._s16[h] + thread.vr[instr.vb]._s16[h] + 1) >> 1;
    }
}
void PPUInterpreter::vavgsw(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._s32[w] = ((s64)thread.vr[instr.va]._s32[w] + (s64)thread.vr[instr.vb]._s32[w] + 1) >> 1;
    }
}
void PPUInterpreter::vavgub(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[instr.vd]._u8[b] = (thread.vr[instr.va]._u8[b] + thread.vr[instr.vb]._u8[b] + 1) >> 1;
    }
}
void PPUInterpreter::vavguh(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._u16[h] = (thread.vr[instr.va]._u16[h] + thread.vr[instr.vb]._u16[h] + 1) >> 1;
    }
}
void PPUInterpreter::vavguw(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = ((u64)thread.vr[instr.va]._u32[w] + (u64)thread.vr[instr.vb]._u32[w] + 1) >> 1;
    }
}
void PPUInterpreter::vcfsx(PPUInstruction instr, PPUThread& thread)
{
    const u32 scale = 1 << instr.vuimm;
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._f32[w] = ((f32)thread.vr[instr.vb]._s32[w]) / scale;
    }
}
void PPUInterpreter::vcfux(PPUInstruction instr, PPUThread& thread)
{
    const u32 scale = 1 << instr.vuimm;
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._f32[w] = ((f32)thread.vr[instr.vb]._u32[w]) / scale;
    }
}
void PPUInterpreter::vcmpbfp(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        u32 mask = 0;
        const f32 A = CheckVSCR_NJ(thread, thread.vr[instr.va]._f32[w]);
        const f32 B = CheckVSCR_NJ(thread, thread.vr[instr.vb]._f32[w]);

        if (A >  B) mask |= 1 << 31;
        if (A < -B) mask |= 1 << 30;

        thread.vr[instr.vd]._u32[w] = mask;
    }
}
void PPUInterpreter::vcmpbfp_(PPUInstruction instr, PPUThread& thread)
{
    bool allInBounds = true;
    for (int w = 0; w < 4; w++) {
        u32 mask = 0;
        const f32 A = CheckVSCR_NJ(thread, thread.vr[instr.va]._f32[w]);
        const f32 B = CheckVSCR_NJ(thread, thread.vr[instr.vb]._f32[w]);

        if (A >  B) mask |= 1 << 31;
        if (A < -B) mask |= 1 << 30;

        thread.vr[instr.vd]._u32[w] = mask;
        if (mask) {
            allInBounds = false;
        }
    }
    thread.cr.setBit(6*4 + 2, allInBounds);
}
void PPUInterpreter::vcmpeqfp(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = thread.vr[instr.va]._f32[w] == thread.vr[instr.vb]._f32[w] ? 0xffffffff : 0;
    }
}
void PPUInterpreter::vcmpeqfp_(PPUInstruction instr, PPUThread& thread)
{
    int all_equal = 0x8;
    int none_equal = 0x2;

    for (int w = 0; w < 4; w++) {
        if (thread.vr[instr.va]._f32[w] == thread.vr[instr.vb]._f32[w]) {
            thread.vr[instr.vd]._u32[w] = 0xffffffff;
            none_equal = 0;
        }
        else {
            thread.vr[instr.vd]._u32[w] = 0;
            all_equal = 0;
        }
    }

    thread.cr.setField(6, all_equal | none_equal);
}
void PPUInterpreter::vcmpequb(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[instr.vd]._u8[b] = thread.vr[instr.va]._u8[b] == thread.vr[instr.vb]._u8[b] ? 0xff : 0;
    }
}
void PPUInterpreter::vcmpequb_(PPUInstruction instr, PPUThread& thread)
{
    int all_equal = 0x8;
    int none_equal = 0x2;
        
    for (int b = 0; b < 16; b++) {
        if (thread.vr[instr.va]._u8[b] == thread.vr[instr.vb]._u8[b]) {
            thread.vr[instr.vd]._u8[b] = 0xff;
            none_equal = 0;
        }
        else {
            thread.vr[instr.vd]._u8[b] = 0;
            all_equal = 0;
        }
    }

    thread.cr.setField(6, all_equal | none_equal);
}
void PPUInterpreter::vcmpequh(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._u16[h] = thread.vr[instr.va]._u16[h] == thread.vr[instr.vb]._u16[h] ? 0xffff : 0;
    }
}
void PPUInterpreter::vcmpequh_(PPUInstruction instr, PPUThread& thread)
{
    int all_equal = 0x8;
    int none_equal = 0x2;
        
    for (int h = 0; h < 8; h++) {
        if (thread.vr[instr.va]._u16[h] == thread.vr[instr.vb]._u16[h]) {
            thread.vr[instr.vd]._u16[h] = 0xffff;
            none_equal = 0;
        }
        else {
            thread.vr[instr.vd]._u16[h] = 0;
            all_equal = 0;
        }
    }
        
    thread.cr.setField(6, all_equal | none_equal);
}
void PPUInterpreter::vcmpequw(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = thread.vr[instr.va]._u32[w] == thread.vr[instr.vb]._u32[w] ? 0xffffffff : 0;
    }
}
void PPUInterpreter::vcmpequw_(PPUInstruction instr, PPUThread& thread)
{
    int all_equal = 0x8;
    int none_equal = 0x2;
        
    for (int w = 0; w < 4; w++) {
        if (thread.vr[instr.va]._u32[w] == thread.vr[instr.vb]._u32[w]) {
            thread.vr[instr.vd]._u32[w] = 0xffffffff;
            none_equal = 0;
        }
        else {
            thread.vr[instr.vd]._u32[w] = 0;
            all_equal = 0;
        }
    }

    thread.cr.setField(6, all_equal | none_equal);
}
void PPUInterpreter::vcmpgefp(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = thread.vr[instr.va]._f32[w] >= thread.vr[instr.vb]._f32[w] ? 0xffffffff : 0;
    }
}
void PPUInterpreter::vcmpgefp_(PPUInstruction instr, PPUThread& thread)
{
    int all_ge = 0x8;
    int none_ge = 0x2;
        
    for (int w = 0; w < 4; w++) {
        if (thread.vr[instr.va]._f32[w] >= thread.vr[instr.vb]._f32[w]) {
            thread.vr[instr.vd]._u32[w] = 0xffffffff;
            none_ge = 0;
        }
        else {
            thread.vr[instr.vd]._u32[w] = 0;
            all_ge = 0;
        }
    }

    thread.cr.setField(6, all_ge | none_ge);
}
void PPUInterpreter::vcmpgtfp(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = thread.vr[instr.va]._f32[w] > thread.vr[instr.vb]._f32[w] ? 0xffffffff : 0;
    }
}
void PPUInterpreter::vcmpgtfp_(PPUInstruction instr, PPUThread& thread)
{
    int all_ge = 0x8;
    int none_ge = 0x2;
        
    for (int w = 0; w < 4; w++) {
        if (thread.vr[instr.va]._f32[w] > thread.vr[instr.vb]._f32[w]) {
            thread.vr[instr.vd]._u32[w] = 0xffffffff;
            none_ge = 0;
        }
        else {
            thread.vr[instr.vd]._u32[w] = 0;
            all_ge = 0;
        }
    }

    thread.cr.setField(6, all_ge | none_ge);
}
void PPUInterpreter::vcmpgtsb(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[instr.vd]._u8[b] = thread.vr[instr.va]._s8[b] > thread.vr[instr.vb]._s8[b] ? 0xff : 0;
    }
}
void PPUInterpreter::vcmpgtsb_(PPUInstruction instr, PPUThread& thread)
{
    int all_gt = 0x8;
    int none_gt = 0x2;
        
    for (int b = 0; b < 16; b++) {
        if (thread.vr[instr.va]._s8[b] > thread.vr[instr.vb]._s8[b]) {
            thread.vr[instr.vd]._u8[b] = 0xff;
            none_gt = 0;
        }
        else {
            thread.vr[instr.vd]._u8[b] = 0;
            all_gt = 0;
        }
    }

    thread.cr.setField(6, all_gt | none_gt);
}
void PPUInterpreter::vcmpgtsh(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._u16[h] = thread.vr[instr.va]._s16[h] > thread.vr[instr.vb]._s16[h] ? 0xffff : 0;
    }
}
void PPUInterpreter::vcmpgtsh_(PPUInstruction instr, PPUThread& thread)
{
    int all_gt = 0x8;
    int none_gt = 0x2;
        
    for (int h = 0; h < 8; h++) {
        if (thread.vr[instr.va]._s16[h] > thread.vr[instr.vb]._s16[h]) {
            thread.vr[instr.vd]._u16[h] = 0xffff;
            none_gt = 0;
        }
        else {
            thread.vr[instr.vd]._u16[h] = 0;
            all_gt = 0;
        }
    }

    thread.cr.setField(6, all_gt | none_gt);
}
void PPUInterpreter::vcmpgtsw(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = thread.vr[instr.va]._s32[w] > thread.vr[instr.vb]._s32[w] ? 0xffffffff : 0;
    }
}
void PPUInterpreter::vcmpgtsw_(PPUInstruction instr, PPUThread& thread)
{
    int all_gt = 0x8;
    int none_gt = 0x2;
        
    for (int w = 0; w < 4; w++) {
        if (thread.vr[instr.va]._s32[w] > thread.vr[instr.vb]._s32[w]) {
            thread.vr[instr.vd]._u32[w] = 0xffffffff;
            none_gt = 0;
        }
        else {
            thread.vr[instr.vd]._u32[w] = 0;
            all_gt = 0;
        }
    }

    thread.cr.setField(6, all_gt | none_gt);
}
void PPUInterpreter::vcmpgtub(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[instr.vd]._u8[b] = thread.vr[instr.va]._u8[b] > thread.vr[instr.vb]._u8[b] ? 0xff : 0;
    }
}
void PPUInterpreter::vcmpgtub_(PPUInstruction instr, PPUThread& thread)
{
    int all_gt = 0x8;
    int none_gt = 0x2;

    for (int b = 0; b < 16; b++) {
        if (thread.vr[instr.va]._u8[b] > thread.vr[instr.vb]._u8[b]) {
            thread.vr[instr.vd]._u8[b] = 0xff;
            none_gt = 0;
        }
        else {
            thread.vr[instr.vd]._u8[b] = 0;
            all_gt = 0;
        }
    }

    thread.cr.setField(6, all_gt | none_gt);
}
void PPUInterpreter::vcmpgtuh(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._u16[h] = thread.vr[instr.va]._u16[h] > thread.vr[instr.vb]._u16[h] ? 0xffff : 0;
    }
}
void PPUInterpreter::vcmpgtuh_(PPUInstruction instr, PPUThread& thread)
{
    int all_gt = 0x8;
    int none_gt = 0x2;
        
    for (int h = 0; h < 8; h++) {
        if (thread.vr[instr.va]._u16[h] > thread.vr[instr.vb]._u16[h]) {
            thread.vr[instr.vd]._u16[h] = 0xffff;
            none_gt = 0;
        }
        else {
            thread.vr[instr.vd]._u16[h] = 0;
            all_gt = 0;
        }
    }

    thread.cr.setField(6, all_gt | none_gt);
}
void PPUInterpreter::vcmpgtuw(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = thread.vr[instr.va]._u32[w] > thread.vr[instr.vb]._u32[w] ? 0xffffffff : 0;
    }
}
void PPUInterpreter::vcmpgtuw_(PPUInstruction instr, PPUThread& thread)
{
    int all_gt = 0x8;
    int none_gt = 0x2;
        
    for (int w = 0; w < 4; w++) {
        if (thread.vr[instr.va]._u32[w] > thread.vr[instr.vb]._u32[w]) {
            thread.vr[instr.vd]._u32[w] = 0xffffffff;
            none_gt = 0;
        }
        else {
            thread.vr[instr.vd]._u32[w] = 0;
            all_gt = 0;
        }
    }

    thread.cr.setField(6, all_gt | none_gt);
}
void PPUInterpreter::vctsxs(PPUInstruction instr, PPUThread& thread)
{
    int nScale = 1 << instr.vuimm;
        
    for (int w = 0; w < 4; w++) {        
        f32 result = thread.vr[instr.vb]._f32[w] * nScale;

        if (result > 0x7fffffff)
            thread.vr[instr.vd]._s32[w] = 0x7fffffff;
        else if (result < -0x80000000LL)
            thread.vr[instr.vd]._s32[w] = -0x80000000LL;
        else {
            thread.vr[instr.vd]._s32[w] = (int)result;
        }
    }
}
void PPUInterpreter::vctuxs(PPUInstruction instr, PPUThread& thread)
{
    int nScale = 1 << instr.vuimm;

    for (int w = 0; w < 4; w++) {
        s64 result = (s64)(thread.vr[instr.vb]._f32[w] * nScale);

        if (result > 0xffffffff)
            thread.vr[instr.vd]._u32[w] = 0xffffffff;
        else if (result < 0)
            thread.vr[instr.vd]._u32[w] = 0;
        else
            thread.vr[instr.vd]._u32[w] = (u32)result;
    }
}
void PPUInterpreter::vexptefp(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._f32[w] = exp(thread.vr[instr.vb]._f32[w] * log(2.0f));
    }
}
void PPUInterpreter::vlogefp(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._f32[w] = log(thread.vr[instr.vb]._f32[w]) / log(2.0f);
    }
}
void PPUInterpreter::vmaddfp(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._f32[w] = thread.vr[instr.va]._f32[w] * thread.vr[instr.vc]._f32[w] + thread.vr[instr.vb]._f32[w];
    }
}
void PPUInterpreter::vmaxfp(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._f32[w] = std::max(thread.vr[instr.va]._f32[w], thread.vr[instr.vb]._f32[w]);
    }
}
void PPUInterpreter::vmaxsb(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 16; b++)
        thread.vr[instr.vd]._s8[b] = std::max(thread.vr[instr.va]._s8[b], thread.vr[instr.vb]._s8[b]);
}
void PPUInterpreter::vmaxsh(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._s16[h] = std::max(thread.vr[instr.va]._s16[h], thread.vr[instr.vb]._s16[h]);
    }
}
void PPUInterpreter::vmaxsw(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._s32[w] = std::max(thread.vr[instr.va]._s32[w], thread.vr[instr.vb]._s32[w]);
    }
}
void PPUInterpreter::vmaxub(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 16; b++)
        thread.vr[instr.vd]._u8[b] = std::max(thread.vr[instr.va]._u8[b], thread.vr[instr.vb]._u8[b]);
}
void PPUInterpreter::vmaxuh(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._u16[h] = std::max(thread.vr[instr.va]._u16[h], thread.vr[instr.vb]._u16[h]);
    }
}
void PPUInterpreter::vmaxuw(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = std::max(thread.vr[instr.va]._u32[w], thread.vr[instr.vb]._u32[w]);
    }
}
void PPUInterpreter::vmhaddshs(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        s32 result = (s32)thread.vr[instr.va]._s16[h] * (s32)thread.vr[instr.vb]._s16[h] + (s32)thread.vr[instr.vc]._s16[h];
        if (result > INT16_MAX) {
            thread.vr[instr.vd]._s16[h] = (s16)INT16_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < INT16_MIN) {
            thread.vr[instr.vd]._s16[h] = (s16)INT16_MIN;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[instr.vd]._s16[h] = (s16)result;
        }
    }
}
void PPUInterpreter::vmhraddshs(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        s32 result = (s32)thread.vr[instr.va]._s16[h] * (s32)thread.vr[instr.vb]._s16[h] + (s32)thread.vr[instr.vc]._s16[h] + 0x4000;

        if (result > INT16_MAX) {
            thread.vr[instr.vd]._s16[h] = (s16)INT16_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < INT16_MIN) {
            thread.vr[instr.vd]._s16[h] = (s16)INT16_MIN;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[instr.vd]._s16[h] = (s16)result;
        }
    }
}
void PPUInterpreter::vminfp(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._f32[w] = std::min(thread.vr[instr.va]._f32[w], thread.vr[instr.vb]._f32[w]);
    }
}
void PPUInterpreter::vminsb(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[instr.vd]._s8[b] = std::min(thread.vr[instr.va]._s8[b], thread.vr[instr.vb]._s8[b]);
    }
}
void PPUInterpreter::vminsh(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._s16[h] = std::min(thread.vr[instr.va]._s16[h], thread.vr[instr.vb]._s16[h]);
    }
}
void PPUInterpreter::vminsw(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._s32[w] = std::min(thread.vr[instr.va]._s32[w], thread.vr[instr.vb]._s32[w]);
    }
}
void PPUInterpreter::vminub(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[instr.vd]._u8[b] = std::min(thread.vr[instr.va]._u8[b], thread.vr[instr.vb]._u8[b]);
    }
}
void PPUInterpreter::vminuh(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._u16[h] = std::min(thread.vr[instr.va]._u16[h], thread.vr[instr.vb]._u16[h]);
    }
}
void PPUInterpreter::vminuw(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = std::min(thread.vr[instr.va]._u32[w], thread.vr[instr.vb]._u32[w]);
    }
}
void PPUInterpreter::vmladduhm(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._u16[h] = thread.vr[instr.va]._u16[h] * thread.vr[instr.vb]._u16[h] + thread.vr[instr.vc]._u16[h];
    }
}
void PPUInterpreter::vmrghb(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._u8[15 - h*2 + 0] = thread.vr[instr.va]._u8[15 - h];
        thread.vr[instr.vd]._u8[15 - h*2 - 1] = thread.vr[instr.vb]._u8[15 - h];
    }
}
void PPUInterpreter::vmrghh(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u16[7 - w*2 + 0] = thread.vr[instr.va]._u16[7 - w];
        thread.vr[instr.vd]._u16[7 - w*2 - 1] = thread.vr[instr.vb]._u16[7 - w];
    }
}
void PPUInterpreter::vmrghw(PPUInstruction instr, PPUThread& thread)
{
    for (int d = 0; d < 2; d++) {
        thread.vr[instr.vd]._u32[3 - d*2 + 0] = thread.vr[instr.va]._u32[3 - d];
        thread.vr[instr.vd]._u32[3 - d*2 - 1] = thread.vr[instr.vb]._u32[3 - d];
    }
}
void PPUInterpreter::vmrglb(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._u8[15 - h*2 + 0] = thread.vr[instr.va]._u8[7 - h];
        thread.vr[instr.vd]._u8[15 - h*2 - 1] = thread.vr[instr.vb]._u8[7 - h];
    }
}
void PPUInterpreter::vmrglh(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u16[7 - w*2] = thread.vr[instr.va]._u16[3 - w];
        thread.vr[instr.vd]._u16[7 - w*2 - 1] = thread.vr[instr.vb]._u16[3 - w];
    }
}
void PPUInterpreter::vmrglw(PPUInstruction instr, PPUThread& thread)
{
    for (int d = 0; d < 2; d++) {
        thread.vr[instr.vd]._u32[3 - d*2 + 0] = thread.vr[instr.va]._u32[1 - d];
        thread.vr[instr.vd]._u32[3 - d*2 - 1] = thread.vr[instr.vb]._u32[1 - d];
    }
}
void PPUInterpreter::vmsummbm(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        s32 result = 0;
        for (int b = 0; b < 4; b++) {
            result += thread.vr[instr.va]._s8[w*4 + b] * thread.vr[instr.vb]._u8[w*4 + b];
        }
        result += thread.vr[instr.vc]._s32[w];
        thread.vr[instr.vd]._s32[w] = result;
    }
}
void PPUInterpreter::vmsumshm(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        s32 result = 0;
        for (int h = 0; h < 2; h++) {
            result += thread.vr[instr.va]._s16[w*2 + h] * thread.vr[instr.vb]._s16[w*2 + h];
        }
        result += thread.vr[instr.vc]._s32[w];
        thread.vr[instr.vd]._s32[w] = result;
    }
}
void PPUInterpreter::vmsumshs(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        s64 result = 0;
        for (int h = 0; h < 2; h++) {
            result += thread.vr[instr.va]._s16[w*2 + h] * thread.vr[instr.vb]._s16[w*2 + h];
        }
        result += thread.vr[instr.vc]._s32[w];

        if (result > 0x7fffffff) {
            thread.vr[instr.vd]._s32[w] = 0x7fffffff;
            thread.vscr.SAT = 1;
        }
        else if (result < -0x80000000LL) {
            thread.vr[instr.vd]._s32[w] = -0x80000000LL;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[instr.vd]._s32[w] = (s32)result;
        }
    }
}
void PPUInterpreter::vmsumubm(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        u32 result = 0;
        for (int b = 0; b < 4; b++) {
            result += thread.vr[instr.va]._u8[w*4 + b] * thread.vr[instr.vb]._u8[w*4 + b];
        }
        result += thread.vr[instr.vc]._u32[w];
        thread.vr[instr.vd]._u32[w] = result;
    }
}
void PPUInterpreter::vmsumuhm(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        u32 result = 0;

        for (int h = 0; h < 2; h++) {
            result += thread.vr[instr.va]._u16[w*2 + h] * thread.vr[instr.vb]._u16[w*2 + h];
        }

        result += thread.vr[instr.vc]._u32[w];
        thread.vr[instr.vd]._u32[w] = result;
    }
}
void PPUInterpreter::vmsumuhs(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        u64 result = 0;
        for (int h = 0; h < 2; h++) {
            result += thread.vr[instr.va]._u16[w*2 + h] * thread.vr[instr.vb]._u16[w*2 + h];
        }
        result += thread.vr[instr.vc]._u32[w];

        if (result > 0xffffffff) {
            thread.vr[instr.vd]._u32[w] = 0xffffffff;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[instr.vd]._u32[w] = (u32)result;
        }
    }
}
void PPUInterpreter::vmulesb(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._s16[h] = (s16)thread.vr[instr.va]._s8[h*2+1] * (s16)thread.vr[instr.vb]._s8[h*2+1];
    }
}
void PPUInterpreter::vmulesh(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._s32[w] = (s32)thread.vr[instr.va]._s16[w*2+1] * (s32)thread.vr[instr.vb]._s16[w*2+1];
    }
}
void PPUInterpreter::vmuleub(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._u16[h] = (u16)thread.vr[instr.va]._u8[h*2+1] * (u16)thread.vr[instr.vb]._u8[h*2+1];
    }
}
void PPUInterpreter::vmuleuh(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = (u32)thread.vr[instr.va]._u16[w*2+1] * (u32)thread.vr[instr.vb]._u16[w*2+1];
    }
}
void PPUInterpreter::vmulosb(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._s16[h] = (s16)thread.vr[instr.va]._s8[h*2] * (s16)thread.vr[instr.vb]._s8[h*2];
    }
}
void PPUInterpreter::vmulosh(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._s32[w] = (s32)thread.vr[instr.va]._s16[w*2] * (s32)thread.vr[instr.vb]._s16[w*2];
    }
}
void PPUInterpreter::vmuloub(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._u16[h] = (u16)thread.vr[instr.va]._u8[h*2] * (u16)thread.vr[instr.vb]._u8[h*2];
    }
}
void PPUInterpreter::vmulouh(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = (u32)thread.vr[instr.va]._u16[w*2] * (u32)thread.vr[instr.vb]._u16[w*2];
    }
}
void PPUInterpreter::vnmsubfp(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._f32[w] = -(thread.vr[instr.va]._f32[w] * thread.vr[instr.vc]._f32[w] - thread.vr[instr.vb]._f32[w]);
    }
}
void PPUInterpreter::vnor(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = ~(thread.vr[instr.va]._u32[w] | thread.vr[instr.vb]._u32[w]);
    }
}
void PPUInterpreter::vor(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = thread.vr[instr.va]._u32[w] | thread.vr[instr.vb]._u32[w];
    }
}
void PPUInterpreter::vperm(PPUInstruction instr, PPUThread& thread)
{
    u8 tmpSRC[32];
    memcpy(tmpSRC, thread.vr[instr.vb]._u8, 16);
    memcpy(tmpSRC + 16, thread.vr[instr.va]._u8, 16);

    for (int b = 0; b < 16; b++) {
        u8 index = thread.vr[instr.vc]._u8[b] & 0x1f;
            
        thread.vr[instr.vd]._u8[b] = tmpSRC[0x1f - index];
    }
}
void PPUInterpreter::vpkpx(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 4; h++) {
        const u16 bb7  = thread.vr[instr.vb]._u8[15 - (h*4 + 0)] & 0x1;
        const u16 bb8  = thread.vr[instr.vb]._u8[15 - (h*4 + 1)] >> 3;
        const u16 bb16 = thread.vr[instr.vb]._u8[15 - (h*4 + 2)] >> 3;
        const u16 bb24 = thread.vr[instr.vb]._u8[15 - (h*4 + 3)] >> 3;
        const u16 ab7  = thread.vr[instr.va]._u8[15 - (h*4 + 0)] & 0x1;
        const u16 ab8  = thread.vr[instr.va]._u8[15 - (h*4 + 1)] >> 3;
        const u16 ab16 = thread.vr[instr.va]._u8[15 - (h*4 + 2)] >> 3;
        const u16 ab24 = thread.vr[instr.va]._u8[15 - (h*4 + 3)] >> 3;

        thread.vr[instr.vd]._u16[3 - h] = (bb7 << 15) | (bb8 << 10) | (bb16 << 5) | bb24;
        thread.vr[instr.vd]._u16[7 - h] = (ab7 << 15) | (ab8 << 10) | (ab16 << 5) | ab24;
    }
}
void PPUInterpreter::vpkshss(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 8; b++) {
        s16 result = thread.vr[instr.va]._s16[b];

        if (result > INT8_MAX) {
            result = INT8_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < INT8_MIN) {
            result = INT8_MIN;
            thread.vscr.SAT = 1;
        }
            
        thread.vr[instr.vd]._s8[b+8] = result;

        result = thread.vr[instr.vb]._s16[b];

        if (result > INT8_MAX) {
            result = INT8_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < INT8_MIN) {
            result = INT8_MIN;
            thread.vscr.SAT = 1;
        }

        thread.vr[instr.vd]._s8[b] = result;
    }
}
void PPUInterpreter::vpkshus(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 8; b++) {
        s16 result = thread.vr[instr.va]._s16[b];

        if (result > UINT8_MAX) {
            result = UINT8_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < 0) {
            result = 0;
            thread.vscr.SAT = 1;
        }
            
        thread.vr[instr.vd]._u8[b+8] = result;

        result = thread.vr[instr.vb]._s16[b];

        if (result > UINT8_MAX) {
            result = UINT8_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < 0) {
            result = 0;
            thread.vscr.SAT = 1;
        }
            
        thread.vr[instr.vd]._u8[b] = result;
    }
}
void PPUInterpreter::vpkswss(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 4; h++) {
        s32 result = thread.vr[instr.va]._s32[h];

        if (result > INT16_MAX) {
            result = INT16_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < INT16_MIN) {
            result = INT16_MIN;
            thread.vscr.SAT = 1;
        }
            
        thread.vr[instr.vd]._s16[h+4] = result;

        result = thread.vr[instr.vb]._s32[h];

        if (result > INT16_MAX) {
            result = INT16_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < INT16_MIN) {
            result = INT16_MIN;
            thread.vscr.SAT = 1;
        }
            
        thread.vr[instr.vd]._s16[h] = result;
    }
}
void PPUInterpreter::vpkswus(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 4; h++) {
        s32 result = thread.vr[instr.va]._s32[h];

        if (result > UINT16_MAX) {
            result = UINT16_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < 0) {
            result = 0;
            thread.vscr.SAT = 1;
        }
            
        thread.vr[instr.vd]._u16[h+4] = result;

        result = thread.vr[instr.vb]._s32[h];

        if (result > UINT16_MAX) {
            result = UINT16_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < 0) {
            result = 0;
            thread.vscr.SAT = 1;
        }
            
        thread.vr[instr.vd]._u16[h] = result;
    }
}
void PPUInterpreter::vpkuhum(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 8; b++) {
        thread.vr[instr.vd]._u8[b+8] = thread.vr[instr.va]._u8[b*2];
        thread.vr[instr.vd]._u8[b  ] = thread.vr[instr.vb]._u8[b*2];
    }
}
void PPUInterpreter::vpkuhus(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 8; b++) {
        u16 result = thread.vr[instr.va]._u16[b];

        if (result > UINT8_MAX) {
            result = UINT8_MAX;
            thread.vscr.SAT = 1;
        }
            
        thread.vr[instr.vd]._u8[b+8] = result;

        result = thread.vr[instr.vb]._u16[b];

        if (result > UINT8_MAX) {
            result = UINT8_MAX;
            thread.vscr.SAT = 1;
        }
            
        thread.vr[instr.vd]._u8[b] = result;
    }
}
void PPUInterpreter::vpkuwum(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 4; h++) {
        thread.vr[instr.vd]._u16[h+4] = thread.vr[instr.va]._u16[h*2];
        thread.vr[instr.vd]._u16[h  ] = thread.vr[instr.vb]._u16[h*2];
    }
}
void PPUInterpreter::vpkuwus(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 4; h++) {
        u32 result = thread.vr[instr.va]._u32[h];

        if (result > UINT16_MAX) {
            result = UINT16_MAX;
            thread.vscr.SAT = 1;
        }
            
        thread.vr[instr.vd]._u16[h+4] = result;

        result = thread.vr[instr.vb]._u32[h];

        if (result > UINT16_MAX) {
            result = UINT16_MAX;
            thread.vscr.SAT = 1;
        }
            
        thread.vr[instr.vd]._u16[h] = result;
    }
}
void PPUInterpreter::vrefp(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._f32[w] = 1.0f / thread.vr[instr.vb]._f32[w];
    }
}
void PPUInterpreter::vrfim(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._f32[w] = floor(thread.vr[instr.vb]._f32[w]);
    }
}
void PPUInterpreter::vrfin(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._f32[w] = floor(thread.vr[instr.vb]._f32[w] + 0.5f);
    }
}
void PPUInterpreter::vrfip(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._f32[w] = ceil(thread.vr[instr.vb]._f32[w]);
    }
}
void PPUInterpreter::vrfiz(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        f32 f;
        modff(thread.vr[instr.vb]._f32[w], &f);
        thread.vr[instr.vd]._f32[w] = f;
    }
}
void PPUInterpreter::vrlb(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        int nRot = thread.vr[instr.vb]._u8[b] & 0x7;

        thread.vr[instr.vd]._u8[b] = (thread.vr[instr.va]._u8[b] << nRot) | (thread.vr[instr.va]._u8[b] >> (8 - nRot));
    }
}
void PPUInterpreter::vrlh(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._u16[h] = rotl16(thread.vr[instr.va]._u16[h], thread.vr[instr.vb]._u8[h*2] & 0xf);
    }
}
void PPUInterpreter::vrlw(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = rotl32(thread.vr[instr.va]._u32[w], thread.vr[instr.vb]._u8[w*4] & 0x1f);
    }
}
void PPUInterpreter::vrsqrtefp(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._f32[w] = 1.0f / sqrtf(thread.vr[instr.vb]._f32[w]);
    }
}
void PPUInterpreter::vsel(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[instr.vd]._u8[b] = (thread.vr[instr.vb]._u8[b] & thread.vr[instr.vc]._u8[b]) | (thread.vr[instr.va]._u8[b] & (~thread.vr[instr.vc]._u8[b]));
    }
}
void PPUInterpreter::vsl(PPUInstruction instr, PPUThread& thread)
{
    u8 sh = thread.vr[instr.vb]._u8[0] & 0x7;

    u32 t = 1;

    for (int b = 0; b < 16; b++) {
        t &= (thread.vr[instr.vb]._u8[b] & 0x7) == sh;
    }

    if (t) {
        thread.vr[instr.vd]._u8[0] = thread.vr[instr.va]._u8[0] << sh;

        for (int b = 1; b < 16; b++) {
            thread.vr[instr.vd]._u8[b] = (thread.vr[instr.va]._u8[b] << sh) | (thread.vr[instr.va]._u8[b-1] >> (8 - sh));
        }
    }
}
void PPUInterpreter::vslb(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[instr.vd]._u8[b] = thread.vr[instr.va]._u8[b] << (thread.vr[instr.vb]._u8[b] & 0x7);
    }
}
void PPUInterpreter::vsldoi(PPUInstruction instr, PPUThread& thread)
{
    u8 tmpSRC[32];
    memcpy(tmpSRC, thread.vr[instr.vb]._u8, 16);
    memcpy(tmpSRC + 16, thread.vr[instr.va]._u8, 16);
    for (int b = 0; b < 16; b++) {
        thread.vr[instr.vd]._u8[15 - b] = tmpSRC[31 - (b + instr.sh)];
    }
}
void PPUInterpreter::vslh(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._u16[h] = thread.vr[instr.va]._u16[h] << (thread.vr[instr.vb]._u8[h*2] & 0xf);
    }
}
void PPUInterpreter::vslo(PPUInstruction instr, PPUThread& thread)
{
    u8 nShift = (thread.vr[instr.vb]._u8[0] >> 3) & 0xf;

    thread.vr[instr.vd].clear();

    for (u8 b = 0; b < 16 - nShift; b++) {
        thread.vr[instr.vd]._u8[15 - b] = thread.vr[instr.va]._u8[15 - (b + nShift)];
    }
}
void PPUInterpreter::vslw(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = thread.vr[instr.va]._u32[w] << (thread.vr[instr.vb]._u32[w] & 0x1f);
    }
}
void PPUInterpreter::vspltb(PPUInstruction instr, PPUThread& thread)
{
    u8 byte = thread.vr[instr.vb]._u8[15 - instr.vuimm];
        
    for (int b = 0; b < 16; b++) {
        thread.vr[instr.vd]._u8[b] = byte;
    }
}
void PPUInterpreter::vsplth(PPUInstruction instr, PPUThread& thread)
{
    const u16 hword = thread.vr[instr.vb]._u16[7 - instr.vuimm];
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._u16[h] = hword;
    }
}
void PPUInterpreter::vspltisb(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[instr.vd]._u8[b] = instr.vsimm;
    }
}
void PPUInterpreter::vspltish(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._u16[h] = (s16)instr.vsimm;
    }
}
void PPUInterpreter::vspltisw(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = (s32)instr.vsimm;
    }
}
void PPUInterpreter::vspltw(PPUInstruction instr, PPUThread& thread)
{
    const u32 word = thread.vr[instr.vb]._u32[3 - instr.vuimm];       
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = word;
    }
}
void PPUInterpreter::vsr(PPUInstruction instr, PPUThread& thread)
{
    u8 sh = thread.vr[instr.vb]._u8[0] & 0x7;
    u32 t = 1;

    for (int b = 0; b < 16; b++) {
        t &= (thread.vr[instr.vb]._u8[b] & 0x7) == sh;
    }
    if (t) {
        thread.vr[instr.vd]._u8[15] = thread.vr[instr.va]._u8[15] >> sh;

        for (int b = 0; b < 15; b++) {
            thread.vr[instr.vd]._u8[14-b] = (thread.vr[instr.va]._u8[14-b] >> sh) | (thread.vr[instr.va]._u8[14-b+1] << (8 - sh));
        }
    }
}
void PPUInterpreter::vsrab(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[instr.vd]._s8[b] = thread.vr[instr.va]._s8[b] >> (thread.vr[instr.vb]._u8[b] & 0x7);
    }
}
void PPUInterpreter::vsrah(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._s16[h] = thread.vr[instr.va]._s16[h] >> (thread.vr[instr.vb]._u8[h*2] & 0xf);
    }
}
void PPUInterpreter::vsraw(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._s32[w] = thread.vr[instr.va]._s32[w] >> (thread.vr[instr.vb]._u8[w*4] & 0x1f);
    }
}
void PPUInterpreter::vsrb(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[instr.vd]._u8[b] = thread.vr[instr.va]._u8[b] >> (thread.vr[instr.vb]._u8[b] & 0x7);
    }
}
void PPUInterpreter::vsrh(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._u16[h] = thread.vr[instr.va]._u16[h] >> (thread.vr[instr.vb]._u8[h*2] & 0xf);
    }
}
void PPUInterpreter::vsro(PPUInstruction instr, PPUThread& thread)
{
    const u8 nShift = (thread.vr[instr.vb]._u8[0] >> 3) & 0xf;
    thread.vr[instr.vd].clear();

    for (u8 b = 0; b < 16 - nShift; b++) {
        thread.vr[instr.vd]._u8[b] = thread.vr[instr.va]._u8[b + nShift];
    }
}
void PPUInterpreter::vsrw(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = thread.vr[instr.va]._u32[w] >> (thread.vr[instr.vb]._u8[w*4] & 0x1f);
    }
}
void PPUInterpreter::vsubcuw(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = thread.vr[instr.va]._u32[w] < thread.vr[instr.vb]._u32[w] ? 0 : 1;
    }
}
void PPUInterpreter::vsubfp(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._f32[w] = thread.vr[instr.va]._f32[w] - thread.vr[instr.vb]._f32[w];
    }
}
void PPUInterpreter::vsubsbs(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        s16 result = (s16)thread.vr[instr.va]._s8[b] - (s16)thread.vr[instr.vb]._s8[b];

        if (result < INT8_MIN) {
            thread.vr[instr.vd]._s8[b] = INT8_MIN;
            thread.vscr.SAT = 1;
        }
        else if (result > INT8_MAX) {
            thread.vr[instr.vd]._s8[b] = INT8_MAX;
            thread.vscr.SAT = 1;
        }
        else
            thread.vr[instr.vd]._s8[b] = (s8)result;
    }
}
void PPUInterpreter::vsubshs(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        s32 result = (s32)thread.vr[instr.va]._s16[h] - (s32)thread.vr[instr.vb]._s16[h];

        if (result < INT16_MIN) {
            thread.vr[instr.vd]._s16[h] = (s16)INT16_MIN;
            thread.vscr.SAT = 1;
        }
        else if (result > INT16_MAX) {
            thread.vr[instr.vd]._s16[h] = (s16)INT16_MAX;
            thread.vscr.SAT = 1;
        }
        else
            thread.vr[instr.vd]._s16[h] = (s16)result;
    }
}
void PPUInterpreter::vsubsws(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        s64 result = (s64)thread.vr[instr.va]._s32[w] - (s64)thread.vr[instr.vb]._s32[w];

        if (result < INT32_MIN) {
            thread.vr[instr.vd]._s32[w] = (s32)INT32_MIN;
            thread.vscr.SAT = 1;
        }
        else if (result > INT32_MAX) {
            thread.vr[instr.vd]._s32[w] = (s32)INT32_MAX;
            thread.vscr.SAT = 1;
        }
        else
            thread.vr[instr.vd]._s32[w] = (s32)result;
    }
}
void PPUInterpreter::vsububm(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[instr.vd]._u8[b] = (u8)((thread.vr[instr.va]._u8[b] - thread.vr[instr.vb]._u8[b]) & 0xff);
    }
}
void PPUInterpreter::vsububs(PPUInstruction instr, PPUThread& thread)
{
    for (int b = 0; b < 16; b++) {
        s16 result = (s16)thread.vr[instr.va]._u8[b] - (s16)thread.vr[instr.vb]._u8[b];

        if (result < 0) {
            thread.vr[instr.vd]._u8[b] = 0;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[instr.vd]._u8[b] = (u8)result;
        }
    }
}
void PPUInterpreter::vsubuhm(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._u16[h] = thread.vr[instr.va]._u16[h] - thread.vr[instr.vb]._u16[h];
    }
}
void PPUInterpreter::vsubuhs(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        s32 result = (s32)thread.vr[instr.va]._u16[h] - (s32)thread.vr[instr.vb]._u16[h];

        if (result < 0) {
            thread.vr[instr.vd]._u16[h] = 0;
            thread.vscr.SAT = 1;
        }
        else
            thread.vr[instr.vd]._u16[h] = (u16)result;
    }
}
void PPUInterpreter::vsubuwm(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._u32[w] = thread.vr[instr.va]._u32[w] - thread.vr[instr.vb]._u32[w];
    }
}
void PPUInterpreter::vsubuws(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        s64 result = (s64)thread.vr[instr.va]._u32[w] - (s64)thread.vr[instr.vb]._u32[w];

        if (result < 0) {
            thread.vr[instr.vd]._u32[w] = 0;
            thread.vscr.SAT = 1;
        }
        else
            thread.vr[instr.vd]._u32[w] = (u32)result;
    }
}
void PPUInterpreter::vsum2sws(PPUInstruction instr, PPUThread& thread)
{
    for (int n = 0; n < 2; n++) {
        s64 sum = (s64)thread.vr[instr.va]._s32[n*2] + thread.vr[instr.va]._s32[n*2 + 1] + thread.vr[instr.vb]._s32[n*2];

        if (sum > INT32_MAX) {
            thread.vr[instr.vd]._s32[n*2] = (s32)INT32_MAX;
            thread.vscr.SAT = 1;
        }
        else if (sum < INT32_MIN) {
            thread.vr[instr.vd]._s32[n*2] = (s32)INT32_MIN;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[instr.vd]._s32[n*2] = (s32)sum;
        }
    }
    thread.vr[instr.vd]._s32[1] = 0;
    thread.vr[instr.vd]._s32[3] = 0;
}
void PPUInterpreter::vsum4sbs(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        s64 sum = thread.vr[instr.vb]._s32[w];

        for (int b = 0; b < 4; b++) {
            sum += thread.vr[instr.va]._s8[w*4 + b];
        }

        if (sum > INT32_MAX) {
            thread.vr[instr.vd]._s32[w] = (s32)INT32_MAX;
            thread.vscr.SAT = 1;
        }
        else if (sum < INT32_MIN) {
            thread.vr[instr.vd]._s32[w] = (s32)INT32_MIN;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[instr.vd]._s32[w] = (s32)sum;
        }
    }
}
void PPUInterpreter::vsum4shs(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        s64 sum = thread.vr[instr.vb]._s32[w];

        for (int h = 0; h < 2; h++) {
            sum += thread.vr[instr.va]._s16[w*2 + h];
        }

        if (sum > INT32_MAX) {
            thread.vr[instr.vd]._s32[w] = (s32)INT32_MAX;
            thread.vscr.SAT = 1;
        }
        else if (sum < INT32_MIN) {
            thread.vr[instr.vd]._s32[w] = (s32)INT32_MIN;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[instr.vd]._s32[w] = (s32)sum;
        }
    }
}
void PPUInterpreter::vsum4ubs(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        u64 sum = thread.vr[instr.vb]._u32[w];

        for (int b = 0; b < 4; b++) {
            sum += thread.vr[instr.va]._u8[w*4 + b];
        }

        if (sum > UINT32_MAX) {
            thread.vr[instr.vd]._u32[w] = (u32)UINT32_MAX;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[instr.vd]._u32[w] = (u32)sum;
        }
    }
}
void PPUInterpreter::vsumsws(PPUInstruction instr, PPUThread& thread)
{
    thread.vr[instr.vd].clear();
        
    s64 sum = thread.vr[instr.vb]._s32[3];

    for (int w = 0; w < 4; w++) {
        sum += thread.vr[instr.va]._s32[w];
    }

    if (sum > INT32_MAX) {
        thread.vr[instr.vd]._s32[0] = (s32)INT32_MAX;
        thread.vscr.SAT = 1;
    }
    else if (sum < INT32_MIN) {
        thread.vr[instr.vd]._s32[0] = (s32)INT32_MIN;
        thread.vscr.SAT = 1;
    }
    else
        thread.vr[instr.vd]._s32[0] = (s32)sum;
}
void PPUInterpreter::vupkhpx(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._s8[(3 - w)*4 + 3] = thread.vr[instr.vb]._s8[w*2 + 0] >> 7;
        thread.vr[instr.vd]._u8[(3 - w)*4 + 2] = (thread.vr[instr.vb]._u8[w*2 + 0] >> 2) & 0x1f;
        thread.vr[instr.vd]._u8[(3 - w)*4 + 1] = ((thread.vr[instr.vb]._u8[w*2 + 0] & 0x3) << 3) | ((thread.vr[instr.vb]._u8[w*2 + 1] >> 5) & 0x7);
        thread.vr[instr.vd]._u8[(3 - w)*4 + 0] = thread.vr[instr.vb]._u8[w*2 + 1] & 0x1f;
    }
}
void PPUInterpreter::vupkhsb(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._s16[h] = thread.vr[instr.vb]._s8[h];
    }
}
void PPUInterpreter::vupkhsh(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._s32[w] = thread.vr[instr.vb]._s16[w];
    }
}
void PPUInterpreter::vupklpx(PPUInstruction instr, PPUThread& thread)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[instr.vd]._s8[(3 - w)*4 + 3] = thread.vr[instr.vb]._s8[8 + w*2 + 0] >> 7;
        thread.vr[instr.vd]._u8[(3 - w)*4 + 2] = (thread.vr[instr.vb]._u8[8 + w*2 + 0] >> 2) & 0x1f;
        thread.vr[instr.vd]._u8[(3 - w)*4 + 1] = ((thread.vr[instr.vb]._u8[8 + w*2 + 0] & 0x3) << 3) | ((thread.vr[instr.vb]._u8[8 + w*2 + 1] >> 5) & 0x7);
        thread.vr[instr.vd]._u8[(3 - w)*4 + 0] = thread.vr[instr.vb]._u8[8 + w*2 + 1] & 0x1f;
    }
}
void PPUInterpreter::vupklsb(PPUInstruction instr, PPUThread& thread)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[instr.vd]._s16[h] = thread.vr[instr.vb]._s8[8 + h];
    }
}
void PPUInterpreter::vupklsh(PPUInstruction instr, PPUThread& thread)
{
    thread.vr[instr.vd]._s32[0] = thread.vr[instr.vb]._s16[4 + 0];
    thread.vr[instr.vd]._s32[1] = thread.vr[instr.vb]._s16[4 + 1];
    thread.vr[instr.vd]._s32[2] = thread.vr[instr.vb]._s16[4 + 2];
    thread.vr[instr.vd]._s32[3] = thread.vr[instr.vb]._s16[4 + 3];
}
void PPUInterpreter::vxor(PPUInstruction instr, PPUThread& thread)
{
    thread.vr[instr.vd]._u64[0] = thread.vr[instr.va]._u64[0] ^ thread.vr[instr.vb]._u64[0];
    thread.vr[instr.vd]._u32[1] = thread.vr[instr.va]._u64[1] ^ thread.vr[instr.vb]._u64[1];
}

// Unknown instruction
void PPUInterpreter::unknown(PPUInstruction instr, PPUThread& thread)
{
    std::cout << "Unknown instruction: " << instr.instruction << std::endl;
}
void PPUInterpreter::unknown2(const std::string& instruction)
{
}
