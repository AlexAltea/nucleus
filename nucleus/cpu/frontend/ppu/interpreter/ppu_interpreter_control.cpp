/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
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

u64& getRegBySPR(State& state, u32 spr)
{
    const u32 n = (spr >> 5) | ((spr & 0x1F) << 5);

    switch (n) {
    case 0x001: return state.xer.XER;
    case 0x008: return state.lr;
    case 0x009: return state.ctr;
    }

    //unknown("GetRegBySPR error: Unknown SPR!");
    return state.xer.XER;
}

/**
 * PPC64 Instructions:
 *  - UISA: Processor Control Instructions (Section: 4.2.5)
 *  - VEA: Processor Control Instructions (Section: 4.3.1)
 *  - VEA: Memory Control Instructions (Section: 4.3.3)
 *  - VEA: External Control Instructions (Section: 4.3.4)
 */

void Interpreter::mfocrf(Instruction code)
{
    state.gpr[code.rd] = bitReverse32(state.cr.CR);
}

void Interpreter::mfspr(Instruction code)
{
    state.gpr[code.rd] = getRegBySPR(state, code.spr);
}

void Interpreter::mtocrf(Instruction code)
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
            state.cr.setField(7 - n, (state.gpr[code.rs] >> (4*n)) & 0xf);
        }
        else {
            state.cr.CR = 0;
        }
    }
    else {
        u32 mask = 0;
        for (int i = 0; i < 8; i++) {
            if (code.crm & (1 << i)) {
                mask |= (0xF << (i * 4));
            }
        }
        state.cr.CR = bitReverse32(state.gpr[code.rs] & mask) | (bitReverse32(state.cr.CR) & ~mask);
    }
}

void Interpreter::mtspr(Instruction code)
{
    getRegBySPR(state, code.spr) = state.gpr[code.rs];
}

void Interpreter::mftb(Instruction code)
{
    const u32 n = (code.spr >> 5) | ((code.spr & 0x1f) << 5);
    state.tb.TB = getTimebase();

    switch (n) {
    case 0x10C: state.gpr[code.rd] = state.tb.TB; break;
    case 0x10D: state.gpr[code.rd] = state.tb.TBU; break;
    default: unknown("mftb"); break;
    }
}

void Interpreter::dcbf(Instruction code)
{
}

void Interpreter::dcbst(Instruction code)
{
}

void Interpreter::dcbt(Instruction code)
{
}

void Interpreter::dcbtst(Instruction code)
{
}

void Interpreter::dcbz(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    void* cache_line = nucleus.memory.ptr(addr & ~127);
    if (cache_line) {
        memset(cache_line, 0, 128);
    }
    // TODO: ?
}

void Interpreter::icbi(Instruction code)
{
    // Nothing to do in the interpreter.
}

void Interpreter::eciwx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    state.gpr[code.rd] = nucleus.memory.read32(addr);
}

void Interpreter::ecowx(Instruction code)
{
    const u32 addr = code.ra ? state.gpr[code.ra] + state.gpr[code.rb] : state.gpr[code.rb];
    nucleus.memory.write32(addr, state.gpr[code.rs]);
}

}  // namespace ppu
}  // namespace cpu
