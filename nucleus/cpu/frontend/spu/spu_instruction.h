/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace cpu {
namespace spu {

// SPU Instruction
union Instruction
{
#define FIELD(from, to, type) struct{ u32:(32-to-1); type:(to-from+1); u32:from; }

    /**
     * SPU Instruction fields:
     * Represents the bit fields contained in 32-bit SPU instructions.
     */

    u32 instruction;

    // Opcode fields
    FIELD( 0,  3, u32 opcode);  // Primary opcode
    FIELD(26, 31, u32 op4);     // Extended opcode of 6-bits (up to 0x3F)

    // Instruction fields
    FIELD(11, 17, u32 i7);      // Immediate (7-bit)
    FIELD( 8, 17, u32 i10);     // Immediate (10-bit)
    FIELD( 9, 24, u32 i16);     // Immediate (16-bit)
    FIELD( 7, 24, u32 i18);     // Immediate (18-bit)
    FIELD(18, 24, u32 ra);      // GPR: Source
    FIELD(11, 17, u32 rb);      // GPR: Source
    FIELD(25, 31, u32 rc);      // GPR: Source
    FIELD(25, 31, u32 rt);      // GPR: Source
    FIELD( 4, 10, u32 rt_);     // GPR: Source

    /**
     * SPU Instruction properties:
     * Properties of the current instruction, required by the analyzer.
     */

    // Determines whether the instruction is a valid SPU instruction
    bool is_valid() const;

    // Determines whether the instruction is branch instruction
    bool is_branch() const;
    bool is_branch_conditional() const;
    bool is_branch_unconditional() const;

    // Determines whether the instruction is function call instruction
    bool is_call() const;
    bool is_call_known() const;   // Calling address can be predicted
    bool is_call_unknown() const; // Calling address cannot be predicted
    bool is_jump() const;
    bool is_jump_known() const;   // Jumping address can be predicted
    bool is_jump_unknown() const; // Jumping address cannot be predicted

    // Determines whether the instruction is return instruction
    bool is_return() const;

    // Obtain the target address if the branch is taken
    u32 get_target(u32 currentAddr) const;

#undef FIELD
};

}  // namespace spu
}  // namespace cpu
