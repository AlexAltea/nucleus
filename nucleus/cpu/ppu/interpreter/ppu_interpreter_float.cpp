/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_interpreter.h"

namespace cpu {
namespace ppu {

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

void Interpreter::fabsx(Instruction code, PPUThread& thread)
{
    const f64 value = thread.fpr[code.frb]._f64;
    thread.fpr[code.frd]._f64 = (value < 0.0) ? -value : value;
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::faddx(Instruction code, PPUThread& thread)
{
    const f64 a = thread.fpr[code.fra]._f64;
    const f64 b = thread.fpr[code.frb]._f64;
    const f64 t = a + b;
    if (a != a) {
        thread.fpr[code.frd]._f64 = a;
    } else if (b != b) {
        thread.fpr[code.frd]._f64 = b;
    } else if (t != t) {
        thread.fpscr.setException(FPSCR_VXISI);
        (u64&)thread.fpr[code.frd] = 0x7FF8000000000000ULL;
    } else {
        thread.fpr[code.frd]._f64 = t;
    }
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPRF >> 28); }
}

void Interpreter::faddsx(Instruction code, PPUThread& thread)
{
    const f64 a = thread.fpr[code.fra]._f64;
    const f64 b = thread.fpr[code.frb]._f64;
    const f64 t = a + b;
    if (a != a) {
        thread.fpr[code.frd]._f64 = a;
    } else if (b != b) {
        thread.fpr[code.frd]._f64 = b;
    } else if (t != t) {
        thread.fpscr.setException(FPSCR_VXISI);
        (u64&)thread.fpr[code.frd] = 0x7FF8000000000000ULL;
    } else {
        thread.fpr[code.frd]._f64 = t;
    }
    thread.fpr[code.frd]._f64 = static_cast<f32>(thread.fpr[code.frd]._f64);
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fcfidx(Instruction code, PPUThread& thread)
{
    const s64 bi = (s64&)thread.fpr[code.frb];
    const f64 bf = (f64)bi;
    const s64 bfi = (s64)bf;
    if (bi == bfi) {
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    } else {
        thread.fpscr.FI = 1;
        thread.fpscr.setException(FPSCR_XX);
        thread.fpscr.FR = abs(bfi) > abs(bi);
    }
    thread.fpr[code.frd]._f64 = bf;
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fcmpo(Instruction code, PPUThread& thread)
{
    const int compareResult = PPU_FPR::compare(thread.fpr[code.fra], thread.fpr[code.frb]);
    if (compareResult == PPU_CR::CR_SO) {
        if (thread.fpr[code.fra].isSNaN() || thread.fpr[code.frb].isSNaN()) {
            thread.fpscr.setException(FPSCR_VXSNAN);
            if (!thread.fpscr.VE) {
                thread.fpscr.setException(FPSCR_VXVC);
            }
        }
        else {
            thread.fpscr.setException(FPSCR_VXVC);
        }
        thread.fpscr.FX = 1;
    }
    thread.fpscr.FPRF = (1 << (3 - compareResult));
    thread.cr.setField(code.crfd, 1 << compareResult);
}

void Interpreter::fcmpu(Instruction code, PPUThread& thread)
{
    const int compareResult = PPU_FPR::compare(thread.fpr[code.fra], thread.fpr[code.frb]);
    if (compareResult == PPU_CR::CR_SO) {
        if (std::isnan(thread.fpr[code.fra]._f64) || std::isnan(thread.fpr[code.frb]._f64)) {
            thread.fpscr.setException(FPSCR_VXSNAN);
        }
    }
    thread.fpscr.FPRF = (1 << (3 - compareResult));
    thread.cr.setField(code.crfd, 1 << compareResult);
}

void Interpreter::fctidx(Instruction code, PPUThread& thread)
{
    const f64 b = thread.fpr[code.frb]._f64;
    u64 r;
    if (b > (f64)0x7FFFFFFFFFFFFFFF) {
        r = 0x7FFFFFFFFFFFFFFF;
        if (thread.fpscr.VXCVI) {
            thread.fpscr.FX = 1;
        }
        thread.fpscr.VXCVI = 1;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    }
    else if (b < -(f64)0x8000000000000000) {
        r = 0x8000000000000000;
        if (thread.fpscr.VXCVI) {
            thread.fpscr.FX = 1;
        }
        thread.fpscr.VXCVI = 1;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    }
    else {
        s64 i = 0;
        switch (thread.fpscr.RN) {
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
            thread.fpscr.FI = 0;
            thread.fpscr.FR = 0;
        }
        else {
            thread.fpscr.FI = 1;
            thread.fpscr.FR = fabs(di) > fabs(b);
        }
    }

    thread.fpr[code.frd]._u64 = r;
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fctidzx(Instruction code, PPUThread& thread)
{
    const f64 b = thread.fpr[code.frb]._f64;
    u64 r;
    if (b > (f64)0x7FFFFFFFFFFFFFFF) {
        r = 0x7FFFFFFFFFFFFFFF;
        if (thread.fpscr.VXCVI) {
            thread.fpscr.FX = 1;
        }
        thread.fpscr.VXCVI = 1;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    }
    else if (b < -(f64)0x8000000000000000) {
        r = 0x8000000000000000;
        if (thread.fpscr.VXCVI) {
            thread.fpscr.FX = 1;
        }
        thread.fpscr.VXCVI = 1;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    }
    else {
        s64 i = (s64)b;
        f64 di = i;
        if (di == b) {
            thread.fpscr.FI = 0;
            thread.fpscr.FR = 0;
        }
        else {
            thread.fpscr.FI = 1;
            thread.fpscr.FR = fabs(di) > fabs(b);
        }
        r = (u64)i;
    }

    thread.fpr[code.frd]._u64 = r;
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fctiwx(Instruction code, PPUThread& thread)
{
    const f64 b = thread.fpr[code.frb]._f64;
    u32 r;
    if (b > (f64)0x7FFFFFFF) {
        r = 0x7FFFFFFF;
        if (thread.fpscr.VXCVI) {
            thread.fpscr.FX = 1;
        }
        thread.fpscr.VXCVI = 1;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    }
    else if (b < -(f64)0x80000000) {
        r = 0x80000000;
        if (thread.fpscr.VXCVI) {
            thread.fpscr.FX = 1;
        }
        thread.fpscr.VXCVI = 1;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    }
    else {
        s32 i = 0;
        switch (thread.fpscr.RN)
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
            thread.fpscr.FI = 0;
            thread.fpscr.FR = 0;
        }
        else {
            thread.fpscr.FI = 1;
            thread.fpscr.FR = fabs(di) > fabs(b);
        }
    }

    (u64&)thread.fpr[code.frd] = r;
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fctiwzx(Instruction code, PPUThread& thread)
{
    const f64 b = thread.fpr[code.frb]._f64;
    u32 value;
    if (b > (f64)0x7FFFFFFF) {
        value = 0x7FFFFFFF;
        if (thread.fpscr.VXCVI) thread.fpscr.FX = 1;
        thread.fpscr.VXCVI = 1;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    }
    else if (b < -(f64)0x80000000) {
        value = 0x80000000;
        if (thread.fpscr.VXCVI) thread.fpscr.FX = 1;
        thread.fpscr.VXCVI = 1;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
    }
    else {
        s32 i = (s32)b;
        f64 di = i;
        if (di == b) {
            thread.fpscr.FI = 0;
            thread.fpscr.FR = 0;
        }
        else {
            thread.fpscr.FI = 1;
            thread.fpscr.FR = fabs(di) > fabs(b);
        }
        value = (u32)i;
    }

    (u64&)thread.fpr[code.frd] = value;
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fdivx(Instruction code, PPUThread& thread)
{
    if (std::isnan(thread.fpr[code.fra]._f64)) {
        thread.fpr[code.frd] = thread.fpr[code.fra];
    }
    else if (std::isnan(thread.fpr[code.frb]._f64)) {
        thread.fpr[code.frd] = thread.fpr[code.frb];
    }
    else {
        if (thread.fpr[code.frb] == 0.0) {
            if (thread.fpr[code.fra] == 0.0) {
                thread.fpscr.VXZDZ = 1;
                thread.fpr[code.frd]._u64 = PPU_FPR::FPR_NAN;
            }
            else {
                thread.fpr[code.frd]._f64 = thread.fpr[code.fra]._f64 / thread.fpr[code.frb]._f64;
            }
            thread.fpscr.setException(FPSCR_ZX);
        }
        else if (thread.fpr[code.fra].isInf() && thread.fpr[code.frb].isInf()) {
            thread.fpscr.VXIDI = 1;
            thread.fpr[code.frd]._u64 = PPU_FPR::FPR_NAN;
        }
        else {
            thread.fpr[code.frd]._f64 = thread.fpr[code.fra]._f64 / thread.fpr[code.frb]._f64;
        }
    }
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fdivsx(Instruction code, PPUThread& thread)
{
    if (std::isnan(thread.fpr[code.fra]._f64)) {
        thread.fpr[code.frd] = thread.fpr[code.fra];
    }
    else if (std::isnan(thread.fpr[code.frb]._f64)) {
        thread.fpr[code.frd] = thread.fpr[code.frb];
    }
    else {
        if (thread.fpr[code.frb] == 0.0) {
            if (thread.fpr[code.fra] == 0.0) {
                thread.fpscr.VXZDZ = true;
                thread.fpr[code.frd]._u64 = PPU_FPR::FPR_NAN;
            }
            else {
                thread.fpr[code.frd]._f64 = (f32)(thread.fpr[code.fra]._f64 / thread.fpr[code.frb]._f64);
            }

            thread.fpscr.ZX = true;
        }
        else if (thread.fpr[code.fra].isInf() && thread.fpr[code.frb].isInf()) {
            thread.fpscr.VXIDI = true;
            thread.fpr[code.frd]._u64 = PPU_FPR::FPR_NAN;
        }
        else {
            thread.fpr[code.frd]._f64 = (f32)(thread.fpr[code.fra]._f64 / thread.fpr[code.frb]._f64);
        }
    }
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fmaddx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = thread.fpr[code.fra]._f64 * thread.fpr[code.frc]._f64 + thread.fpr[code.frb]._f64;
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fmaddsx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = static_cast<f32>(thread.fpr[code.fra]._f64 * thread.fpr[code.frc]._f64 + thread.fpr[code.frb]._f64);
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fmrx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd] = thread.fpr[code.frb];
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fmsubx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = thread.fpr[code.fra]._f64 * thread.fpr[code.frc]._f64 - thread.fpr[code.frb]._f64;
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fmsubsx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = static_cast<f32>(thread.fpr[code.fra]._f64 * thread.fpr[code.frc]._f64 - thread.fpr[code.frb]._f64);
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fmulx(Instruction code, PPUThread& thread)
{
    if ((thread.fpr[code.fra].isInf() && thread.fpr[code.frc] == 0.0) ||
        (thread.fpr[code.frc].isInf() && thread.fpr[code.fra] == 0.0)) {
        thread.fpscr.setException(FPSCR_VXIMZ);
        thread.fpr[code.frd]._u64 = PPU_FPR::FPR_NAN;
        thread.fpscr.FI = 0;
        thread.fpscr.FR = 0;
        thread.fpscr.FPRF = FPSCR_FPRF::FPR_FPRF_QNAN;
    }
    else {
        if (thread.fpr[code.fra].isSNaN() || thread.fpr[code.frc].isSNaN()) {
            thread.fpscr.setException(FPSCR_VXSNAN);
        }
        thread.fpr[code.frd]._f64 = thread.fpr[code.fra]._f64 * thread.fpr[code.frc]._f64;
        thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    }
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fmulsx(Instruction code, PPUThread& thread)
{
    const f64 a = thread.fpr[code.fra]._f64;
    const f64 b = thread.fpr[code.frc]._f64;
    const f32 t = a * b;
    if (a != a) {
        thread.fpr[code.frd]._f64 = a;
    } else if (b != b) {
        thread.fpr[code.frd]._f64 = b;
    } else if (t != t) {
        thread.fpscr.setException(FPSCR_VXISI);
        (u64&)thread.fpr[code.frd] = 0x7FF8000000000000ULL;
    } else {
        thread.fpr[code.frd]._f64 = t;
    }
    thread.fpscr.FI = 0;
    thread.fpscr.FR = 0;
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fnabsx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = -::fabs(thread.fpr[code.frb]._f64);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fnegx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = -thread.fpr[code.frb]._f64;
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fnmaddx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = -(thread.fpr[code.fra]._f64 * thread.fpr[code.frc]._f64 + thread.fpr[code.frb]._f64);
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fnmaddsx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = static_cast<f32>(-(thread.fpr[code.fra]._f64 * thread.fpr[code.frc]._f64 + thread.fpr[code.frb]._f64));
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fnmsubx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = -(thread.fpr[code.fra]._f64 * thread.fpr[code.frc]._f64 - thread.fpr[code.frb]._f64);
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fnmsubsx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = static_cast<f32>(-(thread.fpr[code.fra]._f64 * thread.fpr[code.frc]._f64 - thread.fpr[code.frb]._f64));
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fresx(Instruction code, PPUThread& thread)
{
    if (thread.fpr[code.frb] == 0.0) {
        thread.fpscr.setException(FPSCR_ZX);
    }
    thread.fpr[code.frd]._f64 = static_cast<f32>(1.0 / thread.fpr[code.frb]._f64);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::frspx(Instruction code, PPUThread& thread)
{
    const f64 b = thread.fpr[code.frb]._f64;
    f64 b0 = b;
    if (thread.fpscr.NI) {
        if (((u64&)b0 & DOUBLE_EXP) < 0x3800000000000000ULL) {
            (u64&)b0 &= DOUBLE_SIGN;
        }
    }
    const f64 r = static_cast<f32>(b0);
    thread.fpscr.FR = fabs(r) > fabs(b);
    if (b != r) {
        thread.fpscr.FI = 1;
        thread.fpscr.setException(FPSCR_XX);
    } else {
        thread.fpscr.FI = 0;
    }
    thread.fpr[code.frd]._f64 = r;
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
}

void Interpreter::frsqrtex(Instruction code, PPUThread& thread)
{
    if (thread.fpr[code.frb] == 0.0) {
        thread.fpscr.setException(FPSCR_ZX);
    }
    thread.fpr[code.frd]._f64 = 1.0 / sqrt(thread.fpr[code.frb]._f64);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fselx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd] = thread.fpr[code.fra]._f64 >= 0.0 ? thread.fpr[code.frc] : thread.fpr[code.frb];
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fsqrtx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = sqrt(thread.fpr[code.frb]._f64);
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fsqrtsx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = static_cast<f32>(sqrt(thread.fpr[code.frb]._f64));
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fsubx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = thread.fpr[code.fra]._f64 - thread.fpr[code.frb]._f64;
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::fsubsx(Instruction code, PPUThread& thread)
{
    thread.fpr[code.frd]._f64 = static_cast<f32>(thread.fpr[code.fra]._f64 - thread.fpr[code.frb]._f64);
    thread.fpscr.FPRF = getFPRFlags(thread.fpr[code.frd]);
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::mcrfs(Instruction code, PPUThread& thread)
{
    u64 mask = (1ULL << code.crbd);
    thread.cr.CR &= ~mask;
    thread.cr.CR |= thread.fpscr.FPSCR & mask;
}

void Interpreter::mffsx(Instruction code, PPUThread& thread)
{
    (u64&)thread.fpr[code.frd] = thread.fpscr.FPSCR;
    if (code.rc) { thread.cr.setField(1, thread.fpscr.FPSCR >> 28); }
}

void Interpreter::mtfsb0x(Instruction code, PPUThread& thread)
{
    thread.fpscr.FPSCR &= ~(1ULL << code.crbd);
    if (code.rc) unknown("mtfsb0.");
}

void Interpreter::mtfsb1x(Instruction code, PPUThread& thread)
{
    thread.fpscr.FPSCR |= (1ULL << code.crbd);
    if (code.rc) unknown("mtfsb1.");
}

void Interpreter::mtfsfix(Instruction code, PPUThread& thread)
{
    const u32 mask = 0xF << (code.crfd * 4);
    const u32 value = (code.imm & 0xF) << (code.crfd * 4);
    thread.fpscr.FPSCR &= ~mask;
    thread.fpscr.FPSCR |= value;
    if (code.rc) unknown("mtfsfi.");
}

void Interpreter::mtfsfx(Instruction code, PPUThread& thread)
{
    u32 mask = 0;
    for (int i = 0; i < 8; i++) {
        if (code.fm & (1 << i)) {
            mask |= 0xf << (i * 4);
        }
    }
    thread.fpscr.FPSCR = (thread.fpscr.FPSCR & ~mask) | ((u32&)thread.fpr[code.frb] & mask);
    if (code.rc) unknown("mtfsf.");
}

}  // namespace ppu
}  // namespace cpu
