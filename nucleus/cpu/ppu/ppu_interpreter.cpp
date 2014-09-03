/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_interpreter.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/ppu/decoder.h"

#include <cmath>

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
}

void PPUInterpreter::step()
{
    const u32 instruction = nucleus.memory.read32(thread.pc);
    //instruction = 0xFC00F02A;
    listMain.call(this, instruction);
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

float PPUInterpreter::CheckVSCR_NJ(const float v) const
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

bool PPUInterpreter::CheckCondition(u32 bo, u32 bi)
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

u64& PPUInterpreter::GetRegBySPR(u32 spr)
{
    const u32 n = (spr >> 5) | ((spr & 0x1f) << 5);

    switch (n) {
    case 0x001: return thread.xer.XER;
    case 0x008: return thread.lr;
    case 0x009: return thread.ctr;
    }

    unknown("GetRegBySPR error: Unknown SPR!");
    return thread.xer.XER;
}

/**
 * PPU Instructions
 */
void PPUInterpreter::add(u32 rd, u32 ra, u32 rb, u32 oe, bool rc)
{
    thread.gpr[rd] = thread.gpr[ra] + thread.gpr[rb];
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(CPU.GPR[rd]);
    if (oe) unknown("addo");
}
void PPUInterpreter::addc(u32 rd, u32 ra, u32 rb, u32 oe, bool rc)
{
    const s64 gpra = thread.gpr[ra];
    const s64 gprb = thread.gpr[rb];
    thread.gpr[rd] = gpra + gprb;
    thread.xer.CA = isCarry(gpra, gprb);
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(CPU.GPR[rd]);
    if (oe) unknown("addco"); 
}
void PPUInterpreter::adde(u32 rd, u32 ra, u32 rb, u32 oe, bool rc)
{
    const u64 gpra = thread.gpr[ra];
    const u64 gprb = thread.gpr[rb];
    if (thread.xer.CA) {
        if (gpra == ~0ULL) {
            thread.gpr[rd] = gprb;
            thread.xer.CA = 1;
        }
        else {
            thread.gpr[rd] = gpra + 1 + gprb;
            thread.xer.CA = isCarry(gpra + 1, gprb);
        }
    }
    else {
        thread.gpr[rd] = gpra + gprb;
        thread.xer.CA = isCarry(gpra, gprb);
    }
    if (rc) // TODO CPU.UpdateCR0<s64>(CPU.GPR[rd]);
    if (oe) unknown("addeo");
}
void PPUInterpreter::addi(u32 rd, u32 ra, s32 simm16)
{
    thread.gpr[rd] = ra ? ((s64)thread.gpr[ra] + simm16) : simm16;
}
void PPUInterpreter::addic(u32 rd, u32 ra, s32 simm16)
{
    const u64 gpra = thread.gpr[ra];
    thread.gpr[rd] = gpra + simm16;
    thread.xer.CA = isCarry(gpra, simm16);
}
void PPUInterpreter::addic_(u32 rd, u32 ra, s32 simm16)
{
    const u64 gpra = thread.gpr[ra];
    thread.gpr[rd] = gpra + simm16;
    thread.xer.CA = isCarry(gpra, simm16);
    // TODO: CPU.UpdateCR0<s64>(thread.gpr[rd]);
}
void PPUInterpreter::addis(u32 rd, u32 ra, s32 simm16)
{
    thread.gpr[rd] = ra ? ((s64)thread.gpr[ra] + (simm16 << 16)) : (simm16 << 16);
}
void PPUInterpreter::addme(u32 rd, u32 ra, u32 oe, bool rc)
{
    const s64 gpra = thread.gpr[ra];
    thread.gpr[rd] = gpra + thread.xer.CA - 1;
    thread.xer.CA |= gpra != 0;
    if (oe) unknown("addmeo");
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[rd]);
}
void PPUInterpreter::addze(u32 rd, u32 ra, u32 oe, bool rc)
{
    const u64 gpra = thread.gpr[ra];
    thread.gpr[rd] = gpra + thread.xer.CA;
    thread.xer.CA = isCarry(gpra, thread.xer.CA);
    if (oe) unknown("addzeo");
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[rd]);
}
void PPUInterpreter::and(u32 ra, u32 rs, u32 rb, bool rc)
{
    thread.gpr[ra] = thread.gpr[rs] & thread.gpr[rb];
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::andc(u32 ra, u32 rs, u32 rb, bool rc)
{
    thread.gpr[ra] = thread.gpr[rs] & ~thread.gpr[rb];
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::andi_(u32 ra, u32 rs, u32 uimm16)
{
    thread.gpr[ra] = thread.gpr[rs] & uimm16;
    // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::andis_(u32 ra, u32 rs, u32 uimm16)
{
    thread.gpr[ra] = thread.gpr[rs] & (uimm16 << 16);
    // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::b(s32 ll, u32 aa, u32 lk)
{
    if (lk) thread.lr = thread.pc + 4;
    thread.pc = (aa ? ll : thread.pc + ll) & ~0x3ULL;
}
void PPUInterpreter::bc(u32 bo, u32 bi, s32 bd, u32 aa, u32 lk)
{
    if (CheckCondition(bo, bi)) {
        if (lk) thread.lr = thread.pc + 4;
        thread.pc = (aa ? bd : thread.pc + bd) & ~0x3ULL;
    }
}
void PPUInterpreter::bcctr(u32 bo, u32 bi, u32 bh, u32 lk)
{
    if (bo & 0x10 || thread.cr.getBit(bi) == ((bo >> 3) & 1)) {
        if (lk) thread.lr = thread.pc + 4;
        thread.pc = thread.ctr & ~0x3ULL;
    }
}    
void PPUInterpreter::bclr(u32 bo, u32 bi, u32 bh, u32 lk)
{
    if (CheckCondition(bo, bi)) {
        const u32 newLR = thread.pc + 4;
        thread.pc = thread.lr & ~0x3ULL;
        if (lk) thread.lr = newLR;
    }
}
void PPUInterpreter::cmp(u32 crfd, u32 l, u32 ra, u32 rb)
{
    // TODO: CPU.UpdateCRnS(l, crfd, thread.gpr[ra], thread.gpr[rb]);
}
void PPUInterpreter::cmpi(u32 crfd, u32 l, u32 ra, s32 simm16)
{
    // TODO: CPU.UpdateCRnS(l, crfd, thread.gpr[ra], simm16);
}
void PPUInterpreter::cmpl(u32 crfd, u32 l, u32 ra, u32 rb)
{
    // TODO: CPU.UpdateCRnU(l, crfd, thread.gpr[ra], thread.gpr[rb]);
}
void PPUInterpreter::cmpli(u32 crfd, u32 l, u32 ra, u32 uimm16)
{
    // TODO: CPU.UpdateCRnU(l, crfd, thread.gpr[ra], uimm16);
}
void PPUInterpreter::cntlzd(u32 ra, u32 rs, bool rc)
{
    for (int i = 0; i < 64; i++) {
        if (thread.gpr[rs] & (1ULL << (63 - i))) {
            thread.gpr[ra] = i;
            break;
        }
    }   
    if (rc) thread.cr.setBit(thread.cr.CR_LT, false);
}
void PPUInterpreter::cntlzw(u32 ra, u32 rs, bool rc)
{
    for (int i = 0; i < 32; i++) {
        if (thread.gpr[rs] & (1ULL << (31 - i))) {
            thread.gpr[ra] = i;
            break;
        }
    }
    if (rc) thread.cr.setBit(thread.cr.CR_LT, false);
}
void PPUInterpreter::crand(u32 crbd, u32 crba, u32 crbb)
{
    const u8 value = thread.cr.getBit(crba) & thread.cr.getBit(crbb);
    thread.cr.setBit(crbd, value);
}
void PPUInterpreter::crandc(u32 crbd, u32 crba, u32 crbb)
{
    const u8 value = thread.cr.getBit(crba) & (1 ^ thread.cr.getBit(crbb));
    thread.cr.setBit(crbd, value);
}
void PPUInterpreter::creqv(u32 crbd, u32 crba, u32 crbb)
{
    const u8 value = 1 ^ (thread.cr.getBit(crba) ^ thread.cr.getBit(crbb));
    thread.cr.setBit(crbd, value);
}
void PPUInterpreter::crnand(u32 crbd, u32 crba, u32 crbb)
{
    const u8 value = 1 ^ (thread.cr.getBit(crba) & thread.cr.getBit(crbb));
    thread.cr.setBit(crbd, value);
}
void PPUInterpreter::crnor(u32 crbd, u32 crba, u32 crbb)
{
    const u8 value = 1 ^ (thread.cr.getBit(crba) | thread.cr.getBit(crbb));
    thread.cr.setBit(crbd, value);
}
void PPUInterpreter::cror(u32 crbd, u32 crba, u32 crbb)
{
    const u8 value = thread.cr.getBit(crba) | thread.cr.getBit(crbb);
    thread.cr.setBit(crbd, value);
}
void PPUInterpreter::crorc(u32 crbd, u32 crba, u32 crbb)
{
    const u8 value = thread.cr.getBit(crba) | (1 ^ thread.cr.getBit(crbb));
    thread.cr.setBit(crbd, value);
}
void PPUInterpreter::crxor(u32 crbd, u32 crba, u32 crbb)
{
    const u8 value = thread.cr.getBit(crba) ^ thread.cr.getBit(crbb);
    thread.cr.setBit(crbd, value);
}
void PPUInterpreter::dcbf(u32 ra, u32 rb)
{
    //unknown("dcbf", false);
    _mm_mfence();
}
void PPUInterpreter::dcbst(u32 ra, u32 rb)
{
    //unknown("dcbst", false);
    _mm_mfence();
}
void PPUInterpreter::dcbt(u32 ra, u32 rb, u32 th)
{
    //unknown("dcbt", false);
    _mm_mfence();
}
void PPUInterpreter::dcbtst(u32 ra, u32 rb, u32 th)
{
    //unknown("dcbtst", false);
    _mm_mfence();
}
void PPUInterpreter::dcbz(u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    void* cache_line = nucleus.memory + (addr & ~127);
    if (cache_line) {
        memset(cache_line, 0, 128);
    }
    _mm_mfence();
}
void PPUInterpreter::divd(u32 rd, u32 ra, u32 rb, u32 oe, bool rc)
{
    const s64 gpra = thread.gpr[ra];
    const s64 gprb = thread.gpr[rb];
    if (gprb == 0 || ((u64)gpra == (1ULL << 63) && gprb == -1)) {
        if (oe) unknown("divdo");
        thread.gpr[rd] = 0;
    }
    else {
        thread.gpr[rd] = gpra / gprb;
    }
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[rd]);
}
void PPUInterpreter::divdu(u32 rd, u32 ra, u32 rb, u32 oe, bool rc)
{
    const u64 gpra = thread.gpr[ra];
    const s64 gprb = thread.gpr[rb];
    if (gprb == 0) {
        if (oe) unknown("divduo");
        thread.gpr[rd] = 0;
    }
    else {
        thread.gpr[rd] = gpra / gprb;
    }
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[rd]);
}
void PPUInterpreter::divw(u32 rd, u32 ra, u32 rb, u32 oe, bool rc)
{
    const s32 gpra = thread.gpr[ra];
    const s32 gprb = thread.gpr[rb];
    if (gprb == 0 || ((u32)gpra == (1 << 31) && gprb == -1)) {
        if (oe) unknown("divwo");
        thread.gpr[rd] = 0;
    }
    else {
        thread.gpr[rd] = (u32)(gpra / gprb);
    }
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[rd]);
}
void PPUInterpreter::divwu(u32 rd, u32 ra, u32 rb, u32 oe, bool rc)
{
    const u32 gpra = thread.gpr[ra];
    const u32 gprb = thread.gpr[rb];
    if (gprb == 0) {
        if (oe) unknown("divwuo");
        thread.gpr[rd] = 0;
    }
    else {
        thread.gpr[rd] = gpra / gprb;
    }
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[rd]);
}
void PPUInterpreter::dss(u32 strm, u32 a)
{
    _mm_mfence();
}
void PPUInterpreter::dst(u32 ra, u32 rb, u32 strm, u32 t)
{
    _mm_mfence();
}
void PPUInterpreter::dstst(u32 ra, u32 rb, u32 strm, u32 t)
{
    _mm_mfence();
}
void PPUInterpreter::eciwx(u32 rd, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    thread.gpr[rd] = nucleus.memory.read32(addr);
}
void PPUInterpreter::ecowx(u32 rs, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    nucleus.memory.write32(addr, thread.gpr[rs]);
}
void PPUInterpreter::eieio()
{
    _mm_mfence();
}
void PPUInterpreter::eqv(u32 ra, u32 rs, u32 rb, bool rc)
{
    thread.gpr[ra] = ~(thread.gpr[rs] ^ thread.gpr[rb]);
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::extsb(u32 ra, u32 rs, bool rc)
{
    thread.gpr[ra] = (s64)(s8)thread.gpr[rs];
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::extsh(u32 ra, u32 rs, bool rc)
{
    thread.gpr[ra] = (s64)(s16)thread.gpr[rs];
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::extsw(u32 ra, u32 rs, bool rc)
{
    thread.gpr[ra] = (s64)(s32)thread.gpr[rs];
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::icbi(u32 ra, u32 rs)
{
    // Nothing to do in the interpreter.
}
void PPUInterpreter::isync()
{
    _mm_mfence();
}
void PPUInterpreter::lbz(u32 rd, u32 ra, s32 d)
{
    thread.gpr[rd] = nucleus.memory.read8(ra ? thread.gpr[ra] + d : d);
}
void PPUInterpreter::lbzu(u32 rd, u32 ra, s32 d)
{
    const u32 addr = thread.gpr[ra] + d;
    thread.gpr[rd] = nucleus.memory.read8(addr);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::lbzux(u32 rd, u32 ra, u32 rb)
{
    const u32 addr = thread.gpr[ra] + thread.gpr[rb];
    thread.gpr[rd] = nucleus.memory.read8(addr);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::lbzx(u32 rd, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    thread.gpr[rd] = nucleus.memory.read8(addr);
}
void PPUInterpreter::ld(u32 rd, u32 ra, s32 ds)
{
    const u32 addr = ra ? thread.gpr[ra] + ds : ds;
    thread.gpr[rd] = nucleus.memory.read64(addr);
}
void PPUInterpreter::ldarx(u32 rd, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    thread.gpr[rd] = nucleus.memory.read64(addr);
    thread.reserve_addr = addr;
    thread.reserve_value = re64(thread.gpr[rd]);
    
}
void PPUInterpreter::ldbrx(u32 rd, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    thread.gpr[rd] = re64(nucleus.memory.read64(addr));
}
void PPUInterpreter::ldu(u32 rd, u32 ra, s32 ds)
{
    const u32 addr = thread.gpr[ra] + ds;
    thread.gpr[rd] = nucleus.memory.read64(addr);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::ldux(u32 rd, u32 ra, u32 rb)
{
    const u32 addr = thread.gpr[ra] + thread.gpr[rb];
    thread.gpr[rd] = nucleus.memory.read64(addr);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::ldx(u32 rd, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    thread.gpr[rd] = nucleus.memory.read64(addr);
}
void PPUInterpreter::lha(u32 rd, u32 ra, s32 d)
{
    const u32 addr = ra ? thread.gpr[ra] + d : d;
    thread.gpr[rd] = (s64)(s16)nucleus.memory.read16(addr);
}
void PPUInterpreter::lhau(u32 rd, u32 ra, s32 d)
{
    const u32 addr = thread.gpr[ra] + d;
    thread.gpr[rd] = (s64)(s16)nucleus.memory.read16(addr);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::lhaux(u32 rd, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    thread.gpr[rd] = (s64)(s16)nucleus.memory.read16(addr);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::lhax(u32 rd, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    thread.gpr[rd] = (s64)(s16)nucleus.memory.read16(addr);
}
void PPUInterpreter::lhbrx(u32 rd, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    thread.gpr[rd] = re16(nucleus.memory.read16(addr));
}
void PPUInterpreter::lhz(u32 rd, u32 ra, s32 d)
{
    const u32 addr = ra ? thread.gpr[ra] + d : d;
    thread.gpr[rd] = nucleus.memory.read16(addr);
}
void PPUInterpreter::lhzu(u32 rd, u32 ra, s32 d)
{
    const u32 addr = thread.gpr[ra] + d;
    thread.gpr[rd] = nucleus.memory.read16(addr);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::lhzux(u32 rd, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    thread.gpr[rd] = nucleus.memory.read16(addr);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::lhzx(u32 rd, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    thread.gpr[rd] = nucleus.memory.read16(addr);
}
void PPUInterpreter::lmw(u32 rd, u32 ra, s32 d)
{
    u32 addr = ra ? thread.gpr[ra] + d : d;
    for (u32 i = rd; i < 32; i++) {
        thread.gpr[i] = nucleus.memory.read32(addr);
        addr += 4;
    }
}
void PPUInterpreter::lswi(u32 rd, u32 ra, u32 nb)
{
    u64 ea = ra ? thread.gpr[ra] : 0;
    u64 n = nb ? nb : 32;
    u8 reg = thread.gpr[rd];
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
void PPUInterpreter::lswx(u32 rd, u32 ra, u32 rb)
{
    unknown("lswx");
}
void PPUInterpreter::lwa(u32 rd, u32 ra, s32 ds)
{
    thread.gpr[rd] = (s64)(s32)nucleus.memory.read32(ra ? thread.gpr[ra] + ds : ds);
}
void PPUInterpreter::lwarx(u32 rd, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    thread.gpr[rd] = nucleus.memory.read32(addr);
    thread.reserve_addr = addr;
    thread.reserve_value = re32(thread.gpr[rd]);
}
void PPUInterpreter::lwaux(u32 rd, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    thread.gpr[rd] = (s64)(s32)nucleus.memory.read32(addr);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::lwax(u32 rd, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    thread.gpr[rd] = (s64)(s32)nucleus.memory.read32(addr);
}
void PPUInterpreter::lwbrx(u32 rd, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    thread.gpr[rd] = re32(nucleus.memory.read32(addr));
}
void PPUInterpreter::lwz(u32 rd, u32 ra, s32 d)
{
    const u32 addr = ra ? thread.gpr[ra] + d : d;
    thread.gpr[rd] = nucleus.memory.read32(addr);
}
void PPUInterpreter::lwzu(u32 rd, u32 ra, s32 d)
{
    const u32 addr = thread.gpr[ra] + d;
    thread.gpr[rd] = nucleus.memory.read32(addr);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::lwzux(u32 rd, u32 ra, u32 rb)
{
    const u32 addr = thread.gpr[ra] + thread.gpr[rb];
    thread.gpr[rd] = nucleus.memory.read32(addr);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::lwzx(u32 rd, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    thread.gpr[rd] = nucleus.memory.read32(addr);
}
void PPUInterpreter::mcrf(u32 crfd, u32 crfs)
{
    thread.cr.setField(crfd, thread.cr.getField(crfs));
}
void PPUInterpreter::mcrfs(u32 crbd, u32 crbs)
{
    u64 mask = (1ULL << crbd);
    thread.cr.CR &= ~mask;
    thread.cr.CR |= thread.fpscr.FPSCR & mask;
}
void PPUInterpreter::mfocrf(u32 a, u32 rd, u32 crm)
{
    thread.gpr[rd] = thread.cr.CR;
}
void PPUInterpreter::mfspr(u32 rd, u32 spr)
{
    thread.gpr[rd] = GetRegBySPR(spr);
}
void PPUInterpreter::mftb(u32 rd, u32 spr)
{
    const u32 n = (spr >> 5) | ((spr & 0x1f) << 5);
    switch (n) {
    case 0x10C: thread.gpr[rd] = thread.tbl; break;
    case 0x10D: thread.gpr[rd] = thread.tbu; break;
    default: unknown("mftb"); break;
    }
}
void PPUInterpreter::mtfsb0(u32 crbd, bool rc)
{
    thread.fpscr.FPSCR &= ~(1ULL << crbd);
    if (rc) unknown("mtfsb0.");
}
void PPUInterpreter::mtfsb1(u32 crbd, bool rc)
{
    thread.fpscr.FPSCR |= (1ULL << crbd);
    if (rc) unknown("mtfsb1.");
}
void PPUInterpreter::mtfsfi(u32 crfd, u32 i, bool rc)
{
    const u64 mask = (0x1ULL << crfd);
    if (i) {
        thread.fpscr.FPSCR |= mask;
    }
    else {
        thread.fpscr.FPSCR &= ~mask;
    }
    if (rc) unknown("mtfsfi.");
}
void PPUInterpreter::mtocrf(u32 l, u32 crm, u32 rs)
{
    if (l) {
        u32 n = 0, count = 0;
        for (int i = 0; i < 8; i++) {
            if (crm & (1 << i)) {
                n = i;
                count++;
            }
        }
        if (count == 1) {
            thread.cr.setField(7 - n, (thread.gpr[rs] >> (4*n)) & 0xf);
        }
        else {
            thread.cr.CR = 0;
        }
    }
    else {
        for (u32 i=0; i<8; ++i) {
            if (crm & (1 << i)) {
                thread.cr.setField(7 - i, thread.gpr[rs] & (0xf << (i * 4)));
            }
        }
    }
}
void PPUInterpreter::mtspr(u32 spr, u32 rs)
{
    GetRegBySPR(spr) = thread.gpr[rs];
}
void PPUInterpreter::mulhd(u32 rd, u32 ra, u32 rb, bool rc)
{
    thread.gpr[rd] = __mulh(thread.gpr[ra], thread.gpr[rb]);
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[rd]);
}
void PPUInterpreter::mulhdu(u32 rd, u32 ra, u32 rb, bool rc)
{
    thread.gpr[rd] = __umulh(thread.gpr[ra], thread.gpr[rb]);
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[rd]);
}
void PPUInterpreter::mulhw(u32 rd, u32 ra, u32 rb, bool rc)
{
    s32 a = thread.gpr[ra];
    s32 b = thread.gpr[rb];
    thread.gpr[rd] = ((s64)a * (s64)b) >> 32;
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[rd]);
}
void PPUInterpreter::mulhwu(u32 rd, u32 ra, u32 rb, bool rc)
{
    u32 a = thread.gpr[ra];
    u32 b = thread.gpr[rb];
    thread.gpr[rd] = ((u64)a * (u64)b) >> 32;
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[rd]);
}
void PPUInterpreter::mulld(u32 rd, u32 ra, u32 rb, u32 oe, bool rc)
{
    thread.gpr[rd] = (s64)((s64)thread.gpr[ra] * (s64)thread.gpr[rb]);
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[rd]);
    if (oe) unknown("mulldo");
}
void PPUInterpreter::mulli(u32 rd, u32 ra, s32 simm16)
{
    thread.gpr[rd] = (s64)thread.gpr[ra] * simm16;
}
void PPUInterpreter::mullw(u32 rd, u32 ra, u32 rb, u32 oe, bool rc)
{
    thread.gpr[rd] = (s64)((s64)(s32)thread.gpr[ra] * (s64)(s32)thread.gpr[rb]);
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[rd]);
    if (oe) unknown("mullwo");
}
void PPUInterpreter::nand(u32 ra, u32 rs, u32 rb, bool rc)
{
    thread.gpr[ra] = ~(thread.gpr[rs] & thread.gpr[rb]);

    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::neg(u32 rd, u32 ra, u32 oe, bool rc)
{
    thread.gpr[rd] = 0 - thread.gpr[ra];
    if (oe) unknown("nego");
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[rd]);
}
void PPUInterpreter::nop()
{
}
void PPUInterpreter::nor(u32 ra, u32 rs, u32 rb, bool rc)
{
    thread.gpr[ra] = ~(thread.gpr[rs] | thread.gpr[rb]);
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::or(u32 ra, u32 rs, u32 rb, bool rc)
{
    thread.gpr[ra] = thread.gpr[rs] | thread.gpr[rb];
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::orc(u32 ra, u32 rs, u32 rb, bool rc)
{
    thread.gpr[ra] = thread.gpr[rs] | ~thread.gpr[rb];
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::ori(u32 ra, u32 rs, u32 uimm16)
{
    thread.gpr[ra] = thread.gpr[rs] | uimm16;
}
void PPUInterpreter::oris(u32 ra, u32 rs, u32 uimm16)
{
    thread.gpr[ra] = thread.gpr[rs] | (uimm16 << 16);
}
void PPUInterpreter::rldc_lr(u32 ra, u32 rs, u32 rb, u32 m_eb, bool is_r, bool rc)
{
    if (is_r) {
        rldicr(ra, rs, thread.gpr[rb], m_eb, rc);
    }
    else {
        rldicl(ra, rs, thread.gpr[rb], m_eb, rc);
    }
}
void PPUInterpreter::rldic(u32 ra, u32 rs, u32 sh, u32 mb, bool rc)
{
    thread.gpr[ra] = rotl64(thread.gpr[rs], sh) & rotateMask[mb][63-sh];
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::rldicl(u32 ra, u32 rs, u32 sh, u32 mb, bool rc)
{
    thread.gpr[ra] = rotl64(thread.gpr[rs], sh) & rotateMask[mb][63];
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::rldicr(u32 ra, u32 rs, u32 sh, u32 me, bool rc)
{
    thread.gpr[ra] = rotl64(thread.gpr[rs], sh) & rotateMask[0][me];
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::rldimi(u32 ra, u32 rs, u32 sh, u32 mb, bool rc)
{
    const u64 mask = rotateMask[mb][63-sh];
    thread.gpr[ra] = (thread.gpr[ra] & ~mask) | (rotl64(thread.gpr[rs], sh) & mask);
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::rlwimi(u32 ra, u32 rs, u32 sh, u32 mb, u32 me, bool rc)
{
    const u64 mask = rotateMask[32 + mb][32 + me];
    thread.gpr[ra] = (thread.gpr[ra] & ~mask) | (rotl32(thread.gpr[rs], sh) & mask);
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::rlwinm(u32 ra, u32 rs, u32 sh, u32 mb, u32 me, bool rc)
{
    thread.gpr[ra] = rotl32(thread.gpr[rs], sh) & rotateMask[32 + mb][32 + me];
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::rlwnm(u32 ra, u32 rs, u32 rb, u32 mb, u32 me, bool rc)
{
    thread.gpr[ra] = rotl32(thread.gpr[rs], thread.gpr[rb] & 0x1f) & rotateMask[32 + mb][32 + me];
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::sc(u32 sc_code)
{
    if (sc_code == 2) {
        unknown("sc");//TODO: SysCall();
    }
    else {
        unknown("sc");
    }
}
void PPUInterpreter::sld(u32 ra, u32 rs, u32 rb, bool rc)
{
    u32 n = thread.gpr[rb] & 0x3f;
    u64 r = rotl64(thread.gpr[rs], n);
    u64 m = (thread.gpr[rb] & 0x40) ? 0 : rotateMask[0][63 - n];
    thread.gpr[ra] = r & m;
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::slw(u32 ra, u32 rs, u32 rb, bool rc)
{
    u32 n = thread.gpr[rb] & 0x1f;
    u32 r = rotl32((u32)thread.gpr[rs], n);
    u32 m = (thread.gpr[rb] & 0x20) ? 0 : rotateMask[32][63 - n];
    thread.gpr[ra] = r & m;
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::srad(u32 ra, u32 rs, u32 rb, bool rc)
{
    s64 RS = thread.gpr[rs];
    u8 shift = thread.gpr[rb] & 127;
    if (shift > 63) {
        thread.gpr[ra] = 0 - (RS < 0);
        thread.xer.CA = (RS < 0);
    }
    else {
        thread.gpr[ra] = RS >> shift;
        thread.xer.CA = (RS < 0) & ((thread.gpr[ra] << shift) != RS);
    }
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::sradi1(u32 ra, u32 rs, u32 sh, bool rc)
{
    s64 RS = thread.gpr[rs];
    thread.gpr[ra] = RS >> sh;
    thread.xer.CA = (RS < 0) & ((thread.gpr[ra] << sh) != RS);
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::sradi2(u32 ra, u32 rs, u32 sh, bool rc)
{
    sradi1(ra, rs, sh, rc);
}
void PPUInterpreter::sraw(u32 ra, u32 rs, u32 rb, bool rc)
{
    s32 gprs = thread.gpr[rs];
    u8 shift = thread.gpr[rb] & 63;
    if (shift > 31) {
        thread.gpr[ra] = 0 - (gprs < 0);
        thread.xer.CA = (gprs < 0);
    }
    else {
        thread.gpr[ra] = gprs >> shift;
        thread.xer.CA = (gprs < 0) & ((thread.gpr[ra] << shift) != gprs);
    }
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::srawi(u32 ra, u32 rs, u32 sh, bool rc)
{
    s32 gprs = (u32)thread.gpr[rs];
    thread.gpr[ra] = gprs >> sh;
    thread.xer.CA = (gprs < 0) & ((u32)(thread.gpr[ra] << sh) != gprs);
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::srd(u32 ra, u32 rs, u32 rb, bool rc)
{
    u32 n = thread.gpr[rb] & 0x3f;
    u64 m = (thread.gpr[rb] & 0x40) ? 0 : rotateMask[n][63];
    u64 r = rotl64(thread.gpr[rs], 64 - n);
    thread.gpr[ra] = r & m;
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::srw(u32 ra, u32 rs, u32 rb, bool rc)
{
    u32 n = thread.gpr[rb] & 0x1f;
    u32 m = (thread.gpr[rb] & 0x20) ? 0 : rotateMask[32 + n][63];
    u32 r = rotl32((u32)thread.gpr[rs], 64 - n);
    thread.gpr[ra] = r & m;
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::stb(u32 rs, u32 ra, s32 d)
{
    const u32 addr = ra ? thread.gpr[ra] + d : d;
    nucleus.memory.write8(addr, thread.gpr[rs]);
}
void PPUInterpreter::stbu(u32 rs, u32 ra, s32 d)
{
    const u32 addr = thread.gpr[ra] + d;
    nucleus.memory.write8(addr, thread.gpr[rs]);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::stbux(u32 rs, u32 ra, u32 rb)
{
    const u32 addr = thread.gpr[ra] + thread.gpr[rb];
    nucleus.memory.write8(addr, thread.gpr[rs]);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::stbx(u32 rs, u32 ra, u32 rb)
{
    nucleus.memory.write8((ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb]), thread.gpr[rs]);
}
void PPUInterpreter::std(u32 rs, u32 ra, s32 d)
{
    nucleus.memory.write64(ra ? thread.gpr[ra] + d : d, thread.gpr[rs]);
}
void PPUInterpreter::stdcx_(u32 rs, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    if (thread.reserve_addr == addr) {
        bool value = false;// TODO: InterlockedCompareExchange((volatile u64*)(nucleus.memory + addr), re64(thread.gpr[rs]), thread.reserve_value) == thread.reserve_value;
        thread.cr.setBit(thread.cr.CR_EQ, value);
    }
    else {
        thread.cr.setBit(thread.cr.CR_EQ, false);
    }
}
void PPUInterpreter::stdu(u32 rs, u32 ra, s32 ds)
{
    const u32 addr = thread.gpr[ra] + ds;
    nucleus.memory.write64(addr, thread.gpr[rs]);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::stdux(u32 rs, u32 ra, u32 rb)
{
    const u32 addr = thread.gpr[ra] + thread.gpr[rb];
    nucleus.memory.write64(addr, thread.gpr[rs]);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::stdx(u32 rs, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    nucleus.memory.write64(ra, thread.gpr[rs]);
}
void PPUInterpreter::sth(u32 rs, u32 ra, s32 d)
{
    const u32 addr = ra ? thread.gpr[ra] + d : d;
    nucleus.memory.write16(addr, thread.gpr[rs]);
}
void PPUInterpreter::sthbrx(u32 rs, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    nucleus.memory.write16(addr, re16((u16)thread.gpr[rs]));
}
void PPUInterpreter::sthu(u32 rs, u32 ra, s32 d)
{
    const u32 addr = thread.gpr[ra] + d;
    nucleus.memory.write16(addr, thread.gpr[rs]);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::sthux(u32 rs, u32 ra, u32 rb)
{
    const u32 addr = thread.gpr[ra] + thread.gpr[rb];
    nucleus.memory.write16(addr, thread.gpr[rs]);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::sthx(u32 rs, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    nucleus.memory.write16(addr, thread.gpr[rs]);
}
void PPUInterpreter::stmw(u32 rs, u32 ra, s32 d)
{
    u32 addr = ra ? thread.gpr[ra] + d : d;
    for (int i = rs; i < 32; i++) {
        nucleus.memory.write32(addr, thread.gpr[i]);
        addr += 4;
    }
}
void PPUInterpreter::stswi(u32 rd, u32 ra, u32 nb)
{
    u64 ea = ra ? thread.gpr[ra] : 0;
    u64 n = nb ? nb : 32;
    u8 reg = thread.gpr[rd];

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
void PPUInterpreter::stswx(u32 rs, u32 ra, u32 rb)
{
    unknown("stwsx");
}
void PPUInterpreter::stw(u32 rs, u32 ra, s32 d)
{
    const u32 addr = ra ? thread.gpr[ra] + d : d;
    nucleus.memory.write32(addr, thread.gpr[rs]);
}
void PPUInterpreter::stwbrx(u32 rs, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    nucleus.memory.write32(addr, re32(thread.gpr[rs]));
}
void PPUInterpreter::stwcx_(u32 rs, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    if (thread.reserve_addr == addr) {
        bool value = false;// TODO: InterlockedCompareExchange((volatile u32*)(nucleus.memory + addr), re32((u32)thread.gpr[rs]), (u32)thread.reserve_value) == (u32)thread.reserve_value;
        thread.cr.setBit(thread.cr.CR_EQ, value);
    }
    else {
        thread.cr.setBit(thread.cr.CR_EQ, false);
    }
}
void PPUInterpreter::stwu(u32 rs, u32 ra, s32 d)
{
    const u32 addr = thread.gpr[ra] + d;
    nucleus.memory.write32(addr, thread.gpr[rs]);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::stwux(u32 rs, u32 ra, u32 rb)
{
    const u32 addr = thread.gpr[ra] + thread.gpr[rb];
    nucleus.memory.write32(addr, thread.gpr[rs]);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::stwx(u32 rs, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    nucleus.memory.write32(addr, thread.gpr[rs]);
}
void PPUInterpreter::subf(u32 rd, u32 ra, u32 rb, u32 oe, bool rc)
{
    thread.gpr[rd] = thread.gpr[rb] - thread.gpr[ra];
    if (oe) unknown("subfo");
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[rd]);
}
void PPUInterpreter::subfc(u32 rd, u32 ra, u32 rb, u32 oe, bool rc)
{
    const u64 gpra = thread.gpr[ra];
    const s64 gprb = thread.gpr[rb];
    thread.gpr[rd] = ~gpra + gprb + 1;
    thread.xer.CA = isCarry(~gpra, gprb, 1);
    if (oe) unknown("subfco");
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[rd]);
}
void PPUInterpreter::subfe(u32 rd, u32 ra, u32 rb, u32 oe, bool rc)
{
    const u64 gpra = thread.gpr[ra];
    const s64 gprb = thread.gpr[rb];
    thread.gpr[rd] = ~gpra + gprb + thread.xer.CA;
    thread.xer.CA = isCarry(~gpra, gprb, thread.xer.CA);
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[rd]);
    if (oe) unknown("subfeo");
}
void PPUInterpreter::subfic(u32 rd, u32 ra, s32 simm16)
{
    const u64 gpra = thread.gpr[ra];
    const u64 IMM = (s64)simm16;
    thread.gpr[rd] = ~gpra + IMM + 1;

    thread.xer.CA = isCarry(~gpra, IMM, 1);
}
void PPUInterpreter::subfme(u32 rd, u32 ra, u32 oe, bool rc)
{
    const u64 gpra = thread.gpr[ra];
    thread.gpr[rd] = ~gpra + thread.xer.CA + ~0ULL;
    thread.xer.CA = isCarry(~gpra, thread.xer.CA, ~0ULL);
    if (oe) unknown("subfmeo");
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[rd]);
}
void PPUInterpreter::subfze(u32 rd, u32 ra, u32 oe, bool rc)
{
    const u64 gpra = thread.gpr[ra];
    thread.gpr[rd] = ~gpra + thread.xer.CA;
    thread.xer.CA = isCarry(~gpra, thread.xer.CA);
    if (oe) unknown("subfzeo");
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[rd]);
}
void PPUInterpreter::sync(u32 l)
{
    _mm_mfence();
}
void PPUInterpreter::td(u32 to, u32 ra, u32 rb)
{
    unknown("td");
}
void PPUInterpreter::tdi(u32 to, u32 ra, s32 simm16)
{
    s64 a = thread.gpr[ra];

    if ((a < (s64)simm16  && (to & 0x10)) ||
        (a > (s64)simm16  && (to & 0x8))  ||
        (a == (s64)simm16 && (to & 0x4))  ||
        ((u64)a < (u64)simm16 && (to & 0x2)) ||
        ((u64)a > (u64)simm16 && (to & 0x1))) {
        unknown("tdi");
    }
}

void PPUInterpreter::tw(u32 to, u32 ra, u32 rb)
{
    s32 a = thread.gpr[ra];
    s32 b = thread.gpr[rb];

    if ((a < b  && (to & 0x10)) ||
        (a > b  && (to & 0x8))  ||
        (a == b && (to & 0x4))  ||
        ((u32)a < (u32)b && (to & 0x2)) ||
        ((u32)a > (u32)b && (to & 0x1))) {
        unknown("tw");
    }
}
void PPUInterpreter::twi(u32 to, u32 ra, s32 simm16)
{
    s32 a = thread.gpr[ra];

    if ((a < simm16  && (to & 0x10)) ||
        (a > simm16  && (to & 0x8))  ||
        (a == simm16 && (to & 0x4))  ||
        ((u32)a < (u32)simm16 && (to & 0x2)) ||
        ((u32)a > (u32)simm16 && (to & 0x1))) {
        unknown("twi");
    }
}
void PPUInterpreter::xor(u32 ra, u32 rs, u32 rb, bool rc)
{
    thread.gpr[ra] = thread.gpr[rs] ^ thread.gpr[rb];
    if (rc) {} // TODO: CPU.UpdateCR0<s64>(thread.gpr[ra]);
}
void PPUInterpreter::xori(u32 ra, u32 rs, u32 uimm16)
{
    thread.gpr[ra] = thread.gpr[rs] ^ uimm16;
}
void PPUInterpreter::xoris(u32 ra, u32 rs, u32 uimm16)
{
    thread.gpr[ra] = thread.gpr[rs] ^ (uimm16 << 16);
}

// Floating-Point Unit instructions
void PPUInterpreter::lfd(u32 frd, u32 ra, s32 d)
{
    const u32 addr = ra ? thread.gpr[ra] + d : d;
    (u64&)thread.fpr[frd] = nucleus.memory.read64(addr);
}
void PPUInterpreter::lfdu(u32 frd, u32 ra, s32 ds)
{
    const u32 addr = thread.gpr[ra] + ds;
    (u64&)thread.fpr[frd] = nucleus.memory.read64(addr);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::lfdux(u32 frd, u32 ra, u32 rb)
{
    const u32 addr = thread.gpr[ra] + thread.gpr[rb];
    (u64&)thread.fpr[frd] = nucleus.memory.read64(addr);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::lfdx(u32 frd, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    (u64&)thread.fpr[frd] = nucleus.memory.read64(addr);
}
void PPUInterpreter::lfs(u32 frd, u32 ra, s32 d)
{
    const u32 addr = ra ? thread.gpr[ra] + d : d;
    const u32 value = nucleus.memory.read32(addr);
    thread.fpr[frd] = (f32&)value;
}
void PPUInterpreter::lfsu(u32 frd, u32 ra, s32 ds)
{
    const u32 addr = thread.gpr[ra] + ds;
    const u32 v = nucleus.memory.read32(addr);
    thread.fpr[frd] = (f32&)v;
    thread.gpr[ra] = addr;
}
void PPUInterpreter::lfsux(u32 frd, u32 ra, u32 rb)
{
    const u32 addr = thread.gpr[ra] + thread.gpr[rb];
    (u64&)thread.fpr[frd] = nucleus.memory.read32(addr);
    thread.fpr[frd] = (f32&)thread.fpr[frd];
    thread.gpr[ra] = addr;
}
void PPUInterpreter::lfsx(u32 frd, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    (u32&)thread.fpr[frd] = nucleus.memory.read32(addr);
    thread.fpr[frd] = (f32&)thread.fpr[frd];
}
void PPUInterpreter::fabs(u32 frd, u32 frb, bool rc)
{
    const f32 value = thread.fpr[frb];
    thread.fpr[frd] = (value < 0) ? -value : value;
    if (rc) unknown("fabs.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
}
void PPUInterpreter::fadd(u32 frd, u32 fra, u32 frb, bool rc)
{
    thread.fpr[frd] = thread.fpr[fra] + thread.fpr[frb];
    if (rc) unknown("fadd.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
}
void PPUInterpreter::fadds(u32 frd, u32 fra, u32 frb, bool rc)
{
    thread.fpr[frd] = static_cast<f32>(thread.fpr[fra] + thread.fpr[frb]);
    if (rc) unknown("fadds.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
}
void PPUInterpreter::fcfid(u32 frd, u32 frb, bool rc)
{
    const s64 bi = (s64&)thread.fpr[frb];
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
    thread.fpr[frd] = bf;
    if (rc) unknown("fcfid.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
}
void PPUInterpreter::fcmpo(u32 crfd, u32 fra, u32 frb)
{/*
    int cmp_res = FPRdouble::Cmp(thread.fpr[fra], thread.fpr[frb]);

    if (cmp_res == CR_SO) {
        if (FPRdouble::IsSNaN(thread.fpr[fra]) || FPRdouble::IsSNaN(thread.fpr[frb])) {
            thread.SetFPSCRException(FPSCR_VXSNAN);
            if (!thread.fpscr.VE) thread.SetFPSCRException(FPSCR_VXVC);
        }
        else {
            thread.SetFPSCRException(FPSCR_VXVC);
        }

        thread.fpscr.FX = 1;
    }

    thread.fpscr.FPRF = cmp_res;
    thread.cr.setField(crfd, cmp_res);
*/}
void PPUInterpreter::fcmpu(u32 crfd, u32 fra, u32 frb)
{/*
    int cmp_res = FPRdouble::Cmp(thread.fpr[fra], thread.fpr[frb]);
    if (cmp_res == CR_SO) {
        if (FPRdouble::IsSNaN(thread.fpr[fra]) || FPRdouble::IsSNaN(thread.fpr[frb])) {
            thread.SetFPSCRException(FPSCR_VXSNAN);
        }
    }
    thread.fpscr.FPRF = cmp_res;
    thread.cr.setField(crfd, cmp_res);
*/}
void PPUInterpreter::fctid(u32 frd, u32 frb, bool rc)
{/*
    const f64 b = thread.fpr[frb];
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

    (u64&)thread.fpr[frd] = 0xfff8000000000000ull | r;
    if (r == 0 && ((u64&)b & DOUBLE_SIGN)) (u64&)thread.fpr[frd] |= 0x100000000ull;

    if (rc) unknown("fctid.");
*/}
void PPUInterpreter::fctidz(u32 frd, u32 frb, bool rc)
{/*
    const f64 b = thread.fpr[frb];
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

    (u64&)thread.fpr[frd] = 0xfff8000000000000ull | r;
    if (r == 0 && ((u64&)b & DOUBLE_SIGN)) (u64&)thread.fpr[frd] |= 0x100000000ull;

    if (rc) unknown("fctidz.");
*/}
void PPUInterpreter::fctiw(u32 frd, u32 frb, bool rc)
{/*
    const f64 b = thread.fpr[frb];
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

    (u64&)thread.fpr[frd] = 0xfff8000000000000ull | r;
    if (r == 0 && ((u64&)b & DOUBLE_SIGN)) (u64&)thread.fpr[frd] |= 0x100000000ull;
    if (rc) unknown("fctiw.");
*/}
void PPUInterpreter::fctiwz(u32 frd, u32 frb, bool rc)
{/*
    const f64 b = thread.fpr[frb];
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

    (u64&)thread.fpr[frd] = 0xfff8000000000000ull | value;
    if (value == 0 && ((u64&)b & DOUBLE_SIGN)) {
        (u64&)thread.fpr[frd] |= 0x100000000ull;
    }
    if (rc) unknown("fctiwz.");
*/}
void PPUInterpreter::fdiv(u32 frd, u32 fra, u32 frb, bool rc)
{/*
    f64 res;

    if (FPRdouble::IsNaN(thread.fpr[fra])) {
        res = thread.fpr[fra];
    }
    else if (FPRdouble::IsNaN(thread.fpr[frb])) {
        res = thread.fpr[frb];
    }
    else {
        if (thread.fpr[frb] == 0.0) {
            if (thread.fpr[fra] == 0.0) {
                thread.fpscr.VXZDZ = 1;
                res = FPR_NAN;
            }
            else {
                res = thread.fpr[fra] / thread.fpr[frb];
            }

            thread.SetFPSCRException(FPSCR_ZX);
        }
        else if (FPRdouble::IsINF(thread.fpr[fra]) && FPRdouble::IsINF(thread.fpr[frb])) {
            thread.fpscr.VXIDI = 1;
            res = FPR_NAN;
        }
        else {
            res = thread.fpr[fra] / thread.fpr[frb];
        }
    }

    thread.fpr[frd] = res;
    thread.fpscr.FPRF = thread.fpr[frd].GetType();
    if (rc) unknown("fdiv.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
*/}
void PPUInterpreter::fdivs(u32 frd, u32 fra, u32 frb, bool rc)
{/*
    if (FPRdouble::IsNaN(thread.fpr[fra])) {
        thread.fpr[frd] = thread.fpr[fra];
    }
    else if (FPRdouble::IsNaN(thread.fpr[frb])) {
        thread.fpr[frd] = thread.fpr[frb];
    }
    else {
        if (thread.fpr[frb] == 0.0) {
            if (thread.fpr[fra] == 0.0) {
                thread.fpscr.VXZDZ = true;
                thread.fpr[frd] = FPR_NAN;
            }
            else {
                thread.fpr[frd] = (f32)(thread.fpr[fra] / thread.fpr[frb]);
            }

            thread.fpscr.ZX = true;
        }
        else if (FPRdouble::IsINF(thread.fpr[fra]) && FPRdouble::IsINF(thread.fpr[frb])) {
            thread.fpscr.VXIDI = true;
            thread.fpr[frd] = FPR_NAN;
        }
        else {
            thread.fpr[frd] = (f32)(thread.fpr[fra] / thread.fpr[frb]);
        }
    }

    thread.fpscr.FPRF = thread.fpr[frd].GetType();
    if (rc) unknown("fdivs.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
*/}
void PPUInterpreter::fmadd(u32 frd, u32 fra, u32 frc, u32 frb, bool rc)
{/*
    thread.fpr[frd] = thread.fpr[fra] * thread.fpr[frc] + thread.fpr[frb];
    thread.fpscr.FPRF = thread.fpr[frd].GetType();
    if (rc) unknown("fmadd.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
*/}
void PPUInterpreter::fmadds(u32 frd, u32 fra, u32 frc, u32 frb, bool rc)
{/*
    thread.fpr[frd] = static_cast<f32>(thread.fpr[fra] * thread.fpr[frc] + thread.fpr[frb]);
    thread.fpscr.FPRF = thread.fpr[frd].GetType();
    if (rc) unknown("fmadds.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
*/}
void PPUInterpreter::fmr(u32 frd, u32 frb, bool rc)
{/*
    thread.fpr[frd] = thread.fpr[frb];
    if (rc) unknown("fmr.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
*/}
void PPUInterpreter::fmsub(u32 frd, u32 fra, u32 frc, u32 frb, bool rc)
{/*
    thread.fpr[frd] = thread.fpr[fra] * thread.fpr[frc] - thread.fpr[frb];
    thread.fpscr.FPRF = thread.fpr[frd].GetType();
    if (rc) unknown("fmsub.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
*/}
void PPUInterpreter::fmsubs(u32 frd, u32 fra, u32 frc, u32 frb, bool rc)
{/*
    thread.fpr[frd] = static_cast<f32>(thread.fpr[fra] * thread.fpr[frc] - thread.fpr[frb]);
    thread.fpscr.FPRF = thread.fpr[frd].GetType();
    if (rc) unknown("fmsubs.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
*/}
void PPUInterpreter::fmul(u32 frd, u32 fra, u32 frc, bool rc)
{/*
    if ((FPRdouble::IsINF(thread.fpr[fra]) && thread.fpr[frc] == 0.0) || (FPRdouble::IsINF(thread.fpr[frc]) && thread.fpr[fra] == 0.0)) {
        thread.SetFPSCRException(FPSCR_VXIMZ);
        thread.fpr[frd] = FPR_NAN;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
        thread.fpscr.FPRF = FPR_QNAN;
    }
    else {
        if (FPRdouble::IsSNaN(thread.fpr[fra]) || FPRdouble::IsSNaN(thread.fpr[frc])) {
            thread.SetFPSCRException(FPSCR_VXSNAN);
        }

        thread.fpr[frd] = thread.fpr[fra] * thread.fpr[frc];
        thread.fpscr.FPRF = thread.fpr[frd].GetType();
    }

    if (rc) unknown("fmul.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
*/}
void PPUInterpreter::fmuls(u32 frd, u32 fra, u32 frc, bool rc)
{/*
    thread.fpr[frd] = static_cast<f32>(thread.fpr[fra] * thread.fpr[frc]);
    thread.fpscr.FI = 0;
    thread.fpscr.FR = 0;
    thread.fpscr.FPRF = thread.fpr[frd].GetType();
    if (rc) unknown("fmuls.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
*/}
void PPUInterpreter::fnabs(u32 frd, u32 frb, bool rc)
{/*
    thread.fpr[frd] = -fabs(thread.fpr[frb]);
    if (rc) unknown("fnabs.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
*/}
void PPUInterpreter::fneg(u32 frd, u32 frb, bool rc)
{/*
    thread.fpr[frd] = -thread.fpr[frb];
    if (rc) unknown("fneg.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
*/}
void PPUInterpreter::fnmadd(u32 frd, u32 fra, u32 frc, u32 frb, bool rc)
{/*
    thread.fpr[frd] = -(thread.fpr[fra] * thread.fpr[frc] + thread.fpr[frb]);
    thread.fpscr.FPRF = thread.fpr[frd].GetType();
    if (rc) unknown("fnmadd.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
*/}
void PPUInterpreter::fnmadds(u32 frd, u32 fra, u32 frc, u32 frb, bool rc)
{/*
    thread.fpr[frd] = static_cast<f32>(-(thread.fpr[fra] * thread.fpr[frc] + thread.fpr[frb]));
    thread.fpscr.FPRF = thread.fpr[frd].GetType();
    if (rc) unknown("fnmadds.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
*/}
void PPUInterpreter::fnmsub(u32 frd, u32 fra, u32 frc, u32 frb, bool rc)
{/*
    thread.fpr[frd] = -(thread.fpr[fra] * thread.fpr[frc] - thread.fpr[frb]);
    thread.fpscr.FPRF = thread.fpr[frd].GetType();
    if (rc) unknown("fnmsub.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
*/}
void PPUInterpreter::fnmsubs(u32 frd, u32 fra, u32 frc, u32 frb, bool rc)
{/*
    thread.fpr[frd] = static_cast<f32>(-(thread.fpr[fra] * thread.fpr[frc] - thread.fpr[frb]));
    thread.fpscr.FPRF = thread.fpr[frd].GetType();
    if (rc) unknown("fnmsubs.");////{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
*/}
void PPUInterpreter::fres(u32 frd, u32 frb, bool rc)
{/*
    if (thread.fpr[frb] == 0.0) {
        thread.SetFPSCRException(FPSCR_ZX);
    }
    thread.fpr[frd] = static_cast<f32>(1.0 / thread.fpr[frb]);
    if (rc) unknown("fres.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
*/}
void PPUInterpreter::frsp(u32 frd, u32 frb, bool rc)
{/*
    const f64 b = thread.fpr[frb];
    f64 b0 = b;
    if (thread.fpscr.NI) {
        if (((u64&)b0 & DOUBLE_EXP) < 0x3800000000000000ULL) (u64&)b0 &= DOUBLE_SIGN;
    }
    const f64 r = static_cast<f32>(b0);
    thread.fpscr.FR = fabs(r) > fabs(b);
    thread.SetFPSCR_FI(b != r);
    thread.fpscr.FPRF = PPCdouble(r).GetType();
    thread.fpr[frd] = r;
*/}
void PPUInterpreter::frsqrte(u32 frd, u32 frb, bool rc)
{/*
    if (thread.fpr[frb] == 0.0) {
        thread.SetFPSCRException(FPSCR_ZX);
    }
    thread.fpr[frd] = static_cast<f32>(1.0 / sqrt(thread.fpr[frb]));
    if (rc) unknown("frsqrte.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
*/}
void PPUInterpreter::fsel(u32 frd, u32 fra, u32 frc, u32 frb, bool rc)
{/*
    thread.fpr[frd] = thread.fpr[fra] >= 0.0 ? thread.fpr[frc] : thread.fpr[frb];
    if (rc) unknown("fsel.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
*/}
void PPUInterpreter::fsqrt(u32 frd, u32 frb, bool rc)
{/*
    thread.fpr[frd] = sqrt(thread.fpr[frb]);
    thread.fpscr.FPRF = thread.fpr[frd].GetType();
    if (rc) unknown("fsqrt.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
*/}
void PPUInterpreter::fsqrts(u32 frd, u32 frb, bool rc)
{/*
    thread.fpr[frd] = static_cast<f32>(sqrt(thread.fpr[frb]));
    thread.fpscr.FPRF = thread.fpr[frd].GetType();
    if (rc) unknown("fsqrts.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
*/}
void PPUInterpreter::fsub(u32 frd, u32 fra, u32 frb, bool rc)
{/*
    thread.fpr[frd] = thread.fpr[fra] - thread.fpr[frb];
    thread.fpscr.FPRF = thread.fpr[frd].GetType();
    if (rc) unknown("fsub.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
*/}
void PPUInterpreter::fsubs(u32 frd, u32 fra, u32 frb, bool rc)
{/*
    thread.fpr[frd] = static_cast<f32>(thread.fpr[fra] - thread.fpr[frb]);
    thread.fpscr.FPRF = thread.fpr[frd].GetType();
    if (rc) unknown("fsubs.");//{} // TODO: CPU.UpdateCR1(thread.fpr[frd]);
*/}
void PPUInterpreter::mffs(u32 frd, bool rc)
{
    (u64&)thread.fpr[frd] = thread.fpscr.FPSCR;
    if (rc) unknown("mffs.");
}
void PPUInterpreter::mtfsf(u32 flm, u32 frb, bool rc)
{
    u32 mask = 0;
    for (int i = 0; i < 8; i++) {
        if (flm & (1 << i)) {
            mask |= 0xf << (i * 4);
        }
    }
    thread.fpscr.FPSCR = (thread.fpscr.FPSCR & ~mask) | ((u32&)thread.fpr[frb] & mask);
    if (rc) unknown("mtfsf.");
}
void PPUInterpreter::stfd(u32 frs, u32 ra, s32 d)
{
    const u32 addr = ra ? thread.gpr[ra] + d : d;
    nucleus.memory.write64(addr, (u64&)thread.fpr[frs]);
}
void PPUInterpreter::stfdu(u32 frs, u32 ra, s32 d)
{
    const u32 addr = thread.gpr[ra] + d;
    nucleus.memory.write64(addr, (u64&)thread.fpr[frs]);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::stfdux(u32 frs, u32 ra, u32 rb)
{
    const u32 addr = thread.gpr[ra] + thread.gpr[rb];
    nucleus.memory.write64(addr, (u64&)thread.fpr[frs]);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::stfdx(u32 frs, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    nucleus.memory.write64(addr, (u64&)thread.fpr[frs]);
}
void PPUInterpreter::stfiwx(u32 frs, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    nucleus.memory.write32(addr, (u32&)thread.fpr[frs]);
}
void PPUInterpreter::stfs(u32 frs, u32 ra, s32 d)
{
    const u32 addr = ra ? thread.gpr[ra] + d : d;
    const f32 value = thread.fpr[frs];
    nucleus.memory.write32(addr, (u32&)value);
}
void PPUInterpreter::stfsu(u32 frs, u32 ra, s32 d)
{
    const u32 addr = thread.gpr[ra] + d;
    const f32 value = thread.fpr[frs];
    nucleus.memory.write32(addr, (u32&)value);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::stfsux(u32 frs, u32 ra, u32 rb)
{
    const u32 addr = thread.gpr[ra] + thread.gpr[rb];
    const f32 value = thread.fpr[frs];
    nucleus.memory.write32(addr, (u32&)value);
    thread.gpr[ra] = addr;
}
void PPUInterpreter::stfsx(u32 frs, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    const f32 value = thread.fpr[frs];
    nucleus.memory.write32(addr, (u32&)value);
}

// Vector/SIMD instructions
void PPUInterpreter::lvebx(u32 vd, u32 ra, u32 rb)
{
    const u32 addr = (ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb]) & ~0xfULL;
    thread.vr[vd]._u128 = nucleus.memory.read128(addr);
}
void PPUInterpreter::lvehx(u32 vd, u32 ra, u32 rb)
{
    const u32 addr = (ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb]) & ~0xfULL;
    thread.vr[vd]._u128 = nucleus.memory.read128(addr);
}
void PPUInterpreter::lvewx(u32 vd, u32 ra, u32 rb)
{
    const u32 addr = (ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb]) & ~0xfULL;
    thread.vr[vd]._u128 = nucleus.memory.read128(addr);
}
void PPUInterpreter::lvlx(u32 vd, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.readLeft(thread.vr[vd]._u8 + eb, addr, 16 - eb);
}
void PPUInterpreter::lvlxl(u32 vd, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.readLeft(thread.vr[vd]._u8 + eb, addr, 16 - eb);
}
void PPUInterpreter::lvrx(u32 vd, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.readRight(thread.vr[vd]._u8, addr & ~0xf, eb);
}
void PPUInterpreter::lvrxl(u32 vd, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.readRight(thread.vr[vd]._u8, addr & ~0xf, eb);
}
void PPUInterpreter::lvsl(u32 vd, u32 ra, u32 rb)
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
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    thread.vr[vd]._u64[0] = lvsl_values[addr & 0xf][0];
    thread.vr[vd]._u64[1] = lvsl_values[addr & 0xf][1];
}
void PPUInterpreter::lvsr(u32 vd, u32 ra, u32 rb)
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
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    thread.vr[vd]._u64[0] = lvsr_values[addr & 0xf][0];
    thread.vr[vd]._u64[1] = lvsr_values[addr & 0xf][1];
}
void PPUInterpreter::lvx(u32 vd, u32 ra, u32 rb)
{
    const u32 addr = (ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb]) & ~0xfULL;
    thread.vr[vd]._u128 = nucleus.memory.read128(addr);
}
void PPUInterpreter::lvxl(u32 vd, u32 ra, u32 rb)
{
    const u32 addr = (ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb]) & ~0xfULL;
    thread.vr[vd]._u128 = nucleus.memory.read128(addr);
}
void PPUInterpreter::mfvscr(u32 vd)
{
    thread.vr[vd].clear();
    thread.vr[vd]._u32[0] = thread.vscr.VSCR;
}
void PPUInterpreter::mtvscr(u32 vb)
{
    thread.vscr.VSCR = thread.vr[vb]._u32[0];
}
void PPUInterpreter::stvebx(u32 vs, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.write8(addr, thread.vr[vs]._u8[15 - eb]);
}
void PPUInterpreter::stvehx(u32 vs, u32 ra, u32 rb)
{
    const u32 addr = (ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb]) & ~1ULL;
    const u8 eb = (addr & 0xf) >> 1;
    nucleus.memory.write16(addr, thread.vr[vs]._u16[7 - eb]);
}
void PPUInterpreter::stvewx(u32 vs, u32 ra, u32 rb)
{
    const u32 addr = (ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb]) & ~3ULL;
    const u8 eb = (addr & 0xf) >> 2;
    nucleus.memory.write32(addr, thread.vr[vs]._u32[3 - eb]);
}
void PPUInterpreter::stvlx(u32 vs, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.writeLeft(addr, thread.vr[vs]._u8 + eb, 16 - eb);
}
void PPUInterpreter::stvlxl(u32 vs, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.writeLeft(addr, thread.vr[vs]._u8 + eb, 16 - eb);
}
void PPUInterpreter::stvrx(u32 vs, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.writeRight(addr - eb, thread.vr[vs]._u8, eb);
}
void PPUInterpreter::stvrxl(u32 vs, u32 ra, u32 rb)
{
    const u32 addr = ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb];
    const u8 eb = addr & 0xf;
    nucleus.memory.writeRight(addr - eb, thread.vr[vs]._u8, eb);
}
void PPUInterpreter::stvx(u32 vs, u32 ra, u32 rb)
{
    const u32 addr = (ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb]) & ~0xfULL;
    nucleus.memory.write128(addr, thread.vr[vs]._u128);
}
void PPUInterpreter::stvxl(u32 vs, u32 ra, u32 rb)
{
    const u32 addr = (ra ? thread.gpr[ra] + thread.gpr[rb] : thread.gpr[rb]) & ~0xfULL;
    nucleus.memory.write128(addr, thread.vr[vs]._u128);
}
void PPUInterpreter::vaddcuw(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = ~thread.vr[va]._u32[w] < thread.vr[vb]._u32[w];
    }
}
void PPUInterpreter::vaddfp(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._f32[w] = thread.vr[va]._f32[w] + thread.vr[vb]._f32[w];
    }
}
void PPUInterpreter::vaddsbs(u32 vd, u32 va, u32 vb)
{
    for (u32 b=0; b<16; ++b) {
        const s16 result = (s16)thread.vr[va]._s8[b] + (s16)thread.vr[vb]._s8[b];
        if (result > 0x7f) {
            thread.vr[vd]._s8[b] = 0x7f;
            thread.vscr.SAT = 1;
        }
        else if (result < -0x80) {
            thread.vr[vd]._s8[b] = -0x80;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[vd]._s8[b] = result;
        }
    }
}
void PPUInterpreter::vaddshs(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        const s32 result = (s32)thread.vr[va]._s16[h] + (s32)thread.vr[vb]._s16[h];
        if (result > 0x7fff) {
            thread.vr[vd]._s16[h] = 0x7fff;
            thread.vscr.SAT = 1;
        }
        else if (result < -0x8000) {
            thread.vr[vd]._s16[h] = -0x8000;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[vd]._s16[h] = result;
        }
    }
}
void PPUInterpreter::vaddsws(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        const s64 result = (s64)thread.vr[va]._s32[w] + (s64)thread.vr[vb]._s32[w];
        if (result > 0x7fffffff) {
            thread.vr[vd]._s32[w] = 0x7fffffff;
            thread.vscr.SAT = 1;
        }
        else if (result < (s32)0x80000000) {
            thread.vr[vd]._s32[w] = 0x80000000;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[vd]._s32[w] = result;
        }
    }
}
void PPUInterpreter::vaddubm(u32 vd, u32 va, u32 vb)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[vd]._u8[b] = thread.vr[va]._u8[b] + thread.vr[vb]._u8[b];
    }
}
void PPUInterpreter::vaddubs(u32 vd, u32 va, u32 vb)
{
    for (int b = 0; b < 16; b++) {
        const u16 result = (u16)thread.vr[va]._u8[b] + (u16)thread.vr[vb]._u8[b];
        if (result > 0xff) {
            thread.vr[vd]._u8[b] = 0xff;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[vd]._u8[b] = result;
        }
    }
}
void PPUInterpreter::vadduhm(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._u16[h] = thread.vr[va]._u16[h] + thread.vr[vb]._u16[h];
    }
}
void PPUInterpreter::vadduhs(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        const u32 result = (u32)thread.vr[va]._u16[h] + (u32)thread.vr[vb]._u16[h];
        if (result > 0xffff) {
            thread.vr[vd]._u16[h] = 0xffff;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[vd]._u16[h] = result;
        }
    }
}
void PPUInterpreter::vadduwm(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = thread.vr[va]._u32[w] + thread.vr[vb]._u32[w];
    }
}
void PPUInterpreter::vadduws(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        const u64 result = (u64)thread.vr[va]._u32[w] + (u64)thread.vr[vb]._u32[w];
        if (result > 0xffffffff) {
            thread.vr[vd]._u32[w] = 0xffffffff;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[vd]._u32[w] = result;
        }
    }
}
void PPUInterpreter::vand(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = thread.vr[va]._u32[w] & thread.vr[vb]._u32[w];
    }
}
void PPUInterpreter::vandc(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = thread.vr[va]._u32[w] & (~thread.vr[vb]._u32[w]);
    }
}
void PPUInterpreter::vavgsb(u32 vd, u32 va, u32 vb)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[vd]._s8[b] = (thread.vr[va]._s8[b] + thread.vr[vb]._s8[b] + 1) >> 1;
    }
}
void PPUInterpreter::vavgsh(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._s16[h] = (thread.vr[va]._s16[h] + thread.vr[vb]._s16[h] + 1) >> 1;
    }
}
void PPUInterpreter::vavgsw(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._s32[w] = ((s64)thread.vr[va]._s32[w] + (s64)thread.vr[vb]._s32[w] + 1) >> 1;
    }
}
void PPUInterpreter::vavgub(u32 vd, u32 va, u32 vb)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[vd]._u8[b] = (thread.vr[va]._u8[b] + thread.vr[vb]._u8[b] + 1) >> 1;
    }
}
void PPUInterpreter::vavguh(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._u16[h] = (thread.vr[va]._u16[h] + thread.vr[vb]._u16[h] + 1) >> 1;
    }
}
void PPUInterpreter::vavguw(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = ((u64)thread.vr[va]._u32[w] + (u64)thread.vr[vb]._u32[w] + 1) >> 1;
    }
}
void PPUInterpreter::vcfsx(u32 vd, u32 uimm5, u32 vb)
{
    const u32 scale = 1 << uimm5;
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._f32[w] = ((f32)thread.vr[vb]._s32[w]) / scale;
    }
}
void PPUInterpreter::vcfux(u32 vd, u32 uimm5, u32 vb)
{
    const u32 scale = 1 << uimm5;
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._f32[w] = ((f32)thread.vr[vb]._u32[w]) / scale;
    }
}
void PPUInterpreter::vcmpbfp(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        u32 mask = 0;
        const f32 A = CheckVSCR_NJ(thread.vr[va]._f32[w]);
        const f32 B = CheckVSCR_NJ(thread.vr[vb]._f32[w]);

        if (A >  B) mask |= 1 << 31;
        if (A < -B) mask |= 1 << 30;

        thread.vr[vd]._u32[w] = mask;
    }
}
void PPUInterpreter::vcmpbfp_(u32 vd, u32 va, u32 vb)
{
    bool allInBounds = true;
    for (int w = 0; w < 4; w++) {
        u32 mask = 0;
        const f32 A = CheckVSCR_NJ(thread.vr[va]._f32[w]);
        const f32 B = CheckVSCR_NJ(thread.vr[vb]._f32[w]);

        if (A >  B) mask |= 1 << 31;
        if (A < -B) mask |= 1 << 30;

        thread.vr[vd]._u32[w] = mask;
        if (mask) {
            allInBounds = false;
        }
    }
    thread.cr.setBit(6*4 + 2, allInBounds);
}
void PPUInterpreter::vcmpeqfp(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = thread.vr[va]._f32[w] == thread.vr[vb]._f32[w] ? 0xffffffff : 0;
    }
}
void PPUInterpreter::vcmpeqfp_(u32 vd, u32 va, u32 vb)
{
    int all_equal = 0x8;
    int none_equal = 0x2;

    for (int w = 0; w < 4; w++) {
        if (thread.vr[va]._f32[w] == thread.vr[vb]._f32[w]) {
            thread.vr[vd]._u32[w] = 0xffffffff;
            none_equal = 0;
        }
        else {
            thread.vr[vd]._u32[w] = 0;
            all_equal = 0;
        }
    }

    thread.cr.setField(6, all_equal | none_equal);
}
void PPUInterpreter::vcmpequb(u32 vd, u32 va, u32 vb)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[vd]._u8[b] = thread.vr[va]._u8[b] == thread.vr[vb]._u8[b] ? 0xff : 0;
    }
}
void PPUInterpreter::vcmpequb_(u32 vd, u32 va, u32 vb)
{
    int all_equal = 0x8;
    int none_equal = 0x2;
        
    for (int b = 0; b < 16; b++) {
        if (thread.vr[va]._u8[b] == thread.vr[vb]._u8[b]) {
            thread.vr[vd]._u8[b] = 0xff;
            none_equal = 0;
        }
        else {
            thread.vr[vd]._u8[b] = 0;
            all_equal = 0;
        }
    }

    thread.cr.setField(6, all_equal | none_equal);
}
void PPUInterpreter::vcmpequh(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._u16[h] = thread.vr[va]._u16[h] == thread.vr[vb]._u16[h] ? 0xffff : 0;
    }
}
void PPUInterpreter::vcmpequh_(u32 vd, u32 va, u32 vb)
{
    int all_equal = 0x8;
    int none_equal = 0x2;
        
    for (int h = 0; h < 8; h++) {
        if (thread.vr[va]._u16[h] == thread.vr[vb]._u16[h]) {
            thread.vr[vd]._u16[h] = 0xffff;
            none_equal = 0;
        }
        else {
            thread.vr[vd]._u16[h] = 0;
            all_equal = 0;
        }
    }
        
    thread.cr.setField(6, all_equal | none_equal);
}
void PPUInterpreter::vcmpequw(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = thread.vr[va]._u32[w] == thread.vr[vb]._u32[w] ? 0xffffffff : 0;
    }
}
void PPUInterpreter::vcmpequw_(u32 vd, u32 va, u32 vb)
{
    int all_equal = 0x8;
    int none_equal = 0x2;
        
    for (int w = 0; w < 4; w++) {
        if (thread.vr[va]._u32[w] == thread.vr[vb]._u32[w]) {
            thread.vr[vd]._u32[w] = 0xffffffff;
            none_equal = 0;
        }
        else {
            thread.vr[vd]._u32[w] = 0;
            all_equal = 0;
        }
    }

    thread.cr.setField(6, all_equal | none_equal);
}
void PPUInterpreter::vcmpgefp(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = thread.vr[va]._f32[w] >= thread.vr[vb]._f32[w] ? 0xffffffff : 0;
    }
}
void PPUInterpreter::vcmpgefp_(u32 vd, u32 va, u32 vb)
{
    int all_ge = 0x8;
    int none_ge = 0x2;
        
    for (int w = 0; w < 4; w++) {
        if (thread.vr[va]._f32[w] >= thread.vr[vb]._f32[w]) {
            thread.vr[vd]._u32[w] = 0xffffffff;
            none_ge = 0;
        }
        else {
            thread.vr[vd]._u32[w] = 0;
            all_ge = 0;
        }
    }

    thread.cr.setField(6, all_ge | none_ge);
}
void PPUInterpreter::vcmpgtfp(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = thread.vr[va]._f32[w] > thread.vr[vb]._f32[w] ? 0xffffffff : 0;
    }
}
void PPUInterpreter::vcmpgtfp_(u32 vd, u32 va, u32 vb)
{
    int all_ge = 0x8;
    int none_ge = 0x2;
        
    for (int w = 0; w < 4; w++) {
        if (thread.vr[va]._f32[w] > thread.vr[vb]._f32[w]) {
            thread.vr[vd]._u32[w] = 0xffffffff;
            none_ge = 0;
        }
        else {
            thread.vr[vd]._u32[w] = 0;
            all_ge = 0;
        }
    }

    thread.cr.setField(6, all_ge | none_ge);
}
void PPUInterpreter::vcmpgtsb(u32 vd, u32 va, u32 vb)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[vd]._u8[b] = thread.vr[va]._s8[b] > thread.vr[vb]._s8[b] ? 0xff : 0;
    }
}
void PPUInterpreter::vcmpgtsb_(u32 vd, u32 va, u32 vb)
{
    int all_gt = 0x8;
    int none_gt = 0x2;
        
    for (int b = 0; b < 16; b++) {
        if (thread.vr[va]._s8[b] > thread.vr[vb]._s8[b]) {
            thread.vr[vd]._u8[b] = 0xff;
            none_gt = 0;
        }
        else {
            thread.vr[vd]._u8[b] = 0;
            all_gt = 0;
        }
    }

    thread.cr.setField(6, all_gt | none_gt);
}
void PPUInterpreter::vcmpgtsh(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._u16[h] = thread.vr[va]._s16[h] > thread.vr[vb]._s16[h] ? 0xffff : 0;
    }
}
void PPUInterpreter::vcmpgtsh_(u32 vd, u32 va, u32 vb)
{
    int all_gt = 0x8;
    int none_gt = 0x2;
        
    for (int h = 0; h < 8; h++) {
        if (thread.vr[va]._s16[h] > thread.vr[vb]._s16[h]) {
            thread.vr[vd]._u16[h] = 0xffff;
            none_gt = 0;
        }
        else {
            thread.vr[vd]._u16[h] = 0;
            all_gt = 0;
        }
    }

    thread.cr.setField(6, all_gt | none_gt);
}
void PPUInterpreter::vcmpgtsw(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = thread.vr[va]._s32[w] > thread.vr[vb]._s32[w] ? 0xffffffff : 0;
    }
}
void PPUInterpreter::vcmpgtsw_(u32 vd, u32 va, u32 vb)
{
    int all_gt = 0x8;
    int none_gt = 0x2;
        
    for (int w = 0; w < 4; w++) {
        if (thread.vr[va]._s32[w] > thread.vr[vb]._s32[w]) {
            thread.vr[vd]._u32[w] = 0xffffffff;
            none_gt = 0;
        }
        else {
            thread.vr[vd]._u32[w] = 0;
            all_gt = 0;
        }
    }

    thread.cr.setField(6, all_gt | none_gt);
}
void PPUInterpreter::vcmpgtub(u32 vd, u32 va, u32 vb)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[vd]._u8[b] = thread.vr[va]._u8[b] > thread.vr[vb]._u8[b] ? 0xff : 0;
    }
}
void PPUInterpreter::vcmpgtub_(u32 vd, u32 va, u32 vb)
{
    int all_gt = 0x8;
    int none_gt = 0x2;

    for (int b = 0; b < 16; b++) {
        if (thread.vr[va]._u8[b] > thread.vr[vb]._u8[b]) {
            thread.vr[vd]._u8[b] = 0xff;
            none_gt = 0;
        }
        else {
            thread.vr[vd]._u8[b] = 0;
            all_gt = 0;
        }
    }

    thread.cr.setField(6, all_gt | none_gt);
}
void PPUInterpreter::vcmpgtuh(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._u16[h] = thread.vr[va]._u16[h] > thread.vr[vb]._u16[h] ? 0xffff : 0;
    }
}
void PPUInterpreter::vcmpgtuh_(u32 vd, u32 va, u32 vb)
{
    int all_gt = 0x8;
    int none_gt = 0x2;
        
    for (int h = 0; h < 8; h++) {
        if (thread.vr[va]._u16[h] > thread.vr[vb]._u16[h]) {
            thread.vr[vd]._u16[h] = 0xffff;
            none_gt = 0;
        }
        else {
            thread.vr[vd]._u16[h] = 0;
            all_gt = 0;
        }
    }

    thread.cr.setField(6, all_gt | none_gt);
}
void PPUInterpreter::vcmpgtuw(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = thread.vr[va]._u32[w] > thread.vr[vb]._u32[w] ? 0xffffffff : 0;
    }
}
void PPUInterpreter::vcmpgtuw_(u32 vd, u32 va, u32 vb)
{
    int all_gt = 0x8;
    int none_gt = 0x2;
        
    for (int w = 0; w < 4; w++) {
        if (thread.vr[va]._u32[w] > thread.vr[vb]._u32[w]) {
            thread.vr[vd]._u32[w] = 0xffffffff;
            none_gt = 0;
        }
        else {
            thread.vr[vd]._u32[w] = 0;
            all_gt = 0;
        }
    }

    thread.cr.setField(6, all_gt | none_gt);
}
void PPUInterpreter::vctsxs(u32 vd, u32 uimm5, u32 vb)
{
    int nScale = 1 << uimm5;
        
    for (int w = 0; w < 4; w++) {        
        f32 result = thread.vr[vb]._f32[w] * nScale;

        if (result > 0x7fffffff)
            thread.vr[vd]._s32[w] = 0x7fffffff;
        else if (result < -0x80000000LL)
            thread.vr[vd]._s32[w] = -0x80000000LL;
        else {
            thread.vr[vd]._s32[w] = (int)result;
        }
    }
}
void PPUInterpreter::vctuxs(u32 vd, u32 uimm5, u32 vb)
{
    int nScale = 1 << uimm5;

    for (int w = 0; w < 4; w++) {
        s64 result = (s64)(thread.vr[vb]._f32[w] * nScale);

        if (result > 0xffffffff)
            thread.vr[vd]._u32[w] = 0xffffffff;
        else if (result < 0)
            thread.vr[vd]._u32[w] = 0;
        else
            thread.vr[vd]._u32[w] = (u32)result;
    }
}
void PPUInterpreter::vexptefp(u32 vd, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._f32[w] = exp(thread.vr[vb]._f32[w] * log(2.0f));
    }
}
void PPUInterpreter::vlogefp(u32 vd, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._f32[w] = log(thread.vr[vb]._f32[w]) / log(2.0f);
    }
}
void PPUInterpreter::vmaddfp(u32 vd, u32 va, u32 vc, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._f32[w] = thread.vr[va]._f32[w] * thread.vr[vc]._f32[w] + thread.vr[vb]._f32[w];
    }
}
void PPUInterpreter::vmaxfp(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._f32[w] = std::max(thread.vr[va]._f32[w], thread.vr[vb]._f32[w]);
    }
}
void PPUInterpreter::vmaxsb(u32 vd, u32 va, u32 vb)
{
    for (int b = 0; b < 16; b++)
        thread.vr[vd]._s8[b] = std::max(thread.vr[va]._s8[b], thread.vr[vb]._s8[b]);
}
void PPUInterpreter::vmaxsh(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._s16[h] = std::max(thread.vr[va]._s16[h], thread.vr[vb]._s16[h]);
    }
}
void PPUInterpreter::vmaxsw(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._s32[w] = std::max(thread.vr[va]._s32[w], thread.vr[vb]._s32[w]);
    }
}
void PPUInterpreter::vmaxub(u32 vd, u32 va, u32 vb)
{
    for (int b = 0; b < 16; b++)
        thread.vr[vd]._u8[b] = std::max(thread.vr[va]._u8[b], thread.vr[vb]._u8[b]);
}
void PPUInterpreter::vmaxuh(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._u16[h] = std::max(thread.vr[va]._u16[h], thread.vr[vb]._u16[h]);
    }
}
void PPUInterpreter::vmaxuw(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = std::max(thread.vr[va]._u32[w], thread.vr[vb]._u32[w]);
    }
}
void PPUInterpreter::vmhaddshs(u32 vd, u32 va, u32 vb, u32 vc)
{
    for (int h = 0; h < 8; h++) {
        s32 result = (s32)thread.vr[va]._s16[h] * (s32)thread.vr[vb]._s16[h] + (s32)thread.vr[vc]._s16[h];
        if (result > INT16_MAX) {
            thread.vr[vd]._s16[h] = (s16)INT16_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < INT16_MIN) {
            thread.vr[vd]._s16[h] = (s16)INT16_MIN;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[vd]._s16[h] = (s16)result;
        }
    }
}
void PPUInterpreter::vmhraddshs(u32 vd, u32 va, u32 vb, u32 vc)
{
    for (int h = 0; h < 8; h++) {
        s32 result = (s32)thread.vr[va]._s16[h] * (s32)thread.vr[vb]._s16[h] + (s32)thread.vr[vc]._s16[h] + 0x4000;

        if (result > INT16_MAX) {
            thread.vr[vd]._s16[h] = (s16)INT16_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < INT16_MIN) {
            thread.vr[vd]._s16[h] = (s16)INT16_MIN;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[vd]._s16[h] = (s16)result;
        }
    }
}
void PPUInterpreter::vminfp(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._f32[w] = std::min(thread.vr[va]._f32[w], thread.vr[vb]._f32[w]);
    }
}
void PPUInterpreter::vminsb(u32 vd, u32 va, u32 vb)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[vd]._s8[b] = std::min(thread.vr[va]._s8[b], thread.vr[vb]._s8[b]);
    }
}
void PPUInterpreter::vminsh(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._s16[h] = std::min(thread.vr[va]._s16[h], thread.vr[vb]._s16[h]);
    }
}
void PPUInterpreter::vminsw(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._s32[w] = std::min(thread.vr[va]._s32[w], thread.vr[vb]._s32[w]);
    }
}
void PPUInterpreter::vminub(u32 vd, u32 va, u32 vb)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[vd]._u8[b] = std::min(thread.vr[va]._u8[b], thread.vr[vb]._u8[b]);
    }
}
void PPUInterpreter::vminuh(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._u16[h] = std::min(thread.vr[va]._u16[h], thread.vr[vb]._u16[h]);
    }
}
void PPUInterpreter::vminuw(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = std::min(thread.vr[va]._u32[w], thread.vr[vb]._u32[w]);
    }
}
void PPUInterpreter::vmladduhm(u32 vd, u32 va, u32 vb, u32 vc)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._u16[h] = thread.vr[va]._u16[h] * thread.vr[vb]._u16[h] + thread.vr[vc]._u16[h];
    }
}
void PPUInterpreter::vmrghb(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._u8[15 - h*2 + 0] = thread.vr[va]._u8[15 - h];
        thread.vr[vd]._u8[15 - h*2 - 1] = thread.vr[vb]._u8[15 - h];
    }
}
void PPUInterpreter::vmrghh(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u16[7 - w*2 + 0] = thread.vr[va]._u16[7 - w];
        thread.vr[vd]._u16[7 - w*2 - 1] = thread.vr[vb]._u16[7 - w];
    }
}
void PPUInterpreter::vmrghw(u32 vd, u32 va, u32 vb)
{
    for (int d = 0; d < 2; d++) {
        thread.vr[vd]._u32[3 - d*2 + 0] = thread.vr[va]._u32[3 - d];
        thread.vr[vd]._u32[3 - d*2 - 1] = thread.vr[vb]._u32[3 - d];
    }
}
void PPUInterpreter::vmrglb(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._u8[15 - h*2 + 0] = thread.vr[va]._u8[7 - h];
        thread.vr[vd]._u8[15 - h*2 - 1] = thread.vr[vb]._u8[7 - h];
    }
}
void PPUInterpreter::vmrglh(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u16[7 - w*2] = thread.vr[va]._u16[3 - w];
        thread.vr[vd]._u16[7 - w*2 - 1] = thread.vr[vb]._u16[3 - w];
    }
}
void PPUInterpreter::vmrglw(u32 vd, u32 va, u32 vb)
{
    for (int d = 0; d < 2; d++) {
        thread.vr[vd]._u32[3 - d*2 + 0] = thread.vr[va]._u32[1 - d];
        thread.vr[vd]._u32[3 - d*2 - 1] = thread.vr[vb]._u32[1 - d];
    }
}
void PPUInterpreter::vmsummbm(u32 vd, u32 va, u32 vb, u32 vc)
{
    for (int w = 0; w < 4; w++) {
        s32 result = 0;
        for (int b = 0; b < 4; b++) {
            result += thread.vr[va]._s8[w*4 + b] * thread.vr[vb]._u8[w*4 + b];
        }
        result += thread.vr[vc]._s32[w];
        thread.vr[vd]._s32[w] = result;
    }
}
void PPUInterpreter::vmsumshm(u32 vd, u32 va, u32 vb, u32 vc)
{
    for (int w = 0; w < 4; w++) {
        s32 result = 0;
        for (int h = 0; h < 2; h++) {
            result += thread.vr[va]._s16[w*2 + h] * thread.vr[vb]._s16[w*2 + h];
        }
        result += thread.vr[vc]._s32[w];
        thread.vr[vd]._s32[w] = result;
    }
}
void PPUInterpreter::vmsumshs(u32 vd, u32 va, u32 vb, u32 vc)
{
    for (int w = 0; w < 4; w++) {
        s64 result = 0;
        for (int h = 0; h < 2; h++) {
            result += thread.vr[va]._s16[w*2 + h] * thread.vr[vb]._s16[w*2 + h];
        }
        result += thread.vr[vc]._s32[w];

        if (result > 0x7fffffff) {
            thread.vr[vd]._s32[w] = 0x7fffffff;
            thread.vscr.SAT = 1;
        }
        else if (result < -0x80000000LL) {
            thread.vr[vd]._s32[w] = -0x80000000LL;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[vd]._s32[w] = (s32)result;
        }
    }
}
void PPUInterpreter::vmsumubm(u32 vd, u32 va, u32 vb, u32 vc)
{
    for (int w = 0; w < 4; w++) {
        u32 result = 0;
        for (int b = 0; b < 4; b++) {
            result += thread.vr[va]._u8[w*4 + b] * thread.vr[vb]._u8[w*4 + b];
        }
        result += thread.vr[vc]._u32[w];
        thread.vr[vd]._u32[w] = result;
    }
}
void PPUInterpreter::vmsumuhm(u32 vd, u32 va, u32 vb, u32 vc)
{
    for (int w = 0; w < 4; w++) {
        u32 result = 0;

        for (int h = 0; h < 2; h++) {
            result += thread.vr[va]._u16[w*2 + h] * thread.vr[vb]._u16[w*2 + h];
        }

        result += thread.vr[vc]._u32[w];
        thread.vr[vd]._u32[w] = result;
    }
}
void PPUInterpreter::vmsumuhs(u32 vd, u32 va, u32 vb, u32 vc)
{
    for (int w = 0; w < 4; w++) {
        u64 result = 0;
        for (int h = 0; h < 2; h++) {
            result += thread.vr[va]._u16[w*2 + h] * thread.vr[vb]._u16[w*2 + h];
        }
        result += thread.vr[vc]._u32[w];

        if (result > 0xffffffff) {
            thread.vr[vd]._u32[w] = 0xffffffff;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[vd]._u32[w] = (u32)result;
        }
    }
}
void PPUInterpreter::vmulesb(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._s16[h] = (s16)thread.vr[va]._s8[h*2+1] * (s16)thread.vr[vb]._s8[h*2+1];
    }
}
void PPUInterpreter::vmulesh(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._s32[w] = (s32)thread.vr[va]._s16[w*2+1] * (s32)thread.vr[vb]._s16[w*2+1];
    }
}
void PPUInterpreter::vmuleub(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._u16[h] = (u16)thread.vr[va]._u8[h*2+1] * (u16)thread.vr[vb]._u8[h*2+1];
    }
}
void PPUInterpreter::vmuleuh(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = (u32)thread.vr[va]._u16[w*2+1] * (u32)thread.vr[vb]._u16[w*2+1];
    }
}
void PPUInterpreter::vmulosb(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._s16[h] = (s16)thread.vr[va]._s8[h*2] * (s16)thread.vr[vb]._s8[h*2];
    }
}
void PPUInterpreter::vmulosh(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._s32[w] = (s32)thread.vr[va]._s16[w*2] * (s32)thread.vr[vb]._s16[w*2];
    }
}
void PPUInterpreter::vmuloub(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._u16[h] = (u16)thread.vr[va]._u8[h*2] * (u16)thread.vr[vb]._u8[h*2];
    }
}
void PPUInterpreter::vmulouh(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = (u32)thread.vr[va]._u16[w*2] * (u32)thread.vr[vb]._u16[w*2];
    }
}
void PPUInterpreter::vnmsubfp(u32 vd, u32 va, u32 vc, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._f32[w] = -(thread.vr[va]._f32[w] * thread.vr[vc]._f32[w] - thread.vr[vb]._f32[w]);
    }
}
void PPUInterpreter::vnor(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = ~(thread.vr[va]._u32[w] | thread.vr[vb]._u32[w]);
    }
}
void PPUInterpreter::vor(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = thread.vr[va]._u32[w] | thread.vr[vb]._u32[w];
    }
}
void PPUInterpreter::vperm(u32 vd, u32 va, u32 vb, u32 vc)
{
    u8 tmpSRC[32];
    memcpy(tmpSRC, thread.vr[vb]._u8, 16);
    memcpy(tmpSRC + 16, thread.vr[va]._u8, 16);

    for (int b = 0; b < 16; b++) {
        u8 index = thread.vr[vc]._u8[b] & 0x1f;
            
        thread.vr[vd]._u8[b] = tmpSRC[0x1f - index];
    }
}
void PPUInterpreter::vpkpx(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 4; h++) {
        const u16 bb7  = thread.vr[vb]._u8[15 - (h*4 + 0)] & 0x1;
        const u16 bb8  = thread.vr[vb]._u8[15 - (h*4 + 1)] >> 3;
        const u16 bb16 = thread.vr[vb]._u8[15 - (h*4 + 2)] >> 3;
        const u16 bb24 = thread.vr[vb]._u8[15 - (h*4 + 3)] >> 3;
        const u16 ab7  = thread.vr[va]._u8[15 - (h*4 + 0)] & 0x1;
        const u16 ab8  = thread.vr[va]._u8[15 - (h*4 + 1)] >> 3;
        const u16 ab16 = thread.vr[va]._u8[15 - (h*4 + 2)] >> 3;
        const u16 ab24 = thread.vr[va]._u8[15 - (h*4 + 3)] >> 3;

        thread.vr[vd]._u16[3 - h] = (bb7 << 15) | (bb8 << 10) | (bb16 << 5) | bb24;
        thread.vr[vd]._u16[7 - h] = (ab7 << 15) | (ab8 << 10) | (ab16 << 5) | ab24;
    }
}
void PPUInterpreter::vpkshss(u32 vd, u32 va, u32 vb)
{
    for (int b = 0; b < 8; b++) {
        s16 result = thread.vr[va]._s16[b];

        if (result > INT8_MAX) {
            result = INT8_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < INT8_MIN) {
            result = INT8_MIN;
            thread.vscr.SAT = 1;
        }
            
        thread.vr[vd]._s8[b+8] = result;

        result = thread.vr[vb]._s16[b];

        if (result > INT8_MAX) {
            result = INT8_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < INT8_MIN) {
            result = INT8_MIN;
            thread.vscr.SAT = 1;
        }

        thread.vr[vd]._s8[b] = result;
    }
}
void PPUInterpreter::vpkshus(u32 vd, u32 va, u32 vb)
{
    for (int b = 0; b < 8; b++) {
        s16 result = thread.vr[va]._s16[b];

        if (result > UINT8_MAX) {
            result = UINT8_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < 0) {
            result = 0;
            thread.vscr.SAT = 1;
        }
            
        thread.vr[vd]._u8[b+8] = result;

        result = thread.vr[vb]._s16[b];

        if (result > UINT8_MAX) {
            result = UINT8_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < 0) {
            result = 0;
            thread.vscr.SAT = 1;
        }
            
        thread.vr[vd]._u8[b] = result;
    }
}
void PPUInterpreter::vpkswss(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 4; h++) {
        s32 result = thread.vr[va]._s32[h];

        if (result > INT16_MAX) {
            result = INT16_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < INT16_MIN) {
            result = INT16_MIN;
            thread.vscr.SAT = 1;
        }
            
        thread.vr[vd]._s16[h+4] = result;

        result = thread.vr[vb]._s32[h];

        if (result > INT16_MAX) {
            result = INT16_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < INT16_MIN) {
            result = INT16_MIN;
            thread.vscr.SAT = 1;
        }
            
        thread.vr[vd]._s16[h] = result;
    }
}
void PPUInterpreter::vpkswus(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 4; h++) {
        s32 result = thread.vr[va]._s32[h];

        if (result > UINT16_MAX) {
            result = UINT16_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < 0) {
            result = 0;
            thread.vscr.SAT = 1;
        }
            
        thread.vr[vd]._u16[h+4] = result;

        result = thread.vr[vb]._s32[h];

        if (result > UINT16_MAX) {
            result = UINT16_MAX;
            thread.vscr.SAT = 1;
        }
        else if (result < 0) {
            result = 0;
            thread.vscr.SAT = 1;
        }
            
        thread.vr[vd]._u16[h] = result;
    }
}
void PPUInterpreter::vpkuhum(u32 vd, u32 va, u32 vb)
{
    for (int b = 0; b < 8; b++) {
        thread.vr[vd]._u8[b+8] = thread.vr[va]._u8[b*2];
        thread.vr[vd]._u8[b  ] = thread.vr[vb]._u8[b*2];
    }
}
void PPUInterpreter::vpkuhus(u32 vd, u32 va, u32 vb)
{
    for (int b = 0; b < 8; b++) {
        u16 result = thread.vr[va]._u16[b];

        if (result > UINT8_MAX) {
            result = UINT8_MAX;
            thread.vscr.SAT = 1;
        }
            
        thread.vr[vd]._u8[b+8] = result;

        result = thread.vr[vb]._u16[b];

        if (result > UINT8_MAX) {
            result = UINT8_MAX;
            thread.vscr.SAT = 1;
        }
            
        thread.vr[vd]._u8[b] = result;
    }
}
void PPUInterpreter::vpkuwum(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 4; h++) {
        thread.vr[vd]._u16[h+4] = thread.vr[va]._u16[h*2];
        thread.vr[vd]._u16[h  ] = thread.vr[vb]._u16[h*2];
    }
}
void PPUInterpreter::vpkuwus(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 4; h++) {
        u32 result = thread.vr[va]._u32[h];

        if (result > UINT16_MAX) {
            result = UINT16_MAX;
            thread.vscr.SAT = 1;
        }
            
        thread.vr[vd]._u16[h+4] = result;

        result = thread.vr[vb]._u32[h];

        if (result > UINT16_MAX) {
            result = UINT16_MAX;
            thread.vscr.SAT = 1;
        }
            
        thread.vr[vd]._u16[h] = result;
    }
}
void PPUInterpreter::vrefp(u32 vd, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._f32[w] = 1.0f / thread.vr[vb]._f32[w];
    }
}
void PPUInterpreter::vrfim(u32 vd, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._f32[w] = floor(thread.vr[vb]._f32[w]);
    }
}
void PPUInterpreter::vrfin(u32 vd, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._f32[w] = floor(thread.vr[vb]._f32[w] + 0.5f);
    }
}
void PPUInterpreter::vrfip(u32 vd, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._f32[w] = ceil(thread.vr[vb]._f32[w]);
    }
}
void PPUInterpreter::vrfiz(u32 vd, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        f32 f;
        modff(thread.vr[vb]._f32[w], &f);
        thread.vr[vd]._f32[w] = f;
    }
}
void PPUInterpreter::vrlb(u32 vd, u32 va, u32 vb)
{
    for (int b = 0; b < 16; b++) {
        int nRot = thread.vr[vb]._u8[b] & 0x7;

        thread.vr[vd]._u8[b] = (thread.vr[va]._u8[b] << nRot) | (thread.vr[va]._u8[b] >> (8 - nRot));
    }
}
void PPUInterpreter::vrlh(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._u16[h] = rotl16(thread.vr[va]._u16[h], thread.vr[vb]._u8[h*2] & 0xf);
    }
}
void PPUInterpreter::vrlw(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = rotl32(thread.vr[va]._u32[w], thread.vr[vb]._u8[w*4] & 0x1f);
    }
}
void PPUInterpreter::vrsqrtefp(u32 vd, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._f32[w] = 1.0f / sqrtf(thread.vr[vb]._f32[w]);
    }
}
void PPUInterpreter::vsel(u32 vd, u32 va, u32 vb, u32 vc)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[vd]._u8[b] = (thread.vr[vb]._u8[b] & thread.vr[vc]._u8[b]) | (thread.vr[va]._u8[b] & (~thread.vr[vc]._u8[b]));
    }
}
void PPUInterpreter::vsl(u32 vd, u32 va, u32 vb)
{
    u8 sh = thread.vr[vb]._u8[0] & 0x7;

    u32 t = 1;

    for (int b = 0; b < 16; b++) {
        t &= (thread.vr[vb]._u8[b] & 0x7) == sh;
    }

    if (t) {
        thread.vr[vd]._u8[0] = thread.vr[va]._u8[0] << sh;

        for (int b = 1; b < 16; b++) {
            thread.vr[vd]._u8[b] = (thread.vr[va]._u8[b] << sh) | (thread.vr[va]._u8[b-1] >> (8 - sh));
        }
    }
}
void PPUInterpreter::vslb(u32 vd, u32 va, u32 vb)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[vd]._u8[b] = thread.vr[va]._u8[b] << (thread.vr[vb]._u8[b] & 0x7);
    }
}
void PPUInterpreter::vsldoi(u32 vd, u32 va, u32 vb, u32 sh)
{
    u8 tmpSRC[32];
    memcpy(tmpSRC, thread.vr[vb]._u8, 16);
    memcpy(tmpSRC + 16, thread.vr[va]._u8, 16);
    for (int b = 0; b < 16; b++) {
        thread.vr[vd]._u8[15 - b] = tmpSRC[31 - (b + sh)];
    }
}
void PPUInterpreter::vslh(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._u16[h] = thread.vr[va]._u16[h] << (thread.vr[vb]._u8[h*2] & 0xf);
    }
}
void PPUInterpreter::vslo(u32 vd, u32 va, u32 vb)
{
    u8 nShift = (thread.vr[vb]._u8[0] >> 3) & 0xf;

    thread.vr[vd].clear();

    for (u8 b = 0; b < 16 - nShift; b++) {
        thread.vr[vd]._u8[15 - b] = thread.vr[va]._u8[15 - (b + nShift)];
    }
}
void PPUInterpreter::vslw(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = thread.vr[va]._u32[w] << (thread.vr[vb]._u32[w] & 0x1f);
    }
}
void PPUInterpreter::vspltb(u32 vd, u32 uimm5, u32 vb)
{
    u8 byte = thread.vr[vb]._u8[15 - uimm5];
        
    for (int b = 0; b < 16; b++) {
        thread.vr[vd]._u8[b] = byte;
    }
}
void PPUInterpreter::vsplth(u32 vd, u32 uimm5, u32 vb)
{
    const u16 hword = thread.vr[vb]._u16[7 - uimm5];
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._u16[h] = hword;
    }
}
void PPUInterpreter::vspltisb(u32 vd, s32 simm5)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[vd]._u8[b] = simm5;
    }
}
void PPUInterpreter::vspltish(u32 vd, s32 simm5)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._u16[h] = (s16)simm5;
    }
}
void PPUInterpreter::vspltisw(u32 vd, s32 simm5)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = (s32)simm5;
    }
}
void PPUInterpreter::vspltw(u32 vd, u32 uimm5, u32 vb)
{
    const u32 word = thread.vr[vb]._u32[3 - uimm5];       
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = word;
    }
}
void PPUInterpreter::vsr(u32 vd, u32 va, u32 vb)
{
    u8 sh = thread.vr[vb]._u8[0] & 0x7;
    u32 t = 1;

    for (int b = 0; b < 16; b++) {
        t &= (thread.vr[vb]._u8[b] & 0x7) == sh;
    }
    if (t) {
        thread.vr[vd]._u8[15] = thread.vr[va]._u8[15] >> sh;

        for (int b = 0; b < 15; b++) {
            thread.vr[vd]._u8[14-b] = (thread.vr[va]._u8[14-b] >> sh) | (thread.vr[va]._u8[14-b+1] << (8 - sh));
        }
    }
}
void PPUInterpreter::vsrab(u32 vd, u32 va, u32 vb)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[vd]._s8[b] = thread.vr[va]._s8[b] >> (thread.vr[vb]._u8[b] & 0x7);
    }
}
void PPUInterpreter::vsrah(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._s16[h] = thread.vr[va]._s16[h] >> (thread.vr[vb]._u8[h*2] & 0xf);
    }
}
void PPUInterpreter::vsraw(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._s32[w] = thread.vr[va]._s32[w] >> (thread.vr[vb]._u8[w*4] & 0x1f);
    }
}
void PPUInterpreter::vsrb(u32 vd, u32 va, u32 vb)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[vd]._u8[b] = thread.vr[va]._u8[b] >> (thread.vr[vb]._u8[b] & 0x7);
    }
}
void PPUInterpreter::vsrh(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._u16[h] = thread.vr[va]._u16[h] >> (thread.vr[vb]._u8[h*2] & 0xf);
    }
}
void PPUInterpreter::vsro(u32 vd, u32 va, u32 vb)
{
    const u8 nShift = (thread.vr[vb]._u8[0] >> 3) & 0xf;
    thread.vr[vd].clear();

    for (u8 b = 0; b < 16 - nShift; b++) {
        thread.vr[vd]._u8[b] = thread.vr[va]._u8[b + nShift];
    }
}
void PPUInterpreter::vsrw(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = thread.vr[va]._u32[w] >> (thread.vr[vb]._u8[w*4] & 0x1f);
    }
}
void PPUInterpreter::vsubcuw(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = thread.vr[va]._u32[w] < thread.vr[vb]._u32[w] ? 0 : 1;
    }
}
void PPUInterpreter::vsubfp(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._f32[w] = thread.vr[va]._f32[w] - thread.vr[vb]._f32[w];
    }
}
void PPUInterpreter::vsubsbs(u32 vd, u32 va, u32 vb)
{
    for (int b = 0; b < 16; b++) {
        s16 result = (s16)thread.vr[va]._s8[b] - (s16)thread.vr[vb]._s8[b];

        if (result < INT8_MIN) {
            thread.vr[vd]._s8[b] = INT8_MIN;
            thread.vscr.SAT = 1;
        }
        else if (result > INT8_MAX) {
            thread.vr[vd]._s8[b] = INT8_MAX;
            thread.vscr.SAT = 1;
        }
        else
            thread.vr[vd]._s8[b] = (s8)result;
    }
}
void PPUInterpreter::vsubshs(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        s32 result = (s32)thread.vr[va]._s16[h] - (s32)thread.vr[vb]._s16[h];

        if (result < INT16_MIN) {
            thread.vr[vd]._s16[h] = (s16)INT16_MIN;
            thread.vscr.SAT = 1;
        }
        else if (result > INT16_MAX) {
            thread.vr[vd]._s16[h] = (s16)INT16_MAX;
            thread.vscr.SAT = 1;
        }
        else
            thread.vr[vd]._s16[h] = (s16)result;
    }
}
void PPUInterpreter::vsubsws(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        s64 result = (s64)thread.vr[va]._s32[w] - (s64)thread.vr[vb]._s32[w];

        if (result < INT32_MIN) {
            thread.vr[vd]._s32[w] = (s32)INT32_MIN;
            thread.vscr.SAT = 1;
        }
        else if (result > INT32_MAX) {
            thread.vr[vd]._s32[w] = (s32)INT32_MAX;
            thread.vscr.SAT = 1;
        }
        else
            thread.vr[vd]._s32[w] = (s32)result;
    }
}
void PPUInterpreter::vsububm(u32 vd, u32 va, u32 vb)
{
    for (int b = 0; b < 16; b++) {
        thread.vr[vd]._u8[b] = (u8)((thread.vr[va]._u8[b] - thread.vr[vb]._u8[b]) & 0xff);
    }
}
void PPUInterpreter::vsububs(u32 vd, u32 va, u32 vb)
{
    for (int b = 0; b < 16; b++) {
        s16 result = (s16)thread.vr[va]._u8[b] - (s16)thread.vr[vb]._u8[b];

        if (result < 0) {
            thread.vr[vd]._u8[b] = 0;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[vd]._u8[b] = (u8)result;
        }
    }
}
void PPUInterpreter::vsubuhm(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._u16[h] = thread.vr[va]._u16[h] - thread.vr[vb]._u16[h];
    }
}
void PPUInterpreter::vsubuhs(u32 vd, u32 va, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        s32 result = (s32)thread.vr[va]._u16[h] - (s32)thread.vr[vb]._u16[h];

        if (result < 0) {
            thread.vr[vd]._u16[h] = 0;
            thread.vscr.SAT = 1;
        }
        else
            thread.vr[vd]._u16[h] = (u16)result;
    }
}
void PPUInterpreter::vsubuwm(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._u32[w] = thread.vr[va]._u32[w] - thread.vr[vb]._u32[w];
    }
}
void PPUInterpreter::vsubuws(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        s64 result = (s64)thread.vr[va]._u32[w] - (s64)thread.vr[vb]._u32[w];

        if (result < 0) {
            thread.vr[vd]._u32[w] = 0;
            thread.vscr.SAT = 1;
        }
        else
            thread.vr[vd]._u32[w] = (u32)result;
    }
}
void PPUInterpreter::vsum2sws(u32 vd, u32 va, u32 vb)
{
    for (int n = 0; n < 2; n++) {
        s64 sum = (s64)thread.vr[va]._s32[n*2] + thread.vr[va]._s32[n*2 + 1] + thread.vr[vb]._s32[n*2];

        if (sum > INT32_MAX) {
            thread.vr[vd]._s32[n*2] = (s32)INT32_MAX;
            thread.vscr.SAT = 1;
        }
        else if (sum < INT32_MIN) {
            thread.vr[vd]._s32[n*2] = (s32)INT32_MIN;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[vd]._s32[n*2] = (s32)sum;
        }
    }
    thread.vr[vd]._s32[1] = 0;
    thread.vr[vd]._s32[3] = 0;
}
void PPUInterpreter::vsum4sbs(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        s64 sum = thread.vr[vb]._s32[w];

        for (int b = 0; b < 4; b++) {
            sum += thread.vr[va]._s8[w*4 + b];
        }

        if (sum > INT32_MAX) {
            thread.vr[vd]._s32[w] = (s32)INT32_MAX;
            thread.vscr.SAT = 1;
        }
        else if (sum < INT32_MIN) {
            thread.vr[vd]._s32[w] = (s32)INT32_MIN;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[vd]._s32[w] = (s32)sum;
        }
    }
}
void PPUInterpreter::vsum4shs(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        s64 sum = thread.vr[vb]._s32[w];

        for (int h = 0; h < 2; h++) {
            sum += thread.vr[va]._s16[w*2 + h];
        }

        if (sum > INT32_MAX) {
            thread.vr[vd]._s32[w] = (s32)INT32_MAX;
            thread.vscr.SAT = 1;
        }
        else if (sum < INT32_MIN) {
            thread.vr[vd]._s32[w] = (s32)INT32_MIN;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[vd]._s32[w] = (s32)sum;
        }
    }
}
void PPUInterpreter::vsum4ubs(u32 vd, u32 va, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        u64 sum = thread.vr[vb]._u32[w];

        for (int b = 0; b < 4; b++) {
            sum += thread.vr[va]._u8[w*4 + b];
        }

        if (sum > UINT32_MAX) {
            thread.vr[vd]._u32[w] = (u32)UINT32_MAX;
            thread.vscr.SAT = 1;
        }
        else {
            thread.vr[vd]._u32[w] = (u32)sum;
        }
    }
}
void PPUInterpreter::vsumsws(u32 vd, u32 va, u32 vb)
{
    thread.vr[vd].clear();
        
    s64 sum = thread.vr[vb]._s32[3];

    for (int w = 0; w < 4; w++) {
        sum += thread.vr[va]._s32[w];
    }

    if (sum > INT32_MAX) {
        thread.vr[vd]._s32[0] = (s32)INT32_MAX;
        thread.vscr.SAT = 1;
    }
    else if (sum < INT32_MIN) {
        thread.vr[vd]._s32[0] = (s32)INT32_MIN;
        thread.vscr.SAT = 1;
    }
    else
        thread.vr[vd]._s32[0] = (s32)sum;
}
void PPUInterpreter::vupkhpx(u32 vd, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._s8[(3 - w)*4 + 3] = thread.vr[vb]._s8[w*2 + 0] >> 7;
        thread.vr[vd]._u8[(3 - w)*4 + 2] = (thread.vr[vb]._u8[w*2 + 0] >> 2) & 0x1f;
        thread.vr[vd]._u8[(3 - w)*4 + 1] = ((thread.vr[vb]._u8[w*2 + 0] & 0x3) << 3) | ((thread.vr[vb]._u8[w*2 + 1] >> 5) & 0x7);
        thread.vr[vd]._u8[(3 - w)*4 + 0] = thread.vr[vb]._u8[w*2 + 1] & 0x1f;
    }
}
void PPUInterpreter::vupkhsb(u32 vd, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._s16[h] = thread.vr[vb]._s8[h];
    }
}
void PPUInterpreter::vupkhsh(u32 vd, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._s32[w] = thread.vr[vb]._s16[w];
    }
}
void PPUInterpreter::vupklpx(u32 vd, u32 vb)
{
    for (int w = 0; w < 4; w++) {
        thread.vr[vd]._s8[(3 - w)*4 + 3] = thread.vr[vb]._s8[8 + w*2 + 0] >> 7;
        thread.vr[vd]._u8[(3 - w)*4 + 2] = (thread.vr[vb]._u8[8 + w*2 + 0] >> 2) & 0x1f;
        thread.vr[vd]._u8[(3 - w)*4 + 1] = ((thread.vr[vb]._u8[8 + w*2 + 0] & 0x3) << 3) | ((thread.vr[vb]._u8[8 + w*2 + 1] >> 5) & 0x7);
        thread.vr[vd]._u8[(3 - w)*4 + 0] = thread.vr[vb]._u8[8 + w*2 + 1] & 0x1f;
    }
}
void PPUInterpreter::vupklsb(u32 vd, u32 vb)
{
    for (int h = 0; h < 8; h++) {
        thread.vr[vd]._s16[h] = thread.vr[vb]._s8[8 + h];
    }
}
void PPUInterpreter::vupklsh(u32 vd, u32 vb)
{
    thread.vr[vd]._s32[0] = thread.vr[vb]._s16[4 + 0];
    thread.vr[vd]._s32[1] = thread.vr[vb]._s16[4 + 1];
    thread.vr[vd]._s32[2] = thread.vr[vb]._s16[4 + 2];
    thread.vr[vd]._s32[3] = thread.vr[vb]._s16[4 + 3];
}
void PPUInterpreter::vxor(u32 vd, u32 va, u32 vb)
{
    thread.vr[vd]._u64[0] = thread.vr[va]._u64[0] ^ thread.vr[vb]._u64[0];
    thread.vr[vd]._u32[1] = thread.vr[va]._u64[1] ^ thread.vr[vb]._u64[1];
}

// Unknown instruction
void PPUInterpreter::unknown(u32 instruction)
{
}
void PPUInterpreter::unknown(const std::string& instruction)
{
}
