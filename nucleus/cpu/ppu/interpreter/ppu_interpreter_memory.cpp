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

void Interpreter::lbz(Instruction code)
{
    state.gpr[code.rd] = nucleus.memory.read8(code.ra ? state.gpr[code.ra] + code.d : code.d);
}

void Interpreter::lbzu(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + code.d;
    state.gpr[code.rd] = nucleus.memory.read8(addr);
    state.gpr[code.ra] = addr;
}

void Interpreter::lbzux(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + state.gpr[code.rb];
    state.gpr[code.rd] = nucleus.memory.read8(addr);
    state.gpr[code.ra] = addr;
}

void Interpreter::lbzx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    state.gpr[code.rd] = nucleus.memory.read8(addr);
}

void Interpreter::ld(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + (code.ds << 2) : (code.ds << 2);
    state.gpr[code.rd] = nucleus.memory.read64(addr);
}

void Interpreter::ldarx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    state.gpr[code.rd] = nucleus.memory.read64(addr);
    state.reserve_addr = addr;
    state.reserve_value = re64(state.gpr[code.rd]);
}

void Interpreter::ldbrx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    state.gpr[code.rd] = re64(nucleus.memory.read64(addr));
}

void Interpreter::ldu(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + (code.ds << 2);
    state.gpr[code.rd] = nucleus.memory.read64(addr);
    state.gpr[code.ra] = addr;
}

void Interpreter::ldux(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + state.gpr[code.rb];
    state.gpr[code.rd] = nucleus.memory.read64(addr);
    state.gpr[code.ra] = addr;
}

void Interpreter::ldx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    state.gpr[code.rd] = nucleus.memory.read64(addr);
}

void Interpreter::lfd(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + code.d : code.d;
    state.fpr[code.frd]._u64 = nucleus.memory.read64(addr);
}

void Interpreter::lfdu(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + code.d;
    state.fpr[code.frd]._u64 = nucleus.memory.read64(addr);
    state.gpr[code.ra] = addr;
}

void Interpreter::lfdux(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + state.gpr[code.rb];
    state.fpr[code.frd]._u64 = nucleus.memory.read64(addr);
    state.gpr[code.ra] = addr;
}

void Interpreter::lfdx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    state.fpr[code.frd]._u64 = nucleus.memory.read64(addr);
}

void Interpreter::lfs(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + code.d : code.d;
    const u32 value = nucleus.memory.read32(addr);
    state.fpr[code.frd]._f64 = (f32&)value;
}

void Interpreter::lfsu(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + code.d;
    const u32 value = nucleus.memory.read32(addr);
    state.fpr[code.frd]._f64 = (f32&)value;
    state.gpr[code.ra] = addr;
}

void Interpreter::lfsux(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + state.gpr[code.rb];
    const u32 value = nucleus.memory.read32(addr);
    state.fpr[code.frd]._f64 = (f32&)value;
    state.gpr[code.ra] = addr;
}

void Interpreter::lfsx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    const u32 value = nucleus.memory.read32(addr);
    state.fpr[code.frd]._f64 = (f32&)value;
}

void Interpreter::lha(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + code.d : code.d;
    state.gpr[code.rd] = (s64)(s16)nucleus.memory.read16(addr);
}

void Interpreter::lhau(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + code.d;
    state.gpr[code.rd] = (s64)(s16)nucleus.memory.read16(addr);
    state.gpr[code.ra] = addr;
}

void Interpreter::lhaux(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    state.gpr[code.rd] = (s64)(s16)nucleus.memory.read16(addr);
    state.gpr[code.ra] = addr;
}

void Interpreter::lhax(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    state.gpr[code.rd] = (s64)(s16)nucleus.memory.read16(addr);
}

void Interpreter::lhbrx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    state.gpr[code.rd] = re16(nucleus.memory.read16(addr));
}

void Interpreter::lhz(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + code.d : code.d;
    state.gpr[code.rd] = nucleus.memory.read16(addr);
}

void Interpreter::lhzu(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + code.d;
    state.gpr[code.rd] = nucleus.memory.read16(addr);
    state.gpr[code.ra] = addr;
}

void Interpreter::lhzux(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    state.gpr[code.rd] = nucleus.memory.read16(addr);
    state.gpr[code.ra] = addr;
}

