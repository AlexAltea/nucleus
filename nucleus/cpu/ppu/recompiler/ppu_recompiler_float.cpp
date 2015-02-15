/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"

namespace cpu {
namespace ppu {

/**
 * PPC64 Instructions:
 *  - UISA: Floating-Point Instructions (Section: 4.2.2)
 */

void Recompiler::fabsx(Instruction code)
{
    llvm::Value* frb = getFPR(code.frb);
    llvm::Value* frd;

    llvm::Function* fabs = getIntrinsicDouble(llvm::Intrinsic::fabs);
    frd = builder.CreateCall(fabs, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::faddx(Instruction code)
{
    llvm::Value* fra = getFPR(code.fra);
    llvm::Value* frb = getFPR(code.frb);
    llvm::Value* frd;

    frd = builder.CreateFAdd(fra, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::faddsx(Instruction code)
{
    llvm::Value* fra = getFPR(code.fra);
    llvm::Value* frb = getFPR(code.frb);
    llvm::Value* frd;

    frd = builder.CreateFAdd(fra, frb);
    frd = builder.CreateFPTrunc(frd, builder.getFloatTy());
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
    llvm::Value* fra = getFPR(code.fra);
    llvm::Value* frb = getFPR(code.frb);
    llvm::Value* frd;

    frd = builder.CreateFDiv(fra, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fdivsx(Instruction code)
{
    llvm::Value* fra = getFPR(code.fra);
    llvm::Value* frb = getFPR(code.frb);
    llvm::Value* frd;

    frd = builder.CreateFDiv(fra, frb);
    frd = builder.CreateFPTrunc(frd, builder.getFloatTy());
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fmaddx(Instruction code)
{
    llvm::Value* fra = getFPR(code.fra);
    llvm::Value* frc = getFPR(code.frc);
    llvm::Value* frb = getFPR(code.frb);
    llvm::Value* frd;

    llvm::Function* fmuladd = getIntrinsicDouble(llvm::Intrinsic::fmuladd);
    frd = builder.CreateCall3(fmuladd, fra, frc, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fmaddsx(Instruction code)
{
    llvm::Value* fra = getFPR(code.fra);
    llvm::Value* frc = getFPR(code.frc);
    llvm::Value* frb = getFPR(code.frb);
    llvm::Value* frd;

    llvm::Function* fmuladd = getIntrinsicDouble(llvm::Intrinsic::fmuladd);
    frd = builder.CreateCall3(fmuladd, fra, frc, frb);
    frd = builder.CreateFPTrunc(frd, builder.getFloatTy());
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fmrx(Instruction code)
{
    llvm::Value* frb = getFPR(code.frb);
    llvm::Value* frd;

    frd = frb;
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fmsubx(Instruction code)
{
    llvm::Value* fra = getFPR(code.fra);
    llvm::Value* frc = getFPR(code.frc);
    llvm::Value* frb = getFPR(code.frb);
    llvm::Value* frd;

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
    llvm::Value* fra = getFPR(code.fra);
    llvm::Value* frc = getFPR(code.frc);
    llvm::Value* frb = getFPR(code.frb);
    llvm::Value* frd;

    // TEST: Is negating frb and calling llvm::Intrinsic::fmuladd faster?
    frd = builder.CreateFMul(fra, frc);
    frd = builder.CreateFSub(frd, frb);
    frd = builder.CreateFPTrunc(frd, builder.getFloatTy());
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fmulx(Instruction code)
{
    llvm::Value* fra = getFPR(code.fra);
    llvm::Value* frb = getFPR(code.frb);
    llvm::Value* frd;

    frd = builder.CreateFMul(fra, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fmulsx(Instruction code)
{
    llvm::Value* fra = getFPR(code.fra);
    llvm::Value* frb = getFPR(code.frb);
    llvm::Value* frd;

    frd = builder.CreateFMul(fra, frb);
    frd = builder.CreateFPTrunc(frd, builder.getFloatTy());
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fnabsx(Instruction code)
{
    llvm::Value* frb = getFPR(code.frb);
    llvm::Value* frd;

    llvm::Function* fabs = getIntrinsicDouble(llvm::Intrinsic::fabs);
    frd = builder.CreateCall(fabs, frb);
    frd = builder.CreateFNeg(frd);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fnegx(Instruction code)
{
    llvm::Value* frb = getFPR(code.frb);
    llvm::Value* frd;

    frd = builder.CreateFNeg(frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fnmaddx(Instruction code)
{
    llvm::Value* fra = getFPR(code.fra);
    llvm::Value* frc = getFPR(code.frc);
    llvm::Value* frb = getFPR(code.frb);
    llvm::Value* frd;

    llvm::Function* fmuladd = getIntrinsicDouble(llvm::Intrinsic::fmuladd);
    frd = builder.CreateCall3(fmuladd, fra, frc, frb);
    frd = builder.CreateFNeg(frd);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fnmaddsx(Instruction code)
{
    llvm::Value* fra = getFPR(code.fra);
    llvm::Value* frc = getFPR(code.frc);
    llvm::Value* frb = getFPR(code.frb);
    llvm::Value* frd;

    llvm::Function* fmuladd = getIntrinsicDouble(llvm::Intrinsic::fmuladd);
    frd = builder.CreateCall3(fmuladd, fra, frc, frb);
    frd = builder.CreateFNeg(frd);
    frd = builder.CreateFPTrunc(frd, builder.getFloatTy());
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fnmsubx(Instruction code)
{
    llvm::Value* fra = getFPR(code.fra);
    llvm::Value* frc = getFPR(code.frc);
    llvm::Value* frb = getFPR(code.frb);
    llvm::Value* frd;

    // TEST: Is negating frb and calling llvm::Intrinsic::fmuladd faster?
    frd = builder.CreateFMul(fra, frc);
    frd = builder.CreateFSub(frd, frb);
    frd = builder.CreateNeg(frd);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fnmsubsx(Instruction code)
{
    llvm::Value* fra = getFPR(code.fra);
    llvm::Value* frc = getFPR(code.frc);
    llvm::Value* frb = getFPR(code.frb);
    llvm::Value* frd;

    // TEST: Is negating frb and calling llvm::Intrinsic::fmuladd faster?
    frd = builder.CreateFMul(fra, frc);
    frd = builder.CreateFSub(frd, frb);
    frd = builder.CreateNeg(frd);
    frd = builder.CreateFPTrunc(frd, builder.getFloatTy());
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
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
    llvm::Value* frb = getFPR(code.frb);
    llvm::Value* frd;

    llvm::Function* sqrt = getIntrinsicDouble(llvm::Intrinsic::sqrt);
    frd = builder.CreateCall(sqrt, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fsqrtsx(Instruction code)
{
    llvm::Value* frb = getFPR(code.frb);
    llvm::Value* frd;

    llvm::Function* sqrt = getIntrinsicDouble(llvm::Intrinsic::sqrt);
    frd = builder.CreateCall(sqrt, frb);
    frd = builder.CreateFPTrunc(frd, builder.getFloatTy());
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fsubx(Instruction code)
{
    llvm::Value* fra = getFPR(code.fra);
    llvm::Value* frb = getFPR(code.frb);
    llvm::Value* frd;

    frd = builder.CreateFSub(fra, frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fsubsx(Instruction code)
{
    llvm::Value* fra = getFPR(code.fra);
    llvm::Value* frb = getFPR(code.frb);
    llvm::Value* frd;

    frd = builder.CreateFSub(fra, frb);
    frd = builder.CreateFPTrunc(frd, builder.getFloatTy());
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
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
