/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "util.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/frontend/ppu/ppu_decoder.h"
#include "nucleus/cpu/frontend/ppu/ppu_tables.h"

namespace cpu {

void nucleusTranslate(void* guestFunc, U64 guestAddr) {
    auto* function = static_cast<frontend::ppu::Function*>(guestFunc);

    function->analyze_cfg();
    function->recompile();

    nucleus.cell.compiler->compile(function->hirFunction);
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

}  // namespace cpu