void Interpreter::lhzx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    state.gpr[code.rd] = nucleus.memory.read16(addr);
}

void Interpreter::lmw(Instruction code)
{
    u32 addr = code.ra ? state.gpr[code.ra] + code.d : code.d;
    for (u32 i = code.rd; i < 32; i++) {
        state.gpr[i] = nucleus.memory.read32(addr);
        addr += 4;
    }
}

void Interpreter::lswi(Instruction code)
{
    u64 ea = code.ra ? state.gpr[code.ra] : 0;
    u64 n = code.nb ? code.nb : 32;
    u8 reg = code.rd;
    while (n > 0) {
        if (n > 3) {
            state.gpr[reg] = nucleus.memory.read32(ea);
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
            state.gpr[reg] = buf;
        }
        reg = (reg + 1) % 32;
    }
}

void Interpreter::lswx(Instruction code)
{
    unknown("lswx");
}

void Interpreter::lwa(Instruction code)
{
    state.gpr[code.rd] = (s64)(s32)nucleus.memory.read32(code.ra ? state.gpr[code.ra] + (code.ds << 2) : (code.ds << 2));
}

void Interpreter::lwarx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    state.gpr[code.rd] = nucleus.memory.read32(addr);
    state.reserve_addr = addr;
    state.reserve_value = re32(state.gpr[code.rd]);
}

void Interpreter::lwaux(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    state.gpr[code.rd] = (s64)(s32)nucleus.memory.read32(addr);
    state.gpr[code.ra] = addr;
}

void Interpreter::lwax(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    state.gpr[code.rd] = (s64)(s32)nucleus.memory.read32(addr);
}

void Interpreter::lwbrx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    state.gpr[code.rd] = re32(nucleus.memory.read32(addr));
}

void Interpreter::lwz(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + code.d : code.d;
    state.gpr[code.rd] = nucleus.memory.read32(addr);
}

void Interpreter::lwzu(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + code.d;
    state.gpr[code.rd] = nucleus.memory.read32(addr);
    state.gpr[code.ra] = addr;
}

void Interpreter::lwzux(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + state.gpr[code.rb];
    state.gpr[code.rd] = nucleus.memory.read32(addr);
    state.gpr[code.ra] = addr;
}

void Interpreter::lwzx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    state.gpr[code.rd] = nucleus.memory.read32(addr);
}

void Interpreter::stb(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + code.d : code.d;
    nucleus.memory.write8(addr, state.gpr[code.rs]);
}

void Interpreter::stbu(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + code.d;
    nucleus.memory.write8(addr, state.gpr[code.rs]);
    state.gpr[code.ra] = addr;
}

void Interpreter::stbux(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + state.gpr[code.rb];
    nucleus.memory.write8(addr, state.gpr[code.rs]);
    state.gpr[code.ra] = addr;
}

void Interpreter::stbx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    nucleus.memory.write8(addr, state.gpr[code.rs]);
}

void Interpreter::std(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + code.d : code.d;
    nucleus.memory.write64(addr, state.gpr[code.rs]);
}

void Interpreter::stdcx_(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    if (state.reserve_addr == addr) {
        bool value = InterlockedCompareExchange((volatile u64*)(nucleus.memory.ptr(addr)), re64(state.gpr[code.rs]), state.reserve_value) == state.reserve_value;
        state.cr.setBit(PPU_CR::CR_EQ, value);
    }
    else {
        state.cr.setBit(PPU_CR::CR_EQ, false);
    }
}

void Interpreter::stdu(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + (code.ds << 2);
    nucleus.memory.write64(addr, state.gpr[code.rs]);
    state.gpr[code.ra] = addr;
}

void Interpreter::stdux(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + state.gpr[code.rb];
    nucleus.memory.write64(addr, state.gpr[code.rs]);
    state.gpr[code.ra] = addr;
}

void Interpreter::stdx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    nucleus.memory.write64(addr, state.gpr[code.rs]);
}

void Interpreter::stfd(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + code.d : code.d;
    nucleus.memory.write64(addr, state.fpr[code.frs]._u64);
}

void Interpreter::stfdu(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + code.d;
    nucleus.memory.write64(addr, state.fpr[code.frs]._u64);
    state.gpr[code.ra] = addr;
}

