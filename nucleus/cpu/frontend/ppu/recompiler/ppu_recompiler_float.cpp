/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"
#include "nucleus/assert.h"

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
        assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::faddsx(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createFAdd(fra, frb);
    frd = builder.createConvert(frd, TYPE_F32);
    if (code.rc) {
        assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fcfidx(Instruction code)
{
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createCast(frb, TYPE_I64);
    frd = builder.createConvert(frd, TYPE_F64);
    if (code.rc) {
        assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fcmpo(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frb = getFPR(code.frb);

    // TODO: Ordered?

    updateCR(code.crfd, fra, frb, false);
}

void Recompiler::fcmpu(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frb = getFPR(code.frb);

    // TODO: Unordered?

    updateCR(code.crfd, fra, frb, false);
}

void Recompiler::fctidx(Instruction code)
{
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createConvert(frb, TYPE_I64);
    frd = builder.createCast(frd, TYPE_F64);
    if (code.rc) {
        assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fctidzx(Instruction code)
{
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createConvert(frb, TYPE_I64);
    frd = builder.createCast(frd, TYPE_F64);
    if (code.rc) {
        assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fctiwx(Instruction code)
{
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createConvert(frb, TYPE_I32);
    frd = builder.createZExt(frd, TYPE_I64);
    frd = builder.createCast(frd, TYPE_F64);
    if (code.rc) {
        assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fctiwzx(Instruction code)
{
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createConvert(frb, TYPE_I32);
    frd = builder.createZExt(frd, TYPE_I64);
    frd = builder.createCast(frd, TYPE_F64);
    if (code.rc) {
        assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fdivx(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createFDiv(fra, frb);
    if (code.rc) {
        assert_always("Unimplemented");
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
        assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.createConvert(frd, TYPE_F32));
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
        assert_always("Unimplemented");
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
        assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.createConvert(frd, TYPE_F32));
}

void Recompiler::fmrx(Instruction code)
{
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = frb;
    if (code.rc) {
        assert_always("Unimplemented");
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
        assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.createConvert(frd, TYPE_F32));
}

void Recompiler::fmulx(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frc = getFPR(code.frc);
    Value* frd;

    frd = builder.createFMul(fra, frc);
    if (code.rc) {
        assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fmulsx(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frc = getFPR(code.frc);
    Value* frd;

    frd = builder.createFMul(fra, frc);
    if (code.rc) {
        assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.createConvert(frd, TYPE_F32));
}

void Recompiler::fnabsx(Instruction code)
{
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createAbs(frb);
    frd = builder.createFNeg(frd);
    if (code.rc) {
        assert_always("Unimplemented");
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
        assert_always("Unimplemented");
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
        assert_always("Unimplemented");
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
        assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.createConvert(frd, TYPE_F32));
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
        assert_always("Unimplemented");
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
        assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.createConvert(frd, TYPE_F32));
}

void Recompiler::fresx(Instruction code)
{
    Value* frb = getFPR(code.frb, TYPE_F32);
    Value* frd;

    frd = builder.createFDiv(builder.getConstantF32(1.0f), frb);
    frd = builder.createConvert(frd, TYPE_F64);
    if (code.rc) {
        //assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::frspx(Instruction code)
{
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createConvert(frb, TYPE_F32);
    frd = builder.createConvert(frd, TYPE_F64);
    if (code.rc) {
        assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::frsqrtex(Instruction code)
{
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createSqrt(frb);
    frd = builder.createFDiv(builder.getConstantF64(1.0), frd);
    if (code.rc) {
        assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fselx(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frc = getFPR(code.frc);
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createCmpSGE(fra, builder.getConstantF64(0.0));
    frd = builder.createSelect(frd, frc, frb);
    if (code.rc) {
        assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::fsqrtx(Instruction code)
{
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createSqrt(frb);
    if (code.rc) {
        assert_always("Unimplemented");
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
        assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.createConvert(frd, TYPE_F32));
}

void Recompiler::fsubx(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createFSub(fra, frb);
    if (code.rc) {
        assert_always("Unimplemented");
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
        assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, builder.createConvert(frd, TYPE_F32));
}

void Recompiler::mcrfs(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::mffsx(Instruction code)
{
    Value* frd;

    frd = builder.createZExt(getFPSCR(), TYPE_I64);
    frd = builder.createCast(frd, TYPE_F64);
    if (code.rc) {
        //assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Recompiler::mtfsb0x(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::mtfsb1x(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::mtfsfix(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::mtfsfx(Instruction code)
{
    assert_always("Unimplemented");
}

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
