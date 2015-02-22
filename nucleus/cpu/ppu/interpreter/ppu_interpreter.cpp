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

Interpreter::Interpreter(PPUThread& thr) : thread(thr)
{
    Interpreter::initRotateMask();
}

void Interpreter::step()
{
    // Callback finished
    if (thread.pc == 0) {
        thread.finishCallback();
        return;
    }

    const Instruction code = { nucleus.memory.read32(thread.pc) };
    const Entry& entry = get_entry(code);

    entry.interpreter(code, thread);
    thread.pc += 4;
}

// Unknown instruction
void Interpreter::unknown(Instruction code, PPUThread& thread)
{
    nucleus.log.error(LOG_CPU, "Unknown instruction: %08X (PC: %08X)", code.instruction, thread.pc);
}
void Interpreter::unknown(const char* instruction)
{
    nucleus.log.error(LOG_CPU, "Unknown instruction: %s", instruction);
}

}  // namespace ppu
}  // namespace cpu
