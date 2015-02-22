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

u64& getRegBySPR(PPUThread& thread, u32 spr)
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
 * PPC64 Instructions:
 *  - UISA: Processor Control Instructions (Section: 4.2.5)
 *  - VEA: Processor Control Instructions (Section: 4.3.1)
 *  - VEA: Memory Control Instructions (Section: 4.3.3)
 *  - VEA: External Control Instructions (Section: 4.3.4)
 */

void Interpreter::mfocrf(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = bitReverse32(thread.cr.CR);
}

void Interpreter::mfspr(Instruction code, PPUThread& thread)
{
    thread.gpr[code.rd] = getRegBySPR(thread, code.spr);
}

void Interpreter::mtocrf(Instruction code, PPUThread& thread)
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

void Interpreter::mtspr(Instruction code, PPUThread& thread)
{
    getRegBySPR(thread, code.spr) = thread.gpr[code.rs];
}

void Interpreter::mftb(Instruction code, PPUThread& thread)
{
    const u32 n = (code.spr >> 5) | ((code.spr & 0x1f) << 5);
    thread.tb = getTimebase();

    switch (n) {
    case 0x10C: thread.gpr[code.rd] = thread.tb; break;
    case 0x10D: thread.gpr[code.rd] = thread.tbu; break;
    default: unknown("mftb"); break;
    }
}

void Interpreter::dcbf(Instruction code, PPUThread& thread)
{
    //unknown("dcbf", false);
    // TODO: _mm_fence();
}

void Interpreter::dcbst(Instruction code, PPUThread& thread)
{
    //unknown("dcbst", false);
    // TODO: _mm_fence();
}

void Interpreter::dcbt(Instruction code, PPUThread& thread)
{
    //unknown("dcbt", false);
    // TODO: _mm_fence();
}

void Interpreter::dcbtst(Instruction code, PPUThread& thread)
{
    //unknown("dcbtst", false);
    // TODO: _mm_fence();
}

void Interpreter::dcbz(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    void* cache_line = nucleus.memory.ptr(addr & ~127);
    if (cache_line) {
        memset(cache_line, 0, 128);
    }
    // TODO: _mm_fence();
}

void Interpreter::icbi(Instruction code, PPUThread& thread)
{
    // Nothing to do in the interpreter.
}

void Interpreter::eciwx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    thread.gpr[code.rd] = nucleus.memory.read32(addr);
}

void Interpreter::ecowx(Instruction code, PPUThread& thread)
{
    const u32 addr = code.ra ? thread.gpr[code.ra] + thread.gpr[code.rb] : thread.gpr[code.rb];
    nucleus.memory.write32(addr, thread.gpr[code.rs]);
}

}  // namespace ppu
}  // namespace cpu
