/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "util.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/frontend/ppu/ppu_decoder.h"
#include "nucleus/cpu/frontend/ppu/ppu_tables.h"

#ifdef NUCLEUS_PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace cpu {

void nucleusTranslate(void* guestFunc, U64 guestAddr) {
    auto* function = static_cast<frontend::ppu::Function*>(guestFunc);

    function->analyze_cfg();
    function->recompile();

    // Allocate space for compiling the function
    const size_t size = 65536;
    hir::Function* hirFunction = function->hirFunction;
    hirFunction->nativeAddress = nucleus.cell.compiler->allocRWXMemory(size);
    hirFunction->nativeSize = size;

    nucleus.cell.compiler->compile(hirFunction);
    function->call();
}

void nucleusCall(U64 guestAddr) {
    logger.warning(LOG_CPU, "Unimplemented");
}

void nucleusSysCall() {
    auto* state = static_cast<frontend::ppu::Thread*>(nucleus.cell.getCurrentThread())->state.get();
    nucleus.lv2.call(*state);
}


void nucleusLog(U64 guestAddr) {
    auto* state = static_cast<frontend::ppu::Thread*>(nucleus.cell.getCurrentThread())->state.get();
    printf("> [%08X] %s\n", U32(guestAddr), frontend::ppu::get_entry(guestAddr).name);
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
    nucleus.log.error(LOG_CPU, "Could not get the Timebase value");
    return 0;
#endif
}

}  // namespace cpu
