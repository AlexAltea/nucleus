/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_interpreter.h"

namespace cpu {
namespace ppu {

#define DOUBLE_SIGN 0x8000000000000000ULL
#define DOUBLE_EXP  0x7FF0000000000000ULL
#define DOUBLE_FRAC 0x000FFFFFFFFFFFFFULL
#define DOUBLE_ZERO 0x0000000000000000ULL

// Return the corresponding FPSCR flags for the FPRF field for a given double value
u32 getFPRFlags(PPU_FPR reg)
{
    const f64 value = reg._f64;
    switch (std::fpclassify(value)) {
    case FP_NAN:        return FPR_FPRF_QNAN;
    case FP_INFINITE:   return std::signbit(value) ? FPR_FPRF_NINF : FPR_FPRF_PINF;
    case FP_SUBNORMAL:  return std::signbit(value) ? FPR_FPRF_ND : FPR_FPRF_PD;
    case FP_ZERO:       return std::signbit(value) ? FPR_FPRF_NZ : FPR_FPRF_PZ;
    default:            return std::signbit(value) ? FPR_FPRF_NN : FPR_FPRF_PN;
    }
}

/**
 * PPC64 Instructions:
 *  - UISA: Floating-Point Instructions (Section: 4.2.2)
 */

void Interpreter::fabsx(Instruction code)
{
    const f64 value = state.fpr[code.frb]._f64;
    state.fpr[code.frd]._f64 = (value < 0.0) ? -value : value;
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::faddx(Instruction code)
{
    const f64 a = state.fpr[code.fra]._f64;
    const f64 b = state.fpr[code.frb]._f64;
    const f64 t = a + b;
    if (a != a) {
        state.fpr[code.frd]._f64 = a;
    } else if (b != b) {
        state.fpr[code.frd]._f64 = b;
    } else if (t != t) {
        state.fpscr.setException(FPSCR_VXISI);
        (u64&)state.fpr[code.frd] = 0x7FF8000000000000ULL;
    } else {
        state.fpr[code.frd]._f64 = t;
    }
    state.fpscr.FPRF = getFPRFlags(state.fpr[code.frd]);
    if (code.rc) { state.cr.setField(1, state.fpscr.FPRF >> 28); }
}

void Interpreter::faddsx(Instruction code)
{
    const f64 a = state.fpr[code.fra]._f64;
    const f64 b = state.fpr[code.frb]._f64;
    const f64 t = a + b;
    if (a != a) {
        state.fpr[code.frd]._f64 = a;
    } else if (b != b) {
        state.fpr[code.frd]._f64 = b;
    } else if (t != t) {
        state.fpscr.setException(FPSCR_VXISI);
        (u64&)state.fpr[code.frd] = 0x7FF8000000000000ULL;
    } else {
        state.fpr[code.frd]._f64 = t;
    }
    state.fpr[code.frd]._f64 = static_cast<f32>(state.fpr[code.frd]._f64);
    state.fpscr.FPRF = getFPRFlags(state.fpr[code.frd]);
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fcfidx(Instruction code)
{
    const s64 bi = (s64&)state.fpr[code.frb];
    const f64 bf = (f64)bi;
    const s64 bfi = (s64)bf;
    if (bi == bfi) {
        state.fpscr.FI = 0;
        state.fpscr.FR = 0;
    } else {
        state.fpscr.FI = 1;
        state.fpscr.setException(FPSCR_XX);
        state.fpscr.FR = abs(bfi) > abs(bi);
    }
    state.fpr[code.frd]._f64 = bf;
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fcmpo(Instruction code)
{
    const int compareResult = PPU_FPR::compare(state.fpr[code.fra], state.fpr[code.frb]);
    if (compareResult == PPU_CR::CR_SO) {
        if (state.fpr[code.fra].isSNaN() || state.fpr[code.frb].isSNaN()) {
            state.fpscr.setException(FPSCR_VXSNAN);
            if (!state.fpscr.VE) {
                state.fpscr.setException(FPSCR_VXVC);
            }
        }
        else {
            state.fpscr.setException(FPSCR_VXVC);
        }
        state.fpscr.FX = 1;
    }
    state.fpscr.FPRF = (1 << (3 - compareResult));
    state.cr.setField(code.crfd, 1 << compareResult);
}

void Interpreter::fcmpu(Instruction code)
{
    const int compareResult = PPU_FPR::compare(state.fpr[code.fra], state.fpr[code.frb]);
    if (compareResult == PPU_CR::CR_SO) {
        if (std::isnan(state.fpr[code.fra]._f64) || std::isnan(state.fpr[code.frb]._f64)) {
            state.fpscr.setException(FPSCR_VXSNAN);
        }
    }
    state.fpscr.FPRF = (1 << (3 - compareResult));
    state.cr.setField(code.crfd, 1 << compareResult);
}

void Interpreter::fctidx(Instruction code)
{
    const f64 b = state.fpr[code.frb]._f64;
    u64 r;
    if (b > (f64)0x7FFFFFFFFFFFFFFF) {
        r = 0x7FFFFFFFFFFFFFFF;
        if (state.fpscr.VXCVI) {
            state.fpscr.FX = 1;
        }
        state.fpscr.VXCVI = 1;
        state.fpscr.FI = 0;
        state.fpscr.FR = 0;
    }
    else if (b < -(f64)0x8000000000000000) {
        r = 0x8000000000000000;
        if (state.fpscr.VXCVI) {
            state.fpscr.FX = 1;
        }
        state.fpscr.VXCVI = 1;
        state.fpscr.FI = 0;
        state.fpscr.FR = 0;
    }
    else {
        s64 i = 0;
        switch (state.fpscr.RN) {
        case FPSCR_RN_NEAR:
            {
                f64 t = b + 0.5;
                i = (s64)t;
                if (t - i < 0 || (t - i == 0 && b > 0)) i--;
                break;
            }
        case FPSCR_RN_ZERO:
            i = (s64)b;
            break;
        case FPSCR_RN_PINF:
            i = (s64)b;
            if (b - i > 0) i++;
            break;
        case FPSCR_RN_MINF:
            i = (s64)b;
            if (b - i < 0) i--;
            break;
        }
        r = (u64)i;
        f64 di = i;
        if (di == b) {
            state.fpscr.FI = 0;
            state.fpscr.FR = 0;
        }
        else {
            state.fpscr.FI = 1;
            state.fpscr.FR = fabs(di) > fabs(b);
        }
    }

    state.fpr[code.frd]._u64 = r;
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fctidzx(Instruction code)
{
    const f64 b = state.fpr[code.frb]._f64;
    u64 r;
    if (b > (f64)0x7FFFFFFFFFFFFFFF) {
        r = 0x7FFFFFFFFFFFFFFF;
        if (state.fpscr.VXCVI) {
            state.fpscr.FX = 1;
        }
        state.fpscr.VXCVI = 1;
        state.fpscr.FI = 0;
        state.fpscr.FR = 0;
    }
    else if (b < -(f64)0x8000000000000000) {
        r = 0x8000000000000000;
        if (state.fpscr.VXCVI) {
            state.fpscr.FX = 1;
        }
        state.fpscr.VXCVI = 1;
        state.fpscr.FI = 0;
        state.fpscr.FR = 0;
    }
    else {
        s64 i = (s64)b;
        f64 di = i;
        if (di == b) {
            state.fpscr.FI = 0;
            state.fpscr.FR = 0;
        }
        else {
            state.fpscr.FI = 1;
            state.fpscr.FR = fabs(di) > fabs(b);
        }
        r = (u64)i;
    }

    state.fpr[code.frd]._u64 = r;
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fctiwx(Instruction code)
{
    const f64 b = state.fpr[code.frb]._f64;
    u32 r;
    if (b > (f64)0x7FFFFFFF) {
        r = 0x7FFFFFFF;
        if (state.fpscr.VXCVI) {
            state.fpscr.FX = 1;
        }
        state.fpscr.VXCVI = 1;
        state.fpscr.FI = 0;
        state.fpscr.FR = 0;
    }
    else if (b < -(f64)0x80000000) {
        r = 0x80000000;
        if (state.fpscr.VXCVI) {
            state.fpscr.FX = 1;
        }
        state.fpscr.VXCVI = 1;
        state.fpscr.FI = 0;
        state.fpscr.FR = 0;
    }
    else {
        s32 i = 0;
        switch (state.fpscr.RN)
        {
        case FPSCR_RN_NEAR:
            {
                f64 t = b + 0.5;
                i = (s32)t;
                if (t - i < 0 || (t - i == 0 && b > 0)) i--;
                break;
            }
        case FPSCR_RN_ZERO:
            i = (s32)b;
            break;
        case FPSCR_RN_PINF:
            i = (s32)b;
            if (b - i > 0) i++;
            break;
        case FPSCR_RN_MINF:
            i = (s32)b;
            if (b - i < 0) i--;
            break;
        }
        r = (u32)i;
        f64 di = i;
        if (di == b) {
            state.fpscr.FI = 0;
            state.fpscr.FR = 0;
        }
        else {
            state.fpscr.FI = 1;
            state.fpscr.FR = fabs(di) > fabs(b);
        }
    }

    (u64&)state.fpr[code.frd] = r;
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fctiwzx(Instruction code)
{
    const f64 b = state.fpr[code.frb]._f64;
    u32 value;
    if (b > (f64)0x7FFFFFFF) {
        value = 0x7FFFFFFF;
        if (state.fpscr.VXCVI) state.fpscr.FX = 1;
        state.fpscr.VXCVI = 1;
        state.fpscr.FI = 0;
        state.fpscr.FR = 0;
    }
    else if (b < -(f64)0x80000000) {
        value = 0x80000000;
        if (state.fpscr.VXCVI) state.fpscr.FX = 1;
        state.fpscr.VXCVI = 1;
        state.fpscr.FI = 0;
        state.fpscr.FR = 0;
    }
    else {
        s32 i = (s32)b;
        f64 di = i;
        if (di == b) {
            state.fpscr.FI = 0;
            state.fpscr.FR = 0;
        }
        else {
            state.fpscr.FI = 1;
            state.fpscr.FR = fabs(di) > fabs(b);
        }
        value = (u32)i;
    }

    (u64&)state.fpr[code.frd] = value;
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fdivx(Instruction code)
{
    if (std::isnan(state.fpr[code.fra]._f64)) {
        state.fpr[code.frd] = state.fpr[code.fra];
    }
    else if (std::isnan(state.fpr[code.frb]._f64)) {
        state.fpr[code.frd] = state.fpr[code.frb];
    }
    else {
        if (state.fpr[code.frb] == 0.0) {
            if (state.fpr[code.fra] == 0.0) {
                state.fpscr.VXZDZ = 1;
                state.fpr[code.frd]._u64 = PPU_FPR::FPR_NAN;
            }
            else {
                state.fpr[code.frd]._f64 = state.fpr[code.fra]._f64 / state.fpr[code.frb]._f64;
            }
            state.fpscr.setException(FPSCR_ZX);
        }
        else if (state.fpr[code.fra].isInf() && state.fpr[code.frb].isInf()) {
            state.fpscr.VXIDI = 1;
            state.fpr[code.frd]._u64 = PPU_FPR::FPR_NAN;
        }
        else {
            state.fpr[code.frd]._f64 = state.fpr[code.fra]._f64 / state.fpr[code.frb]._f64;
        }
    }
    state.fpscr.FPRF = getFPRFlags(state.fpr[code.frd]);
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fdivsx(Instruction code)
{
    if (std::isnan(state.fpr[code.fra]._f64)) {
        state.fpr[code.frd] = state.fpr[code.fra];
    }
    else if (std::isnan(state.fpr[code.frb]._f64)) {
        state.fpr[code.frd] = state.fpr[code.frb];
    }
    else {
        if (state.fpr[code.frb] == 0.0) {
            if (state.fpr[code.fra] == 0.0) {
                state.fpscr.VXZDZ = true;
                state.fpr[code.frd]._u64 = PPU_FPR::FPR_NAN;
            }
            else {
                state.fpr[code.frd]._f64 = (f32)(state.fpr[code.fra]._f64 / state.fpr[code.frb]._f64);
            }

            state.fpscr.ZX = true;
        }
        else if (state.fpr[code.fra].isInf() && state.fpr[code.frb].isInf()) {
            state.fpscr.VXIDI = true;
            state.fpr[code.frd]._u64 = PPU_FPR::FPR_NAN;
        }
        else {
            state.fpr[code.frd]._f64 = (f32)(state.fpr[code.fra]._f64 / state.fpr[code.frb]._f64);
        }
    }
    state.fpscr.FPRF = getFPRFlags(state.fpr[code.frd]);
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fmaddx(Instruction code)
{
    state.fpr[code.frd]._f64 = state.fpr[code.fra]._f64 * state.fpr[code.frc]._f64 + state.fpr[code.frb]._f64;
    state.fpscr.FPRF = getFPRFlags(state.fpr[code.frd]);
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fmaddsx(Instruction code)
{
    state.fpr[code.frd]._f64 = static_cast<f32>(state.fpr[code.fra]._f64 * state.fpr[code.frc]._f64 + state.fpr[code.frb]._f64);
    state.fpscr.FPRF = getFPRFlags(state.fpr[code.frd]);
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fmrx(Instruction code)
{
    state.fpr[code.frd] = state.fpr[code.frb];
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fmsubx(Instruction code)
{
    state.fpr[code.frd]._f64 = state.fpr[code.fra]._f64 * state.fpr[code.frc]._f64 - state.fpr[code.frb]._f64;
    state.fpscr.FPRF = getFPRFlags(state.fpr[code.frd]);
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fmsubsx(Instruction code)
{
    state.fpr[code.frd]._f64 = static_cast<f32>(state.fpr[code.fra]._f64 * state.fpr[code.frc]._f64 - state.fpr[code.frb]._f64);
    state.fpscr.FPRF = getFPRFlags(state.fpr[code.frd]);
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fmulx(Instruction code)
{
    if ((state.fpr[code.fra].isInf() && state.fpr[code.frc] == 0.0) ||
        (state.fpr[code.frc].isInf() && state.fpr[code.fra] == 0.0)) {
        state.fpscr.setException(FPSCR_VXIMZ);
        state.fpr[code.frd]._u64 = PPU_FPR::FPR_NAN;
        state.fpscr.FI = 0;
        state.fpscr.FR = 0;
        state.fpscr.FPRF = FPSCR_FPRF::FPR_FPRF_QNAN;
    }
    else {
        if (state.fpr[code.fra].isSNaN() || state.fpr[code.frc].isSNaN()) {
            state.fpscr.setException(FPSCR_VXSNAN);
        }
        state.fpr[code.frd]._f64 = state.fpr[code.fra]._f64 * state.fpr[code.frc]._f64;
        state.fpscr.FPRF = getFPRFlags(state.fpr[code.frd]);
    }
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fmulsx(Instruction code)
{
    const f64 a = state.fpr[code.fra]._f64;
    const f64 b = state.fpr[code.frc]._f64;
    const f32 t = a * b;
    if (a != a) {
        state.fpr[code.frd]._f64 = a;
    } else if (b != b) {
        state.fpr[code.frd]._f64 = b;
    } else if (t != t) {
        state.fpscr.setException(FPSCR_VXISI);
        (u64&)state.fpr[code.frd] = 0x7FF8000000000000ULL;
    } else {
        state.fpr[code.frd]._f64 = t;
    }
    state.fpscr.FI = 0;
    state.fpscr.FR = 0;
    state.fpscr.FPRF = getFPRFlags(state.fpr[code.frd]);
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fnabsx(Instruction code)
{
    state.fpr[code.frd]._f64 = -::fabs(state.fpr[code.frb]._f64);
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fnegx(Instruction code)
{
    state.fpr[code.frd]._f64 = -state.fpr[code.frb]._f64;
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fnmaddx(Instruction code)
{
    state.fpr[code.frd]._f64 = -(state.fpr[code.fra]._f64 * state.fpr[code.frc]._f64 + state.fpr[code.frb]._f64);
    state.fpscr.FPRF = getFPRFlags(state.fpr[code.frd]);
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fnmaddsx(Instruction code)
{
    state.fpr[code.frd]._f64 = static_cast<f32>(-(state.fpr[code.fra]._f64 * state.fpr[code.frc]._f64 + state.fpr[code.frb]._f64));
    state.fpscr.FPRF = getFPRFlags(state.fpr[code.frd]);
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fnmsubx(Instruction code)
{
    state.fpr[code.frd]._f64 = -(state.fpr[code.fra]._f64 * state.fpr[code.frc]._f64 - state.fpr[code.frb]._f64);
    state.fpscr.FPRF = getFPRFlags(state.fpr[code.frd]);
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fnmsubsx(Instruction code)
{
    state.fpr[code.frd]._f64 = static_cast<f32>(-(state.fpr[code.fra]._f64 * state.fpr[code.frc]._f64 - state.fpr[code.frb]._f64));
    state.fpscr.FPRF = getFPRFlags(state.fpr[code.frd]);
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fresx(Instruction code)
{
    if (state.fpr[code.frb] == 0.0) {
        state.fpscr.setException(FPSCR_ZX);
    }
    state.fpr[code.frd]._f64 = static_cast<f32>(1.0 / state.fpr[code.frb]._f64);
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::frspx(Instruction code)
{
    const f64 b = state.fpr[code.frb]._f64;
    f64 b0 = b;
    if (state.fpscr.NI) {
        if (((u64&)b0 & DOUBLE_EXP) < 0x3800000000000000ULL) {
            (u64&)b0 &= DOUBLE_SIGN;
        }
    }
    const f64 r = static_cast<f32>(b0);
    state.fpscr.FR = fabs(r) > fabs(b);
    if (b != r) {
        state.fpscr.FI = 1;
        state.fpscr.setException(FPSCR_XX);
    } else {
        state.fpscr.FI = 0;
    }
    state.fpr[code.frd]._f64 = r;
    state.fpscr.FPRF = getFPRFlags(state.fpr[code.frd]);
}

void Interpreter::frsqrtex(Instruction code)
{
    if (state.fpr[code.frb] == 0.0) {
        state.fpscr.setException(FPSCR_ZX);
    }
    state.fpr[code.frd]._f64 = 1.0 / sqrt(state.fpr[code.frb]._f64);
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fselx(Instruction code)
{
    state.fpr[code.frd] = state.fpr[code.fra]._f64 >= 0.0 ? state.fpr[code.frc] : state.fpr[code.frb];
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fsqrtx(Instruction code)
{
    state.fpr[code.frd]._f64 = sqrt(state.fpr[code.frb]._f64);
    state.fpscr.FPRF = getFPRFlags(state.fpr[code.frd]);
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fsqrtsx(Instruction code)
{
    state.fpr[code.frd]._f64 = static_cast<f32>(sqrt(state.fpr[code.frb]._f64));
    state.fpscr.FPRF = getFPRFlags(state.fpr[code.frd]);
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fsubx(Instruction code)
{
    state.fpr[code.frd]._f64 = state.fpr[code.fra]._f64 - state.fpr[code.frb]._f64;
    state.fpscr.FPRF = getFPRFlags(state.fpr[code.frd]);
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::fsubsx(Instruction code)
{
    state.fpr[code.frd]._f64 = static_cast<f32>(state.fpr[code.fra]._f64 - state.fpr[code.frb]._f64);
    state.fpscr.FPRF = getFPRFlags(state.fpr[code.frd]);
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::mcrfs(Instruction code)
{
    u64 mask = (1ULL << code.crbd);
    state.cr.CR &= ~mask;
    state.cr.CR |= state.fpscr.FPSCR & mask;
}

void Interpreter::mffsx(Instruction code)
{
    (u64&)state.fpr[code.frd] = state.fpscr.FPSCR;
    if (code.rc) { state.cr.setField(1, state.fpscr.FPSCR >> 28); }
}

void Interpreter::mtfsb0x(Instruction code)
{
    state.fpscr.FPSCR &= ~(1ULL << code.crbd);
    if (code.rc) unknown("mtfsb0.");
}

void Interpreter::mtfsb1x(Instruction code)
{
    state.fpscr.FPSCR |= (1ULL << code.crbd);
    if (code.rc) unknown("mtfsb1.");
}

void Interpreter::mtfsfix(Instruction code)
{
    const u32 mask = 0xF << (code.crfd * 4);
    const u32 value = (code.imm & 0xF) << (code.crfd * 4);
    state.fpscr.FPSCR &= ~mask;
    state.fpscr.FPSCR |= value;
    if (code.rc) unknown("mtfsfi.");
}

void Interpreter::mtfsfx(Instruction code)
{
    u32 mask = 0;
    for (int i = 0; i < 8; i++) {
        if (code.fm & (1 << i)) {
            mask |= 0xf << (i * 4);
        }
    }
    state.fpscr.FPSCR = (state.fpscr.FPSCR & ~mask) | ((u32&)state.fpr[code.frb] & mask);
    if (code.rc) unknown("mtfsf.");
}

}  // namespace ppu
}  // namespace cpu
