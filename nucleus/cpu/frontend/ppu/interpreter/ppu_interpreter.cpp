/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_interpreter.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/frontend/ppu/ppu_tables.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>

namespace cpu {
namespace ppu {

Interpreter::Interpreter(U32 entry, U32 stack)
{
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
    logger.error(LOG_CPU, "Unknown instruction: %08X (PC: %08X)", code.instruction, state.pc);
}
void Interpreter::unknown(const char* instruction)
{
    logger.error(LOG_CPU, "Unknown instruction: %s", instruction);
}

}  // namespace ppu
}  // namespace cpu
