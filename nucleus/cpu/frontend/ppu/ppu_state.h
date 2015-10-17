/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/state.h"

#include <cmath>
#include <string>

namespace cpu {
namespace frontend {
namespace ppu {

// FPSCR Rounding control field values
enum FPSCR_RN {
    FPSCR_RN_NEAR = 0,
    FPSCR_RN_ZERO = 1,
    FPSCR_RN_PINF = 2,
    FPSCR_RN_MINF = 3,
};

// FPSCR Result flags
enum FPSCR_FPRF {
    FPR_FPRF_PZ   = 0x2,
    FPR_FPRF_PN   = 0x4,
    FPR_FPRF_PINF = 0x5,
    FPR_FPRF_NN   = 0x8,
    FPR_FPRF_NINF = 0x9,
    FPR_FPRF_QNAN = 0x11,
    FPR_FPRF_NZ   = 0x12,
    FPR_FPRF_PD   = 0x14,
    FPR_FPRF_ND   = 0x18,
};

// FPSCR flags
enum FPSCR_EXP {
    FPSCR_FX      = 0x80000000,
    FPSCR_FEX     = 0x40000000,
    FPSCR_VX      = 0x20000000,
    FPSCR_OX      = 0x10000000,
    FPSCR_UX      = 0x08000000,
    FPSCR_ZX      = 0x04000000,
    FPSCR_XX      = 0x02000000,
    FPSCR_VXSNAN  = 0x01000000,
    FPSCR_VXISI   = 0x00800000,
    FPSCR_VXIDI   = 0x00400000,
    FPSCR_VXZDZ   = 0x00200000,
    FPSCR_VXIMZ   = 0x00100000,
    FPSCR_VXVC    = 0x00080000,
    FPSCR_FR      = 0x00040000,
    FPSCR_FI      = 0x00020000,
    FPSCR_VXSOFT  = 0x00000400,
    FPSCR_VXSQRT  = 0x00000200,
    FPSCR_VXCVI   = 0x00000100,
};

/**
 * PPU Registers
 */

// Condition Register
union PPU_CR {
    U32 CR;

    // Bit index
    enum {
        CR_LT = 0,
        CR_GT = 1,
        CR_EQ = 2,
        CR_SO = 3,
    };

    U8 getBit(U32 bit) { return (CR >> bit) & 1; }
    void setBit(U32 bit, bool value) { CR = value ? CR | (1 << bit) : CR & ~(1 << bit); }

    U8 getField(U32 field) { return (CR >> field*4) & 0xf; }
    void setField(U32 field, U8 value) {
        U64 maskHigh = ~((1ULL << (field+1)*4)-1);
        U64 maskLow = ((1ULL << field*4)-1);
        CR = (CR & (U32)maskHigh) | (value << field*4) | (CR & (U32)maskLow);
    }

    template <typename T>
    void updateField(U32 field, T a, T b) {
        if (a < b) {
            setField(field, 1 << CR_LT);
        }
        else if (a > b) {
            setField(field, 1 << CR_GT);
        }
        else if (a == b) {
            setField(field, 1 << CR_EQ);
        }
        else {
            setField(field, 1 << CR_SO);
        }
    }
};

// Floating-Point Register
union PPU_FPR {
    F64 f64;
    U64 u64;

    enum : U64 {
        FPR_NAN = 0x7FF8000000000000ULL,
    };

    bool operator== (F64 right) { return f64 == right; }
    bool operator== (U64 right) { return u64 == right; }

    bool isInf() {
        return (u64 & 0x7FFFFFFFFFFFFFFFULL) == 0x7FF0000000000000ULL;
    }

    bool isNaN() {
        return std::isnan(f64);
    }

    bool isSNaN() {
        return
            (u64 & 0x7FF0000000000000ULL) == 0x7FF0000000000000ULL &&
            (u64 & 0x000FFFFFFFFFFFFFULL) != 0ULL &&
            (u64 & 0x0008000000000000ULL) == 0ULL;
    }

    static int compare(PPU_FPR& a, PPU_FPR& b) {
        if(a.f64  < b.f64) return PPU_CR::CR_LT;
        if(a.f64  > b.f64) return PPU_CR::CR_GT;
        if(a.f64 == b.f64) return PPU_CR::CR_EQ;
        return PPU_CR::CR_SO;
    }
};

// Floating-Point Status and Control Register
union PPU_FPSCR {
    U32 FPSCR;

