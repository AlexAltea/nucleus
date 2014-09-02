/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/decoder.h"
#include "nucleus/cpu/ppu/instructions.h"

/**
 * Instruction fields
 */
static UCodeField<0, 31> instruction;    // Primary opcode
static UCodeField<0, 5> opcode;          // Primary opcode
static UCodeField<26, 31> op4;           // Extended opcode of 6-bits (up to 0x3F)
static UCodeField<21, 31> op4_;          // Extended opcode of 11-bits (up to 0x7FF)
static UCodeField<21, 30> op19;          // Extended opcode of 10-bits (up to 0x3FF)
static UCodeField<27, 29> op30;          // Extended opcode of 3-bits (up to 0x7)
static UCodeField<21, 30> op31;          // Extended opcode of 10-bits (up to 0x3FF)
static UCodeField<30, 31> op58;          // Extended opcode of 2-bits (up to 0x3)
static UCodeField<26, 30> op59;          // Extended opcode of 5-bits (up to 0x1F)
static UCodeField<30, 31> op62;          // Extended opcode of 2-bits (up to 0x3)
static UCodeField<26, 30> op63;          // Extended opcode of 5-bits (up to 0x1F)
static UCodeField<21, 30> op63_;         // Extended opcode of 10-bits (up to 0x3FF)

static UCodeFieldSplit<21, 25, 26, 26> mb_; // First 1 bit of a 64-bit mask in rotate instructions
static UCodeFieldSplit<21, 25, 26, 26> me_; // Last 1 bit of a 64-bit mask in rotate instructions
static UCodeFieldSplit<16, 20, 30, 30> sh_; // Shift amount

static UCodeField<30> aa;          // Bit/Flags: Absolute address bit
static UCodeField<31> lk;          // Bit/Flags: Link bit: Update the link register (LR)
static UCodeField<21> oe;          // Bit/Flags: OE bit: Enable enable setting OV and SO in the XER
static UCodeField<31> rc;          // Bit/Flags: Record bit: Update the condition register (CR)
static UCodeField<6> l6;           // Bit/Flags: ?
static UCodeField<10> l10;         // Bit/Flags: ?
static UCodeField<11> l11;         // Bit/Flags: ?
static UCodeField<9, 10> l9_10;    // Bit/Flags: ?
static UCodeField<6, 10> bo;       // Branching: Options for the branch conditional instructions
static UCodeField<11, 15> bi;      // Branching: CR bit to trigger branch conditional instructions
static SCodeField<16, 31> bd;      // Branching: Immediate 14-bit signed integer for branch displacement
static UCodeField<19, 20> bh;      // ?
static UCodeField<11, 13> bfa;     // ?
static UCodeField<6, 8> crfd;      // CR fields: Destination CR or FPSCR field
static UCodeField<11, 13> crfs;    // CR fields: Source CR or FPSCR field
static UCodeField<6, 10> crbd;     // CR fields: Destination bit in the CR or FPSCR
static UCodeField<11, 15> crba;    // CR fields: Source bit in the CR
static UCodeField<16, 20> crbb;    // CR fields: Source bit in the CR
static UCodeField<12, 19> crm;     // Identify the CR fields that are to be updated by the mtcrf instruction
static SCodeField<16, 31> d;       // Immediate 16-bit signed integer
static UCodeField<16, 27> dq;      // ?
static SCodeField<16, 29> ds;      // ?               // TODO: li, ll, ds are shifted to left with 2 (add "00" in binary)
static UCodeField<7, 14> fm;       // ?
static UCodeField<6, 10> frd;      // FPR: Destination
static UCodeField<6, 10> frs;      // FPR: Source
static UCodeField<11, 15> fra;     // FPR: Source
static UCodeField<16, 20> frb;     // FPR: Source
static UCodeField<21, 25> frc;     // FPR: Source
static UCodeField<16, 19> i;       // ?
static SCodeField<6, 29> li;       // Branching:      // TODO: li, ll, ds are shifted to left with 2 (add "00" in binary)
static SCodeField<6, 29> ll;       // Branching:      // TODO: li, ll, ds are shifted to left with 2 (add "00" in binary)
static UCodeField<21, 25> mb;      // First 1 bit of a 64-bit mask in rotate instructions
static UCodeField<26, 30> me;      // Last 1 bit of a 64-bit mask in rotate instructions
static UCodeField<16, 20> nb;      // Number of bytes to move in an immediate string load or store
static UCodeField<6, 10> rd;       // GPR: Destination
static UCodeField<6, 10> rs;       // GPR: Source
static UCodeField<11, 15> ra;      // GPR: Source
static UCodeField<16, 20> rb;      // GPR: Source
static UCodeField<16, 20> sh;      // Shift amount
static UCodeField<11, 20> spr;     // Special-purpose register
static UCodeField<9, 10> strm;     // ?
static UCodeField<6, 31> sys;      // ?
static SCodeField<16, 31> simm;    // Immediate 16-bit signed integer
static UCodeField<16, 31> uimm;    // Immediate 16-bit unsigned integer
static UCodeField<9, 10> th;       // Data stream variant of the dcbt instruction
static UCodeField<6, 10> to;       // Trap conditions
static UCodeField<6, 10> vd;       // Vector/SIMD: Destination vector register
static UCodeField<6, 10> vs;       // Vector/SIMD: Source vector register
static UCodeField<11, 15> va;      // Vector/SIMD: Source vector register
static UCodeField<16, 20> vb;      // Vector/SIMD: Source vector register
static UCodeField<21, 25> vc;      // Vector/SIMD: Source vector register
static UCodeField<22, 25> vshb;    // Vector/SIMD: Specifies a shift amount in bytes
static SCodeField<11, 15> vsimm;   // Vector/SIMD: Immediate 5-bit signed integer
static UCodeField<11, 15> vuimm;   // Vector/SIMD: Immediate 5-bit unsigned integer

/**
 * Instruction tables
 */
static InstrList<1 << opcode.size, PPUInstructions> listMain(opcode, instr_bind(&PPUInstructions::unknown, instruction));
