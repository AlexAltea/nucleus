/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "util.h"
#include "nucleus/emulator.h"
#include "nucleus/logger/logger.h"
#include "nucleus/system/scei/cellos/lv2.h"
#include "nucleus/cpu/cpu_guest.h"
#include "nucleus/cpu/hir/function.h"
#include "nucleus/cpu/frontend/ppu/ppu_decoder.h"
#include "nucleus/cpu/frontend/ppu/ppu_state.h"
#include "nucleus/cpu/frontend/ppu/ppu_tables.h"
#include "nucleus/cpu/frontend/ppu/ppu_thread.h"

#ifdef NUCLEUS_TARGET_WINDOWS
#include <Windows.h>
#endif

namespace cpu {

void nucleusTranslate(void* guestFunc, U64 guestAddr) {
    auto* function = static_cast<frontend::ppu::Function*>(guestFunc);
    function->analyze_cfg();
    function->recompile();

    auto* hirFunction = function->hirFunction;
    auto* cpu = static_cast<GuestCPU*>(CPU::getCurrentThread()->parent);
    auto* state = static_cast<frontend::ppu::PPUThread*>(CPU::getCurrentThread())->state.get();
    cpu->compiler->compile(hirFunction);
    cpu->compiler->call(hirFunction, state);
}

void nucleusCall(U64 guestAddr) {
    auto* thread = static_cast<frontend::ppu::PPUThread*>(CPU::getCurrentThread());
    auto* state = thread->state.get();

    state->pc = guestAddr;
    thread->task();
}

void nucleusSysCall() {
#if !defined(NUCLEUS_BUILD_TEST)
    auto* thread = CPU::getCurrentThread();
    auto* state = static_cast<frontend::ppu::PPUThread*>(thread)->state.get();
    auto* emu = thread->getEmulator();
    static_cast<sys::LV2*>(emu->sys.get())->call(*state);
#endif
}

void nucleusHook(U32 fnid) {
#if !defined(NUCLEUS_BUILD_TEST)
    auto* thread = CPU::getCurrentThread();
    auto* state = static_cast<frontend::ppu::PPUThread*>(thread)->state.get();
    auto* emu = thread->getEmulator();
    auto* lv2 = static_cast<sys::LV2*>(emu->sys.get());
    lv2->modules.call(*state, lv2, fnid);
#endif
}

void nucleusLog(U64 guestAddr) {
    auto* thread = CPU::getCurrentThread();
    auto* state = static_cast<frontend::ppu::PPUThread*>(thread)->state.get();
    auto* emu = thread->getEmulator();
    frontend::ppu::Instruction instr { emu->memory->read32(guestAddr) };
    printf("> [%08X] %s\n", U32(guestAddr), frontend::ppu::get_entry(instr).name);
    int a = 0;
    a += 1;
}

U64 nucleusTime() {
#ifdef NUCLEUS_TARGET_WINDOWS
    static struct PerformanceFreqHolder {
        U64 value;
        PerformanceFreqHolder() {
            LARGE_INTEGER freq;
            QueryPerformanceFrequency(&freq);
            value = freq.QuadPart;
        }
    } freq;

    LARGE_INTEGER cycle;
    QueryPerformanceCounter(&cycle);
    const U64 sec = cycle.QuadPart / freq.value;
    return sec * 79800000 + (cycle.QuadPart % freq.value) * 79800000 / freq.value;
#else
    logger.error(LOG_CPU, "Could not get the Timebase value");
    return 0;
#endif
}

}  // namespace cpu
