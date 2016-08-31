/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace cpu {
namespace frontend {
namespace x86 {

/**
 * X86 State
 * =========
 * This class represents a userland 64-bit x86 thread state.
 *
 * NOTE:
 * To represent 32-bit or 16-bit threads, or the presence/absence of certain
 * extensions, one should simply ignore the unnecessary members or upper bits
 * of some of the members below. Better workarounds will be provided.
 */
class alignas(16) X86State {
public:
    // General-Purpose Registers
    // Index:     0     1     2     3     4     5     6     7     8     9     10    11    12    13    14    15 
    // 64-bit: | RAX | RCX | RDX | RBX | RSP | RBP | RSI | RDI | R8  | R9  | R10 | R11 | R12 | R13 | R14 | R15 |
    // 32-bit: | EAX | ECX | EDX | EBX | ESP | EBP | ESI | EDI | R8D | R9D | R10D| R11D| R12D| R13D| R14D| R15D|
    // 16-bit: |  AX |  CX |  DX |  BX |  SP |  BP |  SI |  DI | R8W | R9W | R10W| R11W| R12W| R13W| R14W| R15W|
    //  8-bit: |  AL |  CL |  DL |  BL |  SPL|  BPL|  SIL|  DIL| R8B | R9B | R10B| R11B| R12B| R13B| R14B| R15B|
    U64 r[16];

    // Segment Registers
    U16 cs;
    U16 ds;
    U16 ss;
    U16 es;
    U16 fs;
    U16 gs;

    // SIMD Registers
    V256 ymm[16];
};

}  // namespace x86
}  // namespace frontend
}  // namespace cpu
