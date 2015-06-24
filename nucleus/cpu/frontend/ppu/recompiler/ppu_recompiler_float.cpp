/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"

namespace cpu {
namespace ppu {

using namespace cpu::hir;

/**
 * PPC64 Instructions:
 *  - UISA: Floating-Point Instructions (Section: 4.2.2)
 */

void Recompiler::fabsx(Instruction code)
{
    Value<F64> frb = getFPR(code.frb);
    Value<F64> frd;

    frd = builder.CreateIntrinsic_Fabs(frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::faddx(Instruction code)
{
    Value<F64> fra = getFPR(code.fra);
    Value<F64> frb = getFPR(code.frb);
    Value<F64> frd;

    frd = builder.CreateFAdd(fra, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::faddsx(Instruction code)
{
    Value<F64> fra = getFPR(code.fra);
    Value<F64> frb = getFPR(code.frb);

    auto result = builder.CreateFAdd(fra, frb);
    auto frd = builder.CreateFPTrunc<F32>(result);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fcfidx(Instruction code)
{
}

void Recompiler::fcmpo(Instruction code)
{
}

void Recompiler::fcmpu(Instruction code)
{
}

void Recompiler::fctidx(Instruction code)
{
}

void Recompiler::fctidzx(Instruction code)
{
}

void Recompiler::fctiwx(Instruction code)
{
}

void Recompiler::fctiwzx(Instruction code)
{
}

void Recompiler::fdivx(Instruction code)
{
    Value<F64> fra = getFPR(code.fra);
    Value<F64> frb = getFPR(code.frb);
    Value<F64> frd;

    frd = builder.CreateFDiv(fra, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fdivsx(Instruction code)
{
    Value<F64> fra = getFPR(code.fra);
    Value<F64> frb = getFPR(code.frb);
    Value<F64> frd;

    frd = builder.CreateFDiv(fra, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.CreateFPTrunc<F32>(frd));
}

void Recompiler::fmaddx(Instruction code)
{
    Value<F64> fra = getFPR(code.fra);
    Value<F64> frc = getFPR(code.frc);
    Value<F64> frb = getFPR(code.frb);
    Value<F64> frd;

    frd = builder.CreateIntrinsic_Fmuladd(fra, frc, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fmaddsx(Instruction code)
{
    Value<F64> fra = getFPR(code.fra);
    Value<F64> frc = getFPR(code.frc);
    Value<F64> frb = getFPR(code.frb);
    Value<F64> frd;

    frd = builder.CreateIntrinsic_Fmuladd(fra, frc, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.CreateFPTrunc<F32>(frd));
}

void Recompiler::fmrx(Instruction code)
{
    Value<F64> frb = getFPR(code.frb);
    Value<F64> frd;

    frd = frb;
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fmsubx(Instruction code)
{
    Value<F64> fra = getFPR(code.fra);
    Value<F64> frc = getFPR(code.frc);
    Value<F64> frb = getFPR(code.frb);
    Value<F64> frd;

    // TEST: Is negating frb and calling llvm::Intrinsic::fmuladd faster?
    frd = builder.CreateFMul(fra, frc);
    frd = builder.CreateFSub(frd, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fmsubsx(Instruction code)
{
    Value<F64> fra = getFPR(code.fra);
    Value<F64> frc = getFPR(code.frc);
    Value<F64> frb = getFPR(code.frb);
    Value<F64> frd;

    // TEST: Is negating frb and calling llvm::Intrinsic::fmuladd faster?
    frd = builder.CreateFMul(fra, frc);
    frd = builder.CreateFSub(frd, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.CreateFPTrunc<F32>(frd));
}

void Recompiler::fmulx(Instruction code)
{
    Value<F64> fra = getFPR(code.fra);
    Value<F64> frb = getFPR(code.frb);
    Value<F64> frd;

    frd = builder.CreateFMul(fra, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fmulsx(Instruction code)
{
    Value<F64> fra = getFPR(code.fra);
    Value<F64> frb = getFPR(code.frb);
    Value<F64> frd;

    frd = builder.CreateFMul(fra, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.CreateFPTrunc<F32>(frd));
}

void Recompiler::fnabsx(Instruction code)
{
    Value<F64> frb = getFPR(code.frb);
    Value<F64> frd;

    frd = builder.CreateIntrinsic_Fabs(frb);
    frd = builder.CreateFNeg(frd);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fnegx(Instruction code)
{
    Value<F64> frb = getFPR(code.frb);
    Value<F64> frd;

    frd = builder.CreateFNeg(frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fnmaddx(Instruction code)
{
    Value<F64> fra = getFPR(code.fra);
    Value<F64> frc = getFPR(code.frc);
    Value<F64> frb = getFPR(code.frb);
    Value<F64> frd;

    frd = builder.CreateIntrinsic_Fmuladd(fra, frc, frb);
    frd = builder.CreateFNeg(frd);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fnmaddsx(Instruction code)
{
    Value<F64> fra = getFPR(code.fra);
    Value<F64> frc = getFPR(code.frc);
    Value<F64> frb = getFPR(code.frb);
    Value<F64> frd;

    frd = builder.CreateIntrinsic_Fmuladd(fra, frc, frb);
    frd = builder.CreateFNeg(frd);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.CreateFPTrunc<F32>(frd));
}

void Recompiler::fnmsubx(Instruction code)
{
    Value<F64> fra = getFPR(code.fra);
    Value<F64> frc = getFPR(code.frc);
    Value<F64> frb = getFPR(code.frb);
    Value<F64> frd;

    // TEST: Is negating frb and calling llvm::Intrinsic::fmuladd faster?
    frd = builder.CreateFMul(fra, frc);
    frd = builder.CreateFSub(frd, frb);
    frd = builder.CreateFNeg(frd);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fnmsubsx(Instruction code)
{
    Value<F64> fra = getFPR(code.fra);
    Value<F64> frc = getFPR(code.frc);
    Value<F64> frb = getFPR(code.frb);
    Value<F64> frd;

    // TEST: Is negating frb and calling llvm::Intrinsic::fmuladd faster?
    frd = builder.CreateFMul(fra, frc);
    frd = builder.CreateFSub(frd, frb);
    frd = builder.CreateFNeg(frd);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.CreateFPTrunc<F32>(frd));
}

void Recompiler::fresx(Instruction code)
{
}

void Recompiler::frspx(Instruction code)
{
}

void Recompiler::frsqrtex(Instruction code)
{
}

void Recompiler::fselx(Instruction code)
{
}

void Recompiler::fsqrtx(Instruction code)
{
    Value<F64> frb = getFPR(code.frb);
    Value<F64> frd;

    frd = builder.CreateIntrinsic_Sqrt(frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fsqrtsx(Instruction code)
{
    Value<F64> frb = getFPR(code.frb);
    Value<F64> frd;

    frd = builder.CreateIntrinsic_Sqrt(frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.CreateFPTrunc<F32>(frd));
}

void Recompiler::fsubx(Instruction code)
{
    Value<F64> fra = getFPR(code.fra);
    Value<F64> frb = getFPR(code.frb);
    Value<F64> frd;

    frd = builder.CreateFSub(fra, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fsubsx(Instruction code)
{
    Value<F64> fra = getFPR(code.fra);
    Value<F64> frb = getFPR(code.frb);
    Value<F64> frd;

    frd = builder.CreateFSub(fra, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.CreateFPTrunc<F32>(frd));
}

void Recompiler::mcrfs(Instruction code)
{
}

void Recompiler::mffsx(Instruction code)
{
}

void Recompiler::mtfsb0x(Instruction code)
{
}

void Recompiler::mtfsb1x(Instruction code)
{
}

void Recompiler::mtfsfix(Instruction code)
{
}

void Recompiler::mtfsfx(Instruction code)
{
}

}  // namespace ppu
}  // namespace cpu
