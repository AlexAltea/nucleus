/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"

namespace cpu {
namespace frontend {
namespace ppu {

using namespace cpu::hir;

/**
 * PPC64 Instructions:
 *  - UISA: Floating-Point Instructions (Section: 4.2.2)
 */

void Recompiler::fabsx(Instruction code)
{
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createAbs(frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::faddx(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createFAdd(fra, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::faddsx(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frb = getFPR(code.frb);

    auto result = builder.createFAdd(fra, frb);
    auto frd = builder.createFTrunc(result, TYPE_F32);
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
    Value* fra = getFPR(code.fra);
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createFDiv(fra, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fdivsx(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createFDiv(fra, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.createFTrunc(frd, TYPE_F32));
}

void Recompiler::fmaddx(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frc = getFPR(code.frc);
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createFMul(fra, frc);
    frd = builder.createFAdd(frd, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fmaddsx(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frc = getFPR(code.frc);
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createFMul(fra, frc);
    frd = builder.createFAdd(frd, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.createFTrunc(frd, TYPE_F32));
}

void Recompiler::fmrx(Instruction code)
{
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = frb;
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fmsubx(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frc = getFPR(code.frc);
    Value* frb = getFPR(code.frb);
    Value* frd;

    // TEST: Is negating frb and calling llvm::Intrinsic::fmuladd faster?
    frd = builder.createFMul(fra, frc);
    frd = builder.createFSub(frd, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fmsubsx(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frc = getFPR(code.frc);
    Value* frb = getFPR(code.frb);
    Value* frd;

    // TEST: Is negating frb and calling llvm::Intrinsic::fmuladd faster?
    frd = builder.createFMul(fra, frc);
    frd = builder.createFSub(frd, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.createFTrunc(frd, TYPE_F32));
}

void Recompiler::fmulx(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createFMul(fra, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fmulsx(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createFMul(fra, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.createFTrunc(frd, TYPE_F32));
}

void Recompiler::fnabsx(Instruction code)
{
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createAbs(frb);
    frd = builder.createFNeg(frd);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fnegx(Instruction code)
{
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createFNeg(frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fnmaddx(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frc = getFPR(code.frc);
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createFMul(fra, frc);
    frd = builder.createFAdd(frd, frb);
    frd = builder.createFNeg(frd);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fnmaddsx(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frc = getFPR(code.frc);
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createFMul(fra, frc);
    frd = builder.createFAdd(frd, frb);
    frd = builder.createFNeg(frd);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.createFTrunc(frd, TYPE_F32));
}

void Recompiler::fnmsubx(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frc = getFPR(code.frc);
    Value* frb = getFPR(code.frb);
    Value* frd;

    // TEST: Is negating frb and calling llvm::Intrinsic::fmuladd faster?
    frd = builder.createFMul(fra, frc);
    frd = builder.createFSub(frd, frb);
    frd = builder.createFNeg(frd);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fnmsubsx(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frc = getFPR(code.frc);
    Value* frb = getFPR(code.frb);
    Value* frd;

    // TEST: Is negating frb and calling llvm::Intrinsic::fmuladd faster?
    frd = builder.createFMul(fra, frc);
    frd = builder.createFSub(frd, frb);
    frd = builder.createFNeg(frd);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.createFTrunc(frd, TYPE_F32));
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
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createSqrt(frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fsqrtsx(Instruction code)
{
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createSqrt(frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.createFTrunc(frd, TYPE_F32));
}

void Recompiler::fsubx(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createFSub(fra, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fsubsx(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createFSub(fra, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.createFTrunc(frd, TYPE_F32));
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
}  // namespace frontend
}  // namespace cpu
