/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/thread.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

#include <cmath>
#include <string>

namespace cpu {
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
    FPSCR_FX        = 0x80000000,
    FPSCR_FEX       = 0x40000000,
    FPSCR_VX        = 0x20000000,
    FPSCR_OX        = 0x10000000,
    FPSCR_UX        = 0x08000000,
    FPSCR_ZX        = 0x04000000,
    FPSCR_XX        = 0x02000000,
    FPSCR_VXSNAN    = 0x01000000,
    FPSCR_VXISI     = 0x00800000,
    FPSCR_VXIDI     = 0x00400000,
    FPSCR_VXZDZ     = 0x00200000,
    FPSCR_VXIMZ     = 0x00100000,
    FPSCR_VXVC      = 0x00080000,
    FPSCR_FR        = 0x00040000,
    FPSCR_FI        = 0x00020000,
    FPSCR_VXSOFT    = 0x00000400,
    FPSCR_VXSQRT    = 0x00000200,
    FPSCR_VXCVI     = 0x00000100,
};

/**
 * PPU Registers
 */

// General-Purpose Register
typedef u64 PPU_GPR;

// Condition Register
union PPU_CR
{
    u32 CR;

    // Bit index
    enum {
        CR_LT = 0,
        CR_GT = 1,
        CR_EQ = 2,
        CR_SO = 3,
    };

    u8 getBit(u32 bit) { return (CR >> bit) & 1; }
    void setBit(u32 bit, bool value) { CR = value ? CR | (1 << bit) : CR & ~(1 << bit); }

    u8 getField(u32 field) { return (CR >> field*4) & 0xf; }
    void setField(u32 field, u8 value) {
        u64 maskHigh = ~((1ULL << (field+1)*4)-1);
        u64 maskLow = ((1ULL << field*4)-1);
        CR = (CR & (u32)maskHigh) | (value << field*4) | (CR & (u32)maskLow);
    }

    template<typename T>
    void updateField(u32 field, T a, T b) {
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
union PPU_FPR
{
    f64 _f64;
    u64 _u64;

    enum : u64 {
        FPR_NAN = 0x7FF8000000000000ULL,
    };

    bool operator== (f64 right) { return _f64 == right; }
    bool operator== (u64 right) { return _u64 == right; }

    bool isInf() {
        return (_u64 & 0x7FFFFFFFFFFFFFFFULL) == 0x7FF0000000000000ULL;
    }

    bool isNaN() {
        return std::isnan(_f64);
    }

    bool isSNaN() {
        return
            (_u64 & 0x7FF0000000000000ULL) == 0x7FF0000000000000ULL &&
            (_u64 & 0x000FFFFFFFFFFFFFULL) != 0ULL &&
            (_u64 & 0x0008000000000000ULL) == 0ULL;
    }

    static int compare(PPU_FPR& a, PPU_FPR& b) {
        if(a._f64  < b._f64) return PPU_CR::CR_LT;
        if(a._f64  > b._f64) return PPU_CR::CR_GT;
        if(a._f64 == b._f64) return PPU_CR::CR_EQ;
        return PPU_CR::CR_SO;
    }
};

// Floating-Point Status and Control Register
union PPU_FPSCR
{
    u32 FPSCR;

    struct {
        u32 RN      :2; // Rounding control
        u32 NI      :1; // Non-IEEE mode
        u32 XE      :1; // Inexact exception enable
        u32 ZE      :1; // IEEE Zero divide exception enable
        u32 UE      :1; // IEEE Underflow exception enable
        u32 OE      :1; // IEEE Overflow exception enable
        u32 VE      :1; // Invalid operation exception enable
        u32 VXCVI   :1; // Invalid operation exception for invalid integer convert
        u32 VXSQRT  :1; // Invalid operation exception for invalid square root
        u32 VXSOFT  :1; // Invalid operation exception for software request
        u32         :1;
        u32 FPRF    :5; // Result flags
        u32 FI      :1; // Fraction inexact
        u32 FR      :1; // Fraction rounded
        u32 VXVC    :1; // Invalid operation exception for invalid compare
        u32 VXIMZ   :1; // Invalid operation exception for Inf * 0
        u32 VXZDZ   :1; // Invalid operation exception for 0 / 0
        u32 VXIDI   :1; // Invalid operation exception for Inf + Inf
        u32 VXISI   :1; // Invalid operation exception for Inf - Inf
        u32 VXSNAN  :1; // Invalid operation exception for SNaN
        u32 XX      :1; // Inexact exception
        u32 ZX      :1; // Zero divide exception
        u32 UX      :1; // Underflow exception
        u32 OX      :1; // Overflow exception
        u32 VX      :1; // Invalid operation exception summary
        u32 FEX     :1; // Enabled exception summary
        u32 FX      :1; // Exception summary
    };

    void setException(const u32 mask) {
        if ((FPSCR & mask) != mask) {
            FX = 1;
        }
        FPSCR |= mask;
    }
};

// XER Register (SPR 1)
union PPU_XER
{
    u64 XER;
    struct {
        u32 BC : 7;  // Byte count
        u32    : 22;
        u32 CA : 1;  // Carry
        u32 OV : 1;  // Overflow
        u32 SO : 1;  // Summary overflow
        u32    : 32;
    };
};

// LR Register (SPR 8)
typedef u64 PPU_LR;

// CTR Register (SPR 9)
typedef u64 PPU_CTR;

union PPU_TB {
    u64 TB;
    struct {
        u32 TBL;
        u32 TBU;
    };
};

// Vector Register
union PPU_VR
{
    u128 _u128;
    u64 _u64[2];
    s64 _s64[2];
    u32 _u32[4];
    s32 _s32[4];
    u16 _u16[8];
    s16 _s16[8];
    u8 _u8[16];
    s8 _s8[16];
    f32 _f32[4];
    f64 _f64[2];

    inline void clear()
    {
        _u64[0] = 0;
        _u64[1] = 0;
    }
};

// Vector Status and Control Register
union PPU_VSCR
{
    u32 VSCR;

    struct {
        u32 SAT : 1;  // Saturation
        u32     : 15;
        u32 NJ  : 1;  // Non-Java
        u32     : 15;
    };
};

struct State {
    // UISA Registers
    PPU_GPR gpr[32] = {};
    PPU_FPR fpr[32] = {};
    PPU_CR cr = {};
    PPU_FPSCR fpscr = {};
    PPU_XER xer = {};
    PPU_LR lr = 0;
    PPU_CTR ctr = 0;

    // VEA Registers
    PPU_TB tb = {};

    // Vector/SIMD Registers
    PPU_VR vr[32] = {};
    PPU_VSCR vscr = {};

    // Reservation Registers
    u64 reserve_addr = 0;
    u64 reserve_value = 0;

    // Program Counter
    u32 pc;

    /**
     * Recompiler utilities
     */

    // Get the LLVM type of this class
    static llvm::StructType* type();

    static void declareGlobalState(llvm::Module* module);
    static llvm::Value* readGPR(llvm::IRBuilder<>& builder, int index);
};

}  // namespace ppu
}  // namespace cpu
