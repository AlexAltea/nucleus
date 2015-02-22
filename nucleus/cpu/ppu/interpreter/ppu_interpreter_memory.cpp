/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_interpreter.h"
#include "nucleus/emulator.h"

#if defined(NUCLEUS_PLATFORM_WINDOWS)
#include <Windows.h>
#elif defined(NUCLEUS_PLATFORM_LINUX)
#define InterlockedCompareExchange(ptr,new_val,old_val)  __sync_val_compare_and_swap(ptr,old_val,new_val)
#define InterlockedCompareExchange64(ptr,new_val,old_val)  __sync_val_compare_and_swap(ptr,old_val,new_val)
#endif

namespace cpu {
namespace ppu {

/**
 * PPC64 Instructions:
 *  - UISA: Load and Store Instructions (Section: 4.2.3)
 *  - UISA: Memory Synchronization Instructions (Section: 4.2.6)
 *  - VEA: Memory Synchronization Instructions (Section: 4.3.2)
 */

void Interpreter::lbz(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = nucleus.memory.read8(code.ra ? thread.gpr[code.ra] + code.d : code.d);
}

void Interpreter::lbzu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    thread.gpr[code.rd] = nucleus.memory.read8(addr);
    thread.gpr[code.ra] = addr;
}

void Interpreter::lbzux(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read8(addr);
    thread.gpr[code.ra] = addr;
}

void Interpreter::lbzx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read8(addr);
}

void Interpreter::ld(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + (code.ds << 2) : (code.ds << 2);
    thread.gpr[code.rd] = nucleus.memory.read64(addr);
}

void Interpreter::ldarx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read64(addr);
    thread.reserve_addr = addr;
    thread.reserve_value = re64(thread.gpr[code.rd]);
}

void Interpreter::ldbrx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = re64(nucleus.memory.read64(addr));
}

void Interpreter::ldu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + (code.ds << 2);
    thread.gpr[code.rd] = nucleus.memory.read64(addr);
    thread.gpr[code.ra] = addr;
}

void Interpreter::ldux(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read64(addr);
    thread.gpr[code.ra] = addr;
}

void Interpreter::ldx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read64(addr);
}

void Interpreter::lfd(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    thread.fpr[code.frd]._u64 = nucleus.memory.read64(addr);
}

void Interpreter::lfdu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    thread.fpr[code.frd]._u64 = nucleus.memory.read64(addr);
    thread.gpr[code.ra] = addr;
}

void Interpreter::lfdux(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    thread.fpr[code.frd]._u64 = nucleus.memory.read64(addr);
    thread.gpr[code.ra] = addr;
}

void Interpreter::lfdx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.fpr[code.frd]._u64 = nucleus.memory.read64(addr);
}

void Interpreter::lfs(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    const u32 value = nucleus.memory.read32(addr);
    thread.fpr[code.frd]._f64 = (f32&)value;
}

void Interpreter::lfsu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    const u32 value = nucleus.memory.read32(addr);
    thread.fpr[code.frd]._f64 = (f32&)value;
    thread.gpr[code.ra] = addr;
}

void Interpreter::lfsux(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    const u32 value = nucleus.memory.read32(addr);
    thread.fpr[code.frd]._f64 = (f32&)value;
    thread.gpr[code.ra] = addr;
}

void Interpreter::lfsx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const u32 value = nucleus.memory.read32(addr);
    thread.fpr[code.frd]._f64 = (f32&)value;
}

void Interpreter::lha(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    thread.gpr[code.rd] = (s64)(s16)nucleus.memory.read16(addr);
}

void Interpreter::lhau(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    thread.gpr[code.rd] = (s64)(s16)nucleus.memory.read16(addr);
    thread.gpr[code.ra] = addr;
}

void Interpreter::lhaux(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = (s64)(s16)nucleus.memory.read16(addr);
    thread.gpr[code.ra] = addr;
}

void Interpreter::lhax(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = (s64)(s16)nucleus.memory.read16(addr);
}

void Interpreter::lhbrx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = re16(nucleus.memory.read16(addr));
}

void Interpreter::lhz(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    thread.gpr[code.rd] = nucleus.memory.read16(addr);
}

void Interpreter::lhzu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    thread.gpr[code.rd] = nucleus.memory.read16(addr);
    thread.gpr[code.ra] = addr;
}

void Interpreter::lhzux(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read16(addr);
    thread.gpr[code.ra] = addr;
}

void Interpreter::lhzx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read16(addr);
}

void Interpreter::lmw(Instruction code, PPUThread& thread)
{
    u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    for (u32 i = code.rd; i < 32; i++) {
        thread.gpr[i] = nucleus.memory.read32(addr);
        addr += 4;
    }
}

void Interpreter::lswi(Instruction code, PPUThread& thread)
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

void Interpreter::lswx(Instruction code, PPUThread& thread)
{
    unknown("lswx");
}

void Interpreter::lwa(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = (s64)(s32)nucleus.memory.read32(code.ra ? thread.gpr[code.ra] + (code.ds << 2) : (code.ds << 2));
}

