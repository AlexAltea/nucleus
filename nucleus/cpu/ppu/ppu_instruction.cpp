/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_instruction.h"

namespace cpu {
namespace ppu {

bool Instruction::is_valid()
{
    // TODO: ?
    return false;
}

bool Instruction::is_branch()
{
    // If instruction is {b, bc, bclr, bcctr}
    if ((opcode == 0x10) || (opcode == 0x12) || (opcode == 0x13 && (op19 == 0x010 || op19 == 0x210))) {
        return true;
    } else {
        return false;
    }
}

bool Instruction::is_return()
{
    // If instruction is {bclr}
    if (opcode == 0x13 && op19 == 0x010) {
        return true;
    } else {
        return false;
    }
}

}  // namespace ppu
}  // namespace cpu
