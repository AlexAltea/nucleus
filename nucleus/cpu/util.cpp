/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "util.h"
#include "nucleus/emulator.h"
#include "nucleus/logger/logger.h"
#include "nucleus/system/lv2.h"
#include "nucleus/cpu/cpu.h"
#include "nucleus/cpu/hir/function.h"
#include "nucleus/cpu/frontend/ppu/ppu_decoder.h"
#include "nucleus/cpu/frontend/ppu/ppu_state.h"
#include "nucleus/cpu/frontend/ppu/ppu_tables.h"
#include "nucleus/cpu/frontend/ppu/ppu_thread.h"

#ifdef NUCLEUS_PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace cpu {

void nucleusTranslate(void* guestFunc, U64 guestAddr) {
    auto* function = static_cast<frontend::ppu::Function*>(guestFunc);
    function->analyze_cfg();
    function->recompile();

    auto* hirFunction = function->hirFunction;
    auto* cpu = CPU::getCurrentThread()->parent;
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
    auto* state = static_cast<frontend::ppu::PPUThread*>(CPU::getCurrentThread())->state.get();
    static_cast<sys::LV2*>(nucleus.sys.get())->call(*state);
}

void nucleusHook(U32 fnid) {
    auto* state = static_cast<frontend::ppu::PPUThread*>(CPU::getCurrentThread())->state.get();
    static_cast<sys::LV2*>(nucleus.sys.get())->modules.call(*state, fnid);
}

void nucleusLog(U64 guestAddr) {
    auto* state = static_cast<frontend::ppu::PPUThread*>(CPU::getCurrentThread())->state.get();
    //printf("> [%08X] %s\n", U32(guestAddr), frontend::ppu::get_entry(guestAddr).name);
    int a = 0;
    a += 1;
}

U64 nucleusTime() {
#ifdef NUCLEUS_PLATFORM_WINDOWS
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
