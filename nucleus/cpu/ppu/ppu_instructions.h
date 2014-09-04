/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

/**
 * PPU Instruction:
 * Consist in a 32-bit value divided in multiple bit fields
 */
union PPUInstruction
{
#define FIELD(from, to, type) struct{ u32:(32-to-1); type:(to-from+1); u32:from; }

    u32 instruction;

    // Opcode fields
    FIELD( 0,  5, u32 opcode);  // Primary opcode
    FIELD(26, 31, u32 op4);     // Extended opcode of 6-bits (up to 0x3F)
    FIELD(21, 31, u32 op4_);    // Extended opcode of 11-bits (up to 0x7FF)
    FIELD(21, 30, u32 op19);    // Extended opcode of 10-bits (up to 0x3FF)
    FIELD(27, 29, u32 op30);    // Extended opcode of 3-bits (up to 0x7)
    FIELD(21, 30, u32 op31);    // Extended opcode of 10-bits (up to 0x3FF)
    FIELD(30, 31, u32 op58);    // Extended opcode of 2-bits (up to 0x3)
    FIELD(26, 30, u32 op59);    // Extended opcode of 5-bits (up to 0x1F)
    FIELD(30, 31, u32 op62);    // Extended opcode of 2-bits (up to 0x3)
    FIELD(26, 30, u32 op63);    // Extended opcode of 5-bits (up to 0x1F)
    FIELD(21, 30, u32 op63_);   // Extended opcode of 10-bits (up to 0x3FF)

    // Instruction fields
    FIELD(30, 30, u32 aa);      // Bit/Flags: Absolute address bit
    FIELD(31, 31, u32 lk);      // Bit/Flags: Link bit: Update the link register (LR)
    FIELD(21, 21, u32 oe);      // Bit/Flags: OE bit: Enable enable setting OV and SO in the XER
    FIELD(31, 31, u32 rc);      // Bit/Flags: Record bit: Update the condition register (CR)
    FIELD( 6,  6, u32 l6);      // Bit/Flags: ?
    FIELD(10, 10, u32 l10);     // Bit/Flags: ?
    FIELD(11, 11, u32 l11);     // Bit/Flags: ?
    FIELD( 9, 10, u32 l9_10);   // Bit/Flags: ?
    FIELD( 6, 10, u32 bo);      // Branching: Options for the branch conditional instructions
    FIELD(11, 15, u32 bi);      // Branching: CR bit to trigger branch conditional instructions
    FIELD(16, 31, s32 bd);      // Branching: Immediate 14-bit signed integer for branch displacement
    FIELD(19, 20, u32 bh);      // ?
    FIELD(11, 13, u32 bfa);     // ?
    FIELD( 6,  8, u32 crfd);    // CR fields: Destination CR or FPSCR field
    FIELD(11, 13, u32 crfs);    // CR fields: Source CR or FPSCR field
    FIELD( 6, 10, u32 crbd);    // CR fields: Destination bit in the CR or FPSCR
    FIELD(11, 15, u32 crba);    // CR fields: Source bit in the CR
    FIELD(16, 20, u32 crbb);    // CR fields: Source bit in the CR
    FIELD(12, 19, u32 crm);     // Identify the CR fields that are to be updated by the mtcrf instruction
    FIELD(16, 31, s32 d);       // Immediate 16-bit signed integer
    FIELD(16, 27, u32 dq);      // ?
    FIELD(16, 29, s32 ds);      // ?
    FIELD( 7, 14, u32 fm);      // ?
    FIELD( 6, 10, u32 frd);     // FPR: Destination
    FIELD( 6, 10, u32 frs);     // FPR: Source
    FIELD(11, 15, u32 fra);     // FPR: Source
    FIELD(16, 20, u32 frb);     // FPR: Source
    FIELD(21, 25, u32 frc);     // FPR: Source
    FIELD(16, 19, u32 i);       // ?
    FIELD( 6, 29, s32 li);      // Branching:
    FIELD( 6, 29, s32 ll);      // Branching:
    FIELD(21, 25, u32 mb);      // First 1 bit of a 64-bit mask in rotate instructions
    FIELD(26, 30, u32 me);      // Last 1 bit of a 64-bit mask in rotate instructions
    FIELD(16, 20, u32 nb);      // Number of bytes to move in an immediate string load or store
    FIELD( 6, 10, u32 rd);      // GPR: Destination
    FIELD( 6, 10, u32 rs);      // GPR: Source
    FIELD(11, 15, u32 ra);      // GPR: Source
    FIELD(16, 20, u32 rb);      // GPR: Source
    FIELD(16, 20, u32 sh);      // Shift amount
    FIELD(11, 20, u32 spr);     // Special-purpose register
    FIELD( 9, 10, u32 strm);    // ?
    FIELD( 6, 31, u32 sys);     // ?
    FIELD(16, 31, s32 simm);    // Immediate 16-bit signed integer
    FIELD(16, 31, u32 uimm);    // Immediate 16-bit unsigned integer
    FIELD( 9, 10, u32 th);      // Data stream variant of the dcbt instruction
    FIELD( 6, 10, u32 to);      // Trap conditions
    FIELD( 6, 10, u32 vd);      // Vector/SIMD: Destination vector register
    FIELD( 6, 10, u32 vs);      // Vector/SIMD: Source vector register
    FIELD(11, 15, u32 va);      // Vector/SIMD: Source vector register
    FIELD(16, 20, u32 vb);      // Vector/SIMD: Source vector register
    FIELD(21, 25, u32 vc);      // Vector/SIMD: Source vector register
    FIELD(22, 25, u32 vshb);    // Vector/SIMD: Specifies a shift amount in bytes
    FIELD(11, 15, s32 vsimm);   // Vector/SIMD: Immediate 5-bit signed integer
    FIELD(11, 15, u32 vuimm);   // Vector/SIMD: Immediate 5-bit unsigned integer

    /*
    static UCodeFieldSplit<21, 25, 26, 26> mb_; // First 1 bit of a 64-bit mask in rotate instructions
    static UCodeFieldSplit<21, 25, 26, 26> me_; // Last 1 bit of a 64-bit mask in rotate instructions
    static UCodeFieldSplit<16, 20, 30, 30> sh_; // Shift amount
    */

#undef FIELD
};
