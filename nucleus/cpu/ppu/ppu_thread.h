/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/thread.h"
#include <string>

// FPSCR Rounding control field values
enum FPSCR_RN {
    FPSCR_RN_NEAR = 0,
    FPSCR_RN_ZERO = 1,
    FPSCR_RN_PINF = 2,
    FPSCR_RN_MINF = 3,
};

/**
 * PPU Registers
 */

// General-Purpose Register
typedef u64 PPU_GPR;

// Floating-Point Register
typedef f64 PPU_FPR;

// Condition Register
union PPU_CR
{
    u32 CR;

    // Bit index
    enum {
        CR_LT = 3,
        CR_GT = 2,
        CR_EQ = 1,
        CR_SO = 0,
    };

    u8 getBit(u32 bit) { return (CR >> bit) & 1; }
    void setBit(u32 bit, bool value) { CR = value ? CR | (1 << bit) : CR & ~(1 << bit); }

    u8 getField(u32 field) { return (CR >> field*4) & 0xf; }
    void setField(u32 field, u8 value) { CR = CR & ((~0xf | value) << field*4); }

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

class PPUThread : public CellThread
{
    u32 m_stackAddr;
    u32 m_stackPointer;

public:
    // PPU UISA Registers
    PPU_GPR gpr[32];
    PPU_FPR fpr[32];
    PPU_CR cr;
    PPU_FPSCR fpscr;
    PPU_XER xer;
    PPU_LR lr;
    PPU_CTR ctr;
    
    // PPU VEA Registers
    u32 tbl;
    u32 tbu;

    // PPU Vector Registers
    PPU_VR vr[32];
    PPU_VSCR vscr;
    
    // PPU Reservation Registers
    u64 reserve_addr;
    u64 reserve_value;


    PPUThread();
    ~PPUThread();

    void reset();
    void setPC(u32 addr);
};
