/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_instruction.h"
#include "nucleus/cpu/ppu/ppu_tables.h"

namespace cpu {
namespace ppu {

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
    // If instruction is {bl}
    if (opcode == 0x12 && lk == 1) {
        return true;
    }
    return false;
}

bool Instruction::is_return() const
{
    // If instruction is {bclr*}
    if (opcode == 0x13 && op19 == 0x010) {
        return true;
    }
    return false;
}

u32 Instruction::get_target(u32 currentAddr) const
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
    return 0;
}

}  // namespace ppu
}  // namespace cpu