void Interpreter::stfdux(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + state.gpr[code.rb];
    nucleus.memory.write64(addr, state.fpr[code.frs]._u64);
    state.gpr[code.ra] = addr;
}

void Interpreter::stfdx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    nucleus.memory.write64(addr, state.fpr[code.frs]._u64);
}

void Interpreter::stfiwx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    nucleus.memory.write32(addr, (u32&)state.fpr[code.frs]);
}

void Interpreter::stfs(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + code.d : code.d;
    const f32 value = state.fpr[code.frs]._f64;
    nucleus.memory.write32(addr, (u32&)value);
}

void Interpreter::stfsu(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + code.d;
    const f32 value = state.fpr[code.frs]._f64;
    nucleus.memory.write32(addr, (u32&)value);
    state.gpr[code.ra] = addr;
}

void Interpreter::stfsux(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + state.gpr[code.rb];
    const f32 value = state.fpr[code.frs]._f64;
    nucleus.memory.write32(addr, (u32&)value);
    state.gpr[code.ra] = addr;
}

void Interpreter::stfsx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    const f32 value = state.fpr[code.frs]._f64;
    nucleus.memory.write32(addr, (u32&)value);
}

void Interpreter::sth(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + code.d : code.d;
    nucleus.memory.write16(addr, state.gpr[code.rs]);
}

void Interpreter::sthbrx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    nucleus.memory.write16(addr, re16((u16)state.gpr[code.rs]));
}

void Interpreter::sthu(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + code.d;
    nucleus.memory.write16(addr, state.gpr[code.rs]);
    state.gpr[code.ra] = addr;
}

void Interpreter::sthux(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + state.gpr[code.rb];
    nucleus.memory.write16(addr, state.gpr[code.rs]);
    state.gpr[code.ra] = addr;
}

void Interpreter::sthx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    nucleus.memory.write16(addr, state.gpr[code.rs]);
}

void Interpreter::stmw(Instruction code)
{
    u32 addr = code.ra ? state.gpr[code.ra] + code.d : code.d;
    for (int i = code.rs; i < 32; i++) {
        nucleus.memory.write32(addr, state.gpr[i]);
        addr += 4;
    }
}

void Interpreter::stswi(Instruction code)
{
    u64 ea = code.ra ? state.gpr[code.ra] : 0;
    u64 n = code.nb ? code.nb : 32;
    u8 reg = code.rd;

    while (n > 0) {
        if (n > 3) {
            nucleus.memory.write32(ea, state.gpr[reg]);
            ea += 4;
            n -= 4;
        }
        else {
            u32 buf = state.gpr[reg];
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

void Interpreter::stswx(Instruction code)
{
    unknown("stwsx");
}

void Interpreter::stw(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + code.d : code.d;
    nucleus.memory.write32(addr, state.gpr[code.rs]);
}

void Interpreter::stwbrx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    nucleus.memory.write32(addr, re32(state.gpr[code.rs]));
}

void Interpreter::stwcx_(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    if (state.reserve_addr == addr) {
        bool value = InterlockedCompareExchange((volatile u32*)(nucleus.memory.ptr(addr)), re32((u32)state.gpr[code.rs]), (u32)state.reserve_value) == (u32)state.reserve_value;
        state.cr.setBit(PPU_CR::CR_EQ, value);
    }
    else {
        state.cr.setBit(PPU_CR::CR_EQ, false);
    }
}

void Interpreter::stwu(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + code.d;
    nucleus.memory.write32(addr, state.gpr[code.rs]);
    state.gpr[code.ra] = addr;
}

void Interpreter::stwux(Instruction code)
{
    const u32 addr = state.gpr[code.ra] + state.gpr[code.rb];
    nucleus.memory.write32(addr, state.gpr[code.rs]);
    state.gpr[code.ra] = addr;
}

void Interpreter::stwx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    nucleus.memory.write32(addr, state.gpr[code.rs]);
}

void Interpreter::eieio(Instruction code)
{
    // TODO: _mm_fence();
}

void Interpreter::isync(Instruction code)
{
    // TODO: _mm_fence();
}

void Interpreter::sync(Instruction code)
{
    // TODO: _mm_fence();
}

}  // namespace ppu
}  // namespace cpu
