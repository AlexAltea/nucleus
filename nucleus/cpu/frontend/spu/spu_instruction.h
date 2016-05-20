/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace cpu {
namespace frontend {
namespace spu {

// SPU Instruction
union Instruction
{
#define FIELD(from, to, type) struct{ U32:(32-to-1); type:(to-from+1); U32:from; }

    /**
     * SPU Instruction fields:
     * Represents the bit fields contained in 32-bit SPU instructions.
     */

    U32 value;

    // Opcode fields
    FIELD( 0,  3, U32 opcode);  // Primary opcode
    FIELD(26, 31, U32 op4);     // Extended opcode of 6-bits (up to 0x3F)

    // Instruction fields
    FIELD(11, 17, U32 i7);      // Immediate (7-bit)
    FIELD( 8, 17, U32 i10);     // Immediate (10-bit)
    FIELD( 9, 24, U32 i16);     // Immediate (16-bit)
    FIELD( 7, 24, U32 i18);     // Immediate (18-bit)
    FIELD(18, 24, U32 ra);      // GPR: Source
    FIELD(11, 17, U32 rb);      // GPR: Source
    FIELD(25, 31, U32 rc);      // GPR: Source
    FIELD(25, 31, U32 rt);      // GPR: Source
    FIELD( 4, 10, U32 rt_);     // GPR: Source

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
    U32 get_target(U32 currentAddr) const;

#undef FIELD
};

}  // namespace spu
}  // namespace frontend
}  // namespace cpu
