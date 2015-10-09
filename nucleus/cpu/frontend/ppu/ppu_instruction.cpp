/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_instruction.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/frontend/ppu/ppu_tables.h"

namespace cpu {
namespace frontend {
namespace ppu {

Instruction::Instruction() : instruction(0)
{
}

Instruction::Instruction(U32 addr) : instruction(nucleus.memory.read32(addr))
{
}

bool Instruction::is_valid() const
{
    // If instruction is not defined in the PPU architecture
    if (get_entry(*this).type == ENTRY_INVALID) {
        return false;
    }
    return true;
}

bool Instruction::is_branch() const
{
    // If instruction is {b*, bc*, bclr*, bcctr*}
    if ((opcode == 0x10) || (opcode == 0x12) || (opcode == 0x13 && (op19 == 0x010 || op19 == 0x210))) {
        return true;
    }
    return false;
}

bool Instruction::is_branch_conditional() const
{
    // If instruction is {bc*}
    if (opcode == 0x10) {
        return true;
    }
    // If instruction is {bclr*} with conditions
    if (opcode == 0x13 && op19 == 0x010) {
        const U8 bo0 = (bo & 0x10);
	    const U8 bo2 = (bo & 0x04);
        if (!bo0 || !bo2) {
            return true;
        }
    }
    return false;
}

bool Instruction::is_branch_unconditional() const
{
    // If instruction is {b*}
    if (opcode == 0x12) {
        return true;
    }
    return false;
}

bool Instruction::is_call() const
{
    // If instruction is {bl*} U {bctrl}
    return is_call_known() || is_call_unknown();
}

bool Instruction::is_call_known() const
{
    // If instruction is {bl*}
    return (opcode == 0x12 && lk == 1);
}

bool Instruction::is_call_unknown() const
{
    // If instruction is {bctrl}
    return (opcode == 0x13 && op19 == 0x210 && lk == 1);
}

bool Instruction::is_return() const
{
    // If instruction is {bclr*}
    if (opcode == 0x13 && op19 == 0x010) {
        // Discard conditional return
        const U8 bo0 = (bo & 0x10);
	    const U8 bo2 = (bo & 0x04);
        if (!bo0 || !bo2) {
            return false;
        }
        return true;
    }
    return false;
}

U32 Instruction::get_target(U32 currentAddr) const
{
    // If instruction is {bc*}
    if (opcode == 0x10) {
        if (aa) {
            return (bd << 2);
        }
        return (bd << 2) + currentAddr;
    }
    // If instruction is {b*}
    if (opcode == 0x12) {
        if (aa) {
            return (li << 2);
        }
        return (li << 2) + currentAddr;
    }
    // If instruction is {bclr*} with conditions
    if (opcode == 0x13 && op19 == 0x010) {
        const U8 bo0 = (bo & 0x10);
	    const U8 bo2 = (bo & 0x04);
        if (!bo0 || !bo2) {
            return currentAddr + 4;
        }
    }
    return 0;
}

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