    struct {
        U32 RN      :2; // Rounding control
        U32 NI      :1; // Non-IEEE mode
        U32 XE      :1; // Inexact exception enable
        U32 ZE      :1; // IEEE Zero divide exception enable
        U32 UE      :1; // IEEE Underflow exception enable
        U32 OE      :1; // IEEE Overflow exception enable
        U32 VE      :1; // Invalid operation exception enable
        U32 VXCVI   :1; // Invalid operation exception for invalid integer convert
        U32 VXSQRT  :1; // Invalid operation exception for invalid square root
        U32 VXSOFT  :1; // Invalid operation exception for software request
        U32         :1; // Reserved
        U32 FPRF    :5; // Result flags
        U32 FI      :1; // Fraction inexact
        U32 FR      :1; // Fraction rounded
        U32 VXVC    :1; // Invalid operation exception for invalid compare
        U32 VXIMZ   :1; // Invalid operation exception for Inf * 0
        U32 VXZDZ   :1; // Invalid operation exception for 0 / 0
        U32 VXIDI   :1; // Invalid operation exception for Inf + Inf
        U32 VXISI   :1; // Invalid operation exception for Inf - Inf
        U32 VXSNAN  :1; // Invalid operation exception for SNaN
        U32 XX      :1; // Inexact exception
        U32 ZX      :1; // Zero divide exception
        U32 UX      :1; // Underflow exception
        U32 OX      :1; // Overflow exception
        U32 VX      :1; // Invalid operation exception summary
        U32 FEX     :1; // Enabled exception summary
        U32 FX      :1; // Exception summary
    };

    void setException(const U32 mask) {
        if ((FPSCR & mask) != mask) {
            FX = 1;
        }
        FPSCR |= mask;
    }
};

// LR Register (SPR 8)
typedef U64 PPU_LR;

// CTR Register (SPR 9)
typedef U64 PPU_CTR;

union PPU_TB {
    U64 TB;

    struct {
        U32 TBL;
        U32 TBU;
    };
};

// Vector Register
union PPU_VR {
    U128 u128;
    U64 u64[2];
    S64 s64[2];
    U32 u32[4];
    S32 s32[4];
    U16 u16[8];
    S16 s16[8];
    U8 u8[16];
    U8 s8[16];
    F32 f32[4];
    F64 f64[2];

    inline void clear() {
        u64[0] = 0;
        u64[1] = 0;
    }
};

// Vector Status and Control Register
union PPU_VSCR {
    U32 VSCR;

    struct {
        U32 SAT : 1;  // Saturation
        U32     : 15; // Reserved
        U32 NJ  : 1;  // Non-Java
        U32     : 15; // Reserved
    };
};

struct State : public cpu::State {
    // UISA Registers
    U64 r[32];      // General-Purpose Registers
    F64 f[32];      // Floating-Point Register
    PPU_CR cr;
    PPU_FPSCR fpscr;

    /**
     * XER register
     * ============
     * The fixed-point exception register (XER, SPR1) is 64-bit, user-level
     * register consisting of following fields, ordered from MSb to LSb:
     *
     *  +----------------+--+--+--+----------------+------------+
     *  |   (reserved)   |SO|OV|CA|   (reserved)   | Byte count |
     *  +----------------+--+--+--+----------------+------------+
     *   0             31 32 33 34 35            56 57        63
     */
    struct XER {
        U8 so;  // Summary overflow (1-bit)
        U8 ov;  // Overflow (1-bit)
        U8 ca;  // Carry (1-bit)
        U8 bc;  // Byte count (7-bits)
    } xer;

    PPU_LR lr;
    PPU_CTR ctr;

    // Vector/SIMD Registers
    V128 v[32];     // Vector Register
    PPU_VSCR vscr;

    // VEA Registers
    PPU_TB tb;

    // Reservation Registers
    U64 reserve_addr;
    U64 reserve_value;

    // Program Counter
    U32 pc;
};

#ifdef NUCLEUS_ARCH_X86
static_assert(offsetof(State, v) % 16 == 0, "PPU vector registers have to be 16-bit aligned in x86 hosts");
#endif

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