void Interpreter::lwarx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read32(addr);
    thread.reserve_addr = addr;
    thread.reserve_value = re32(thread.gpr[code.rd]);
}

void Interpreter::lwaux(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = (s64)(s32)nucleus.memory.read32(addr);
    thread.gpr[code.ra] = addr;
}

void Interpreter::lwax(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = (s64)(s32)nucleus.memory.read32(addr);
}

void Interpreter::lwbrx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = re32(nucleus.memory.read32(addr));
}

void Interpreter::lwz(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    thread.gpr[code.rd] = nucleus.memory.read32(addr);
}

void Interpreter::lwzu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    thread.gpr[code.rd] = nucleus.memory.read32(addr);
    thread.gpr[code.ra] = addr;
}

void Interpreter::lwzux(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read32(addr);
    thread.gpr[code.ra] = addr;
}

void Interpreter::lwzx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read32(addr);
}

void Interpreter::stb(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    nucleus.memory.write8(addr, thread.gpr[code.rs]);
}

void Interpreter::stbu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    nucleus.memory.write8(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}

void Interpreter::stbux(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    nucleus.memory.write8(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}

void Interpreter::stbx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write8(addr, thread.gpr[code.rs]);
}

void Interpreter::std(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    nucleus.memory.write64(addr, thread.gpr[code.rs]);
}

void Interpreter::stdcx_(Instruction code, PPUThread& thread)
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

void Interpreter::stdu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + (code.ds << 2);
    nucleus.memory.write64(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}

void Interpreter::stdux(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    nucleus.memory.write64(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}

void Interpreter::stdx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write64(addr, thread.gpr[code.rs]);
}

void Interpreter::stfd(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    nucleus.memory.write64(addr, thread.fpr[code.frs]._u64);
}

void Interpreter::stfdu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    nucleus.memory.write64(addr, thread.fpr[code.frs]._u64);
    thread.gpr[code.ra] = addr;
}

void Interpreter::stfdux(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    nucleus.memory.write64(addr, thread.fpr[code.frs]._u64);
    thread.gpr[code.ra] = addr;
}

void Interpreter::stfdx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write64(addr, thread.fpr[code.frs]._u64);
}

void Interpreter::stfiwx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write32(addr, (u32&)thread.fpr[code.frs]);
}

void Interpreter::stfs(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    const f32 value = thread.fpr[code.frs]._f64;
    nucleus.memory.write32(addr, (u32&)value);
}

void Interpreter::stfsu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    const f32 value = thread.fpr[code.frs]._f64;
    nucleus.memory.write32(addr, (u32&)value);
    thread.gpr[code.ra] = addr;
}

void Interpreter::stfsux(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    const f32 value = thread.fpr[code.frs]._f64;
    nucleus.memory.write32(addr, (u32&)value);
    thread.gpr[code.ra] = addr;
}

void Interpreter::stfsx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    const f32 value = thread.fpr[code.frs]._f64;
    nucleus.memory.write32(addr, (u32&)value);
}

void Interpreter::sth(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    nucleus.memory.write16(addr, thread.gpr[code.rs]);
}

void Interpreter::sthbrx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write16(addr, re16((u16)thread.gpr[code.rs]));
}

void Interpreter::sthu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    nucleus.memory.write16(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}

void Interpreter::sthux(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    nucleus.memory.write16(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}

void Interpreter::sthx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write16(addr, thread.gpr[code.rs]);
}

void Interpreter::stmw(Instruction code, PPUThread& thread)
{
    u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    for (int i = code.rs; i < 32; i++) {
        nucleus.memory.write32(addr, thread.gpr[i]);
        addr += 4;
    }
}

void Interpreter::stswi(Instruction code, PPUThread& thread)
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

void Interpreter::stswx(Instruction code, PPUThread& thread)
{
    unknown("stwsx");
}

void Interpreter::stw(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + code.d : code.d;
    nucleus.memory.write32(addr, thread.gpr[code.rs]);
}

void Interpreter::stwbrx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write32(addr, re32(thread.gpr[code.rs]));
}

void Interpreter::stwcx_(Instruction code, PPUThread& thread)
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

void Interpreter::stwu(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + code.d;
    nucleus.memory.write32(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}

void Interpreter::stwux(Instruction code, PPUThread& thread)
{
    const u32 addr = thread.gpr[code.ra] + thread.gpr[code.rb];
    nucleus.memory.write32(addr, thread.gpr[code.rs]);
    thread.gpr[code.ra] = addr;
}

void Interpreter::stwx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write32(addr, thread.gpr[code.rs]);
}

void Interpreter::eieio(Instruction code, PPUThread& thread)
{
    // TODO: _mm_fence();
}

void Interpreter::isync(Instruction code, PPUThread& thread)
{
    // TODO: _mm_fence();
}

void Interpreter::sync(Instruction code, PPUThread& thread)
{
    // TODO: _mm_fence();
}

}  // namespace ppu
}  // namespace cpu
