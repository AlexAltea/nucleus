/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_translator.h"
#include "nucleus/assert.h"

namespace cpu {
namespace frontend {
namespace ppu {

using namespace cpu::hir;

/**
 * PPC64 Instructions:
 *  - UISA: Floating-Point Instructions (Section: 4.2.2)
 */

void Translator::fabsx(Instruction code)
{
    Value* frb = getFPR(code.frb);
    Value* frd;

    frd = builder.createAbs(frb);
    if (code.rc) {
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Translator::faddx(Instruction code)
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

void Translator::faddsx(Instruction code)
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

void Translator::fcfidx(Instruction code)
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

void Translator::fcmpo(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frb = getFPR(code.frb);

    // TODO: Ordered?

    updateCR(code.crfd, fra, frb, false);
}

void Translator::fcmpu(Instruction code)
{
    Value* fra = getFPR(code.fra);
    Value* frb = getFPR(code.frb);

    // TODO: Unordered?

    updateCR(code.crfd, fra, frb, false);
}

void Translator::fctidx(Instruction code)
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

void Translator::fctidzx(Instruction code)
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

void Translator::fctiwx(Instruction code)
{
    Value* frb = getFPR(code.frb);
    Value* frd;

    // TODO: Saturation check
    frd = builder.createConvert(frb, TYPE_I32);
    frd = builder.createZExt(frd, TYPE_I64);
    frd = builder.createCast(frd, TYPE_F64);
    if (code.rc) {
        assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Translator::fctiwzx(Instruction code)
{
    Value* frb = getFPR(code.frb);
    Value* frd;

    // TODO: Saturation check
    frd = builder.createConvert(frb, TYPE_I32);
    frd = builder.createZExt(frd, TYPE_I64);
    frd = builder.createCast(frd, TYPE_F64);
    if (code.rc) {
        assert_always("Unimplemented");
        // TODO: CR1 update
    }

    setFPR(code.frd, frd);
}

void Translator::fdivx(Instruction code)
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

void Translator::fdivsx(Instruction code)
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

void Translator::fmaddx(Instruction code)
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

void Translator::fmaddsx(Instruction code)
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

void Translator::fmrx(Instruction code)
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

void Translator::fmsubx(Instruction code)
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

void Translator::fmsubsx(Instruction code)
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

void Translator::fmulx(Instruction code)
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

void Translator::fmulsx(Instruction code)
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

void Translator::fnabsx(Instruction code)
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

void Translator::fnegx(Instruction code)
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

void Translator::fnmaddx(Instruction code)
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

void Translator::fnmaddsx(Instruction code)
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

void Translator::fnmsubx(Instruction code)
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

void Translator::fnmsubsx(Instruction code)
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

void Translator::fresx(Instruction code)
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

void Translator::frspx(Instruction code)
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

void Translator::frsqrtex(Instruction code)
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

void Translator::fselx(Instruction code)
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

void Translator::fsqrtx(Instruction code)
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

void Translator::fsqrtsx(Instruction code)
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

void Translator::fsubx(Instruction code)
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

void Translator::fsubsx(Instruction code)
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

void Translator::mcrfs(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::mffsx(Instruction code)
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

void Translator::mtfsb0x(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::mtfsb1x(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::mtfsfix(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::mtfsfx(Instruction code)
{
    assert_always("Unimplemented");
}

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
