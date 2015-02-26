/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_interpreter.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/ppu/ppu_tables.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>

namespace cpu {
namespace ppu {

u64 Interpreter::rotateMask[64][64];
void Interpreter::initRotateMask()
{
    static bool initialized = false;
    if (initialized) {
        return;
    }
    for (u32 mb = 0; mb < 64; mb++) {
        for (u32 me = 0; me < 64; me++) {
            const u64 mask = (~0ULL >> mb) ^ ((me >= 63) ? 0 : ~0ULL >> (me + 1));
            Interpreter::rotateMask[mb][me] = mb > me ? ~mask : mask;
        }
    }
    initialized = true;
}

Interpreter::Interpreter(u32 entry, u32 stack)
{
    Interpreter::initRotateMask();

    const u32 entry_pc = nucleus.memory.read32(entry);
    const u32 entry_rtoc = nucleus.memory.read32(entry+4);

    // Initialize Program Counter
    state.pc = entry_pc;

    // Initialize UISA Registers (TODO: All of this might be wrong)
    state.gpr[0] = entry_pc;
    state.gpr[1] = stack - 0x200;
    state.gpr[2] = entry_rtoc;
    state.gpr[3] = 0;
    state.gpr[4] = stack - 0x80;
    state.gpr[5] = state.gpr[4] + 0x10;
    state.gpr[11] = entry;
    state.gpr[12] = nucleus.lv2.proc.param.malloc_pagesize;
    state.gpr[13] = nucleus.memory(SEG_USER_MEMORY).getBaseAddr() + 0x7060; // TLS
    state.cr.CR = 0x22000082;
    state.tb.TBL = 1;
    state.tb.TBU = 1;

    // Arguments passed to sys_initialize_tls on liblv2.sprx's start function
    state.gpr[7] = 0x0; // TODO
    state.gpr[8] = 0x0; // TODO
    state.gpr[9] = 0x0; // TODO
    state.gpr[10] = 0x90;
}

void Interpreter::step()
{
    const Instruction code = { nucleus.memory.read32(state.pc) };
    auto method = get_entry(code).interpret;
    (this->*method)(code);

    state.pc += 4;
}

// Unknown instruction
void Interpreter::unknown(Instruction code)
{
    nucleus.log.error(LOG_CPU, "Unknown instruction: %08X (PC: %08X)", code.instruction, state.pc);
}
void Interpreter::unknown(const char* instruction)
{
    nucleus.log.error(LOG_CPU, "Unknown instruction: %s", instruction);
}

}  // namespace ppu
}  // namespace cpu
