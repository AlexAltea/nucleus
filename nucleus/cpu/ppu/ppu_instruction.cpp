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
    // TODO: ?
    return false;
}

bool Instruction::is_branch() const
{
    // If instruction is {b*, bc*, bclr*, bcctr*}
    if ((opcode == 0x10) || (opcode == 0x12) || (opcode == 0x13 && (op19 == 0x010 || op19 == 0x210))) {
        return true;
    } else {
        return false;
    }
}

bool Instruction::is_call() const
{
    // If instruction is {bl}
    if (opcode == 0x12 && lk == 1) {
        return true;
    } else {
        return false;
    }
}

bool Instruction::is_return() const
{
    // If instruction is {bclr*}
    if (opcode == 0x13 && op19 == 0x010) {
        return true;
    } else {
        return false;
    }
}

}  // namespace ppu
}  // namespace cpu
