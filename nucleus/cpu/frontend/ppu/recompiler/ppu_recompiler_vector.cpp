/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"

namespace cpu {
namespace ppu {

using namespace cpu::hir;

/**
 * PPC64 Vector/SIMD Instructions (aka AltiVec):
 *  - Vector UISA Instructions (Section: 4.2.x)
 *  - Vector VEA Instructions (Section: 4.3.x)
 */

void Recompiler::dss(Instruction code)
{
}

void Recompiler::dst(Instruction code)
{
}

void Recompiler::dstst(Instruction code)
{
}

void Recompiler::lvebx(Instruction code)
{
}

void Recompiler::lvehx(Instruction code)
{
}

void Recompiler::lvewx(Instruction code)
{
}

void Recompiler::lvlx(Instruction code)
{
}

void Recompiler::lvlxl(Instruction code)
{
}

void Recompiler::lvrx(Instruction code)
{
}

void Recompiler::lvrxl(Instruction code)
{
}

void Recompiler::lvsl(Instruction code)
{
}

void Recompiler::lvsr(Instruction code)
{
}

void Recompiler::lvx(Instruction code)
{
}

void Recompiler::lvxl(Instruction code)
{
}

void Recompiler::mfvscr(Instruction code)
{
}

void Recompiler::mtvscr(Instruction code)
{
}

void Recompiler::stvebx(Instruction code)
{
}

void Recompiler::stvehx(Instruction code)
{
}

void Recompiler::stvewx(Instruction code)
{
}

void Recompiler::stvlx(Instruction code)
{
}

void Recompiler::stvlxl(Instruction code)
{
}

void Recompiler::stvrx(Instruction code)
{
}

void Recompiler::stvrxl(Instruction code)
{
}

void Recompiler::stvx(Instruction code)
{
}

void Recompiler::stvxl(Instruction code)
{
}

void Recompiler::vaddcuw(Instruction code)
{
}

void Recompiler::vaddfp(Instruction code)
{
    Value<F32, 4> va = getVR<F32>(code.va);
    Value<F32, 4> vb = getVR<F32>(code.vb);
    Value<F32, 4> vd;

    vd = builder.CreateFAdd(va, vb);

    //setVR(code.vd, vd);
}

void Recompiler::vaddsbs(Instruction code)
{
    Value<I8, 16> va = getVR<I8>(code.va);
    Value<I8, 16> vb = getVR<I8>(code.vb);
    Value<I8, 16> vd = va; // TODO: This is wrong. Just a temporary fix

    // TODO: ?

    //setVR(code.vd, vd);
}

void Recompiler::vaddshs(Instruction code)
{
    Value<I16, 8> va = getVR<I16>(code.va);
    Value<I16, 8> vb = getVR<I16>(code.vb);
    Value<I16, 8> vd = va; // TODO: This is wrong. Just a temporary fix

    // TODO: ?

    //setVR(code.vd, vd);
}

void Recompiler::vaddsws(Instruction code)
{
    Value<I32, 4> va = getVR<I32>(code.va);
    Value<I32, 4> vb = getVR<I32>(code.vb);
    Value<I32, 4> vd = va; // TODO: This is wrong. Just a temporary fix

    // TODO: ?

    //setVR(code.vd, vd);
}

void Recompiler::vaddubm(Instruction code)
{
    Value<I8, 16> va = getVR<I8>(code.va);
    Value<I8, 16> vb = getVR<I8>(code.vb);
    Value<I8, 16> vd;

    vd = builder.CreateAdd(va, vb);

    //setVR(code.vd, vd);
}

void Recompiler::vaddubs(Instruction code)
{
    Value<I8, 16> va = getVR<I8>(code.va);
    Value<I8, 16> vb = getVR<I8>(code.vb);
    Value<I8, 16> vd = va; // TODO: This is wrong. Just a temporary fix

    // TODO: ?

    //setVR(code.vd, vd);
}

void Recompiler::vadduhm(Instruction code)
{
    Value<I16, 8> va = getVR<I16>(code.va);
    Value<I16, 8> vb = getVR<I16>(code.vb);
    Value<I16, 8> vd;

    vd = builder.CreateAdd(va, vb);

    //setVR(code.vd, vd);
}

void Recompiler::vadduhs(Instruction code)
{
    Value<I16, 8> va = getVR<I16>(code.va);
    Value<I16, 8> vb = getVR<I16>(code.vb);
    Value<I16, 8> vd = va; // TODO: This is wrong. Just a temporary fix

    // TODO: ?

    //setVR(code.vd, vd);
}

void Recompiler::vadduwm(Instruction code)
{
    Value<I32, 4> va = getVR<I32>(code.va);
    Value<I32, 4> vb = getVR<I32>(code.vb);
    Value<I32, 4> vd;

    vd = builder.CreateAdd(va, vb);

    //setVR(code.vd, vd);
}

void Recompiler::vadduws(Instruction code)
{
    Value<I32, 4> va = getVR<I32>(code.va);
    Value<I32, 4> vb = getVR<I32>(code.vb);
    Value<I32, 4> vd = va; // TODO: This is wrong. Just a temporary fix

    // TODO: ?

    //setVR(code.vd, vd);
}

void Recompiler::vand(Instruction code)
{
    Value<I32, 4> va = getVR<I32>(code.va);
    Value<I32, 4> vb = getVR<I32>(code.vb);
    Value<I32, 4> vd;

    vd = builder.CreateAnd(va, vb);

    //setVR(code.vd, vd);
}

void Recompiler::vandc(Instruction code)
{
    Value<I32, 4> va = getVR<I32>(code.va);
    Value<I32, 4> vb = getVR<I32>(code.vb);
    Value<I32, 4> vd;

    vb = builder.CreateNot(vb);
    vd = builder.CreateAnd(va, vb);

    //setVR(code.vd, vd);
}

void Recompiler::vavgsb(Instruction code)
{
    Value<I8, 16> va = getVR<I8>(code.va);
    Value<I8, 16> vb = getVR<I8>(code.vb);
    Value<I8, 16> vd;

    vd = builder.CreateSAverage(va, vb);

    setVR(code.vd, vd);
}

void Recompiler::vavgsh(Instruction code)
{
    Value<I16, 8> va = getVR<I16>(code.va);
    Value<I16, 8> vb = getVR<I16>(code.vb);
    Value<I16, 8> vd;

    vd = builder.CreateSAverage(va, vb);

    setVR(code.vd, vd);
}

void Recompiler::vavgsw(Instruction code)
{
    Value<I32, 4> va = getVR<I32>(code.va);
    Value<I32, 4> vb = getVR<I32>(code.vb);
    Value<I32, 4> vd;

    vd = builder.CreateSAverage(va, vb);

    setVR(code.vd, vd);
}

void Recompiler::vavgub(Instruction code)
{
    Value<I8, 16> va = getVR<I8>(code.va);
    Value<I8, 16> vb = getVR<I8>(code.vb);
    Value<I8, 16> vd;

    vd = builder.CreateUAverage(va, vb);

    setVR(code.vd, vd);
}

void Recompiler::vavguh(Instruction code)
{
    Value<I16, 8> va = getVR<I16>(code.va);
    Value<I16, 8> vb = getVR<I16>(code.vb);
    Value<I16, 8> vd;

    vd = builder.CreateUAverage(va, vb);

    setVR(code.vd, vd);
}

void Recompiler::vavguw(Instruction code)
{
    Value<I32, 4> va = getVR<I32>(code.va);
    Value<I32, 4> vb = getVR<I32>(code.vb);
    Value<I32, 4> vd;

    vd = builder.CreateUAverage(va, vb);

    setVR(code.vd, vd);
}

void Recompiler::vcfsx(Instruction code)
{
}

void Recompiler::vcfux(Instruction code)
{
}

void Recompiler::vcmpbfp(Instruction code)
{
}

void Recompiler::vcmpbfp_(Instruction code)
{
    vcmpbfp(code);
    // TODO: CR6 update
}

void Recompiler::vcmpeqfp(Instruction code)
{
    Value<F32, 4> va = getVR<F32>(code.va);
    Value<F32, 4> vb = getVR<F32>(code.vb);

    auto result = builder.CreateFCmpOEQ(va, vb);
    auto vd = builder.CreateSExt<I32>(result);

    setVR(code.vd, vd);
}

void Recompiler::vcmpeqfp_(Instruction code)
{
    vcmpeqfp(code);
    // TODO: CR6 update
}

void Recompiler::vcmpequb(Instruction code)
{
    Value<I8, 16> va = getVR<I8>(code.va);
    Value<I8, 16> vb = getVR<I8>(code.vb);

    auto result = builder.CreateICmpEQ(va, vb);
    auto vd = builder.CreateSExt<I8>(result);

    setVR(code.vd, vd);
}

void Recompiler::vcmpequb_(Instruction code)
{
    vcmpequb(code);
    // TODO: CR6 update
}

void Recompiler::vcmpequh(Instruction code)
{
    Value<I16, 8> va = getVR<I16>(code.va);
    Value<I16, 8> vb = getVR<I16>(code.vb);

    auto result = builder.CreateICmpEQ(va, vb);
    auto vd = builder.CreateSExt<I16>(result);

    setVR(code.vd, vd);
}

void Recompiler::vcmpequh_(Instruction code)
{
    vcmpequh(code);
    // TODO: CR6 update
}

void Recompiler::vcmpequw(Instruction code)
{
    Value<I32, 4> va = getVR<I32>(code.va);
    Value<I32, 4> vb = getVR<I32>(code.vb);

    auto result = builder.CreateICmpEQ(va, vb);
    auto vd = builder.CreateSExt<I32>(result);

    setVR(code.vd, vd);
}

void Recompiler::vcmpequw_(Instruction code)
{
    vcmpequw(code);
    // TODO: CR6 update
}

void Recompiler::vcmpgefp(Instruction code)
{
    Value<F32, 4> va = getVR<F32>(code.va);
    Value<F32, 4> vb = getVR<F32>(code.vb);

    auto result = builder.CreateFCmpOGE(va, vb);
    auto vd = builder.CreateSExt<I32>(result);

    setVR(code.vd, vd);
}

void Recompiler::vcmpgefp_(Instruction code)
{
    vcmpgefp(code);
    // TODO: CR6 update
}

void Recompiler::vcmpgtfp(Instruction code)
{
    Value<F32, 4> va = getVR<F32>(code.va);
    Value<F32, 4> vb = getVR<F32>(code.vb);

    auto result = builder.CreateFCmpOGT(va, vb);
    auto vd = builder.CreateSExt<I32>(result);

    setVR(code.vd, vd);
}

void Recompiler::vcmpgtfp_(Instruction code)
{
    vcmpgtfp(code);
    // TODO: CR6 update
}

void Recompiler::vcmpgtsb(Instruction code)
{
    Value<I8, 16> va = getVR<I8>(code.va);
    Value<I8, 16> vb = getVR<I8>(code.vb);

    auto result = builder.CreateICmpSGT(va, vb);
    auto vd = builder.CreateSExt<I8>(result);

    setVR(code.vd, vd);
}

void Recompiler::vcmpgtsb_(Instruction code)
{
    vcmpgtsb(code);
    // TODO: CR6 update
}

void Recompiler::vcmpgtsh(Instruction code)
{
    Value<I16, 8> va = getVR<I16>(code.va);
    Value<I16, 8> vb = getVR<I16>(code.vb);

    auto result = builder.CreateICmpSGT(va, vb);
    auto vd = builder.CreateSExt<I16>(result);

    setVR(code.vd, vd);
}

void Recompiler::vcmpgtsh_(Instruction code)
{
    vcmpgtsh(code);
    // TODO: CR6 update
}

void Recompiler::vcmpgtsw(Instruction code)
{
    Value<I32, 4> va = getVR<I32>(code.va);
    Value<I32, 4> vb = getVR<I32>(code.vb);

    auto result = builder.CreateICmpSGT(va, vb);
    auto vd = builder.CreateSExt<I32>(result);

    setVR(code.vd, vd);
}

void Recompiler::vcmpgtsw_(Instruction code)
{
    vcmpgtsw(code);
    // TODO: CR6 update
}

void Recompiler::vcmpgtub(Instruction code)
{
    Value<I8, 16> va = getVR<I8>(code.va);
    Value<I8, 16> vb = getVR<I8>(code.vb);

    auto result = builder.CreateICmpUGT(va, vb);
    auto vd = builder.CreateSExt<I8>(result);

    setVR(code.vd, vd);
}

void Recompiler::vcmpgtub_(Instruction code)
{
    vcmpgtub(code);
    // TODO: CR6 update
}

void Recompiler::vcmpgtuh(Instruction code)
{
    Value<I16, 8> va = getVR<I16>(code.va);
    Value<I16, 8> vb = getVR<I16>(code.vb);

    auto result = builder.CreateICmpUGT(va, vb);
    auto vd = builder.CreateSExt<I16>(result);

    setVR(code.vd, vd);
}

void Recompiler::vcmpgtuh_(Instruction code)
{
    vcmpgtuh(code);
    // TODO: CR6 update
}

void Recompiler::vcmpgtuw(Instruction code)
{
    Value<I32, 4> va = getVR<I32>(code.va);
    Value<I32, 4> vb = getVR<I32>(code.vb);

    auto result = builder.CreateICmpUGT(va, vb);
    auto vd = builder.CreateSExt<I32>(result);

    setVR(code.vd, vd);
}

void Recompiler::vcmpgtuw_(Instruction code)
{
    vcmpgtuw(code);
    // TODO: CR6 update
}

void Recompiler::vctsxs(Instruction code)
{
}

void Recompiler::vctuxs(Instruction code)
{
}

void Recompiler::vexptefp(Instruction code)
{
    Value<F32, 4> vb = getVR<F32>(code.vb);
    Value<F32, 4> vd = vb; // TODO: This is wrong. Just a temporary fix

    //llvm::Function* exp2 = getIntrinsicFloat(llvm::Intrinsic::exp2); // TODO: Get a 4xf32 intrinsic instead
    //vd = builder.CreateCall(exp2, vb);

    setVR(code.vd, vd);
}

void Recompiler::vlogefp(Instruction code)
{
    Value<F32, 4> vb = getVR<F32>(code.vb);
    Value<F32, 4> vd = vb; // TODO: This is wrong. Just a temporary fix

    //llvm::Function* log2 = getIntrinsicFloat(llvm::Intrinsic::log2); // TODO: Get a 4xf32 intrinsic instead
    //vd = builder.CreateCall(log2, vb);

    setVR(code.vd, vd);
}

void Recompiler::vmaddfp(Instruction code)
{
    Value<F32, 4> va = getVR<F32>(code.va);
    Value<F32, 4> vc = getVR<F32>(code.vc);
    Value<F32, 4> vb = getVR<F32>(code.vb);
    Value<F32, 4> vd;

    vd = builder.CreateFMul(va, vc);
    vd = builder.CreateFAdd(vd, vb);

    setVR(code.vd, vd);
}

void Recompiler::vmaxfp(Instruction code)
{
}

void Recompiler::vmaxsb(Instruction code)
{
}

void Recompiler::vmaxsh(Instruction code)
{
}

void Recompiler::vmaxsw(Instruction code)
{
}

void Recompiler::vmaxub(Instruction code)
{
}

void Recompiler::vmaxuh(Instruction code)
{
}

void Recompiler::vmaxuw(Instruction code)
{
}

void Recompiler::vmhaddshs(Instruction code)
{
}

void Recompiler::vmhraddshs(Instruction code)
{
}

void Recompiler::vminfp(Instruction code)
{
}

void Recompiler::vminsb(Instruction code)
{
}

void Recompiler::vminsh(Instruction code)
{
}

void Recompiler::vminsw(Instruction code)
{
}

void Recompiler::vminub(Instruction code)
{
}

void Recompiler::vminuh(Instruction code)
{
}

void Recompiler::vminuw(Instruction code)
{
}

void Recompiler::vmladduhm(Instruction code)
{
}

void Recompiler::vmrghb(Instruction code)
{
}

void Recompiler::vmrghh(Instruction code)
{
}

void Recompiler::vmrghw(Instruction code)
{
}

void Recompiler::vmrglb(Instruction code)
{
}

void Recompiler::vmrglh(Instruction code)
{
}

void Recompiler::vmrglw(Instruction code)
{
}

void Recompiler::vmsummbm(Instruction code)
{
}

void Recompiler::vmsumshm(Instruction code)
{
}

void Recompiler::vmsumshs(Instruction code)
{
}

void Recompiler::vmsumubm(Instruction code)
{
}

void Recompiler::vmsumuhm(Instruction code)
{
}

void Recompiler::vmsumuhs(Instruction code)
{
}

void Recompiler::vmulesb(Instruction code)
{
}

void Recompiler::vmulesh(Instruction code)
{
}

void Recompiler::vmuleub(Instruction code)
{
}

void Recompiler::vmuleuh(Instruction code)
{
}

void Recompiler::vmulosb(Instruction code)
{
}

void Recompiler::vmulosh(Instruction code)
{
}

void Recompiler::vmuloub(Instruction code)
{
}

void Recompiler::vmulouh(Instruction code)
{
}

void Recompiler::vnmsubfp(Instruction code)
{
    Value<F32, 4> va = getVR<F32>(code.va);
    Value<F32, 4> vc = getVR<F32>(code.vc);
    Value<F32, 4> vb = getVR<F32>(code.vb);
    Value<F32, 4> vd;

    // NOTE: vb-(va*vc) = -((va*vc)-vb)
    vd = builder.CreateFMul(va, vc);
    vd = builder.CreateFSub(vb, vd);

    setVR(code.vd, vd);
}

void Recompiler::vnor(Instruction code)
{
    Value<I32, 4> va = getVR<I32>(code.va);
    Value<I32, 4> vb = getVR<I32>(code.vb);
    Value<I32, 4> vd;

    vd = builder.CreateOr(va, vb);
    vd = builder.CreateNot(vd);

    setVR(code.vd, vd);
}

void Recompiler::vor(Instruction code)
{
    Value<I32, 4> va = getVR<I32>(code.va);
    Value<I32, 4> vb = getVR<I32>(code.vb);
    Value<I32, 4> vd;

    vd = builder.CreateOr(va, vb);

    setVR(code.vd, vd);
}

void Recompiler::vperm(Instruction code)
{
}

void Recompiler::vpkpx(Instruction code)
{
}

void Recompiler::vpkshss(Instruction code)
{
}

void Recompiler::vpkshus(Instruction code)
{
}

void Recompiler::vpkswss(Instruction code)
{
}

void Recompiler::vpkswus(Instruction code)
{
}

void Recompiler::vpkuhum(Instruction code)
{
}

void Recompiler::vpkuhus(Instruction code)
{
}

void Recompiler::vpkuwum(Instruction code)
{
}

void Recompiler::vpkuwus(Instruction code)
{
}

void Recompiler::vrefp(Instruction code)
{
}

void Recompiler::vrfim(Instruction code)
{
}

void Recompiler::vrfin(Instruction code)
{
}

void Recompiler::vrfip(Instruction code)
{
}

void Recompiler::vrfiz(Instruction code)
{
}

void Recompiler::vrlb(Instruction code)
{
}

void Recompiler::vrlh(Instruction code)
{
}

void Recompiler::vrlw(Instruction code)
{
}

void Recompiler::vrsqrtefp(Instruction code)
{
}

void Recompiler::vsel(Instruction code)
{
}

void Recompiler::vsl(Instruction code)
{
}

void Recompiler::vslb(Instruction code)
{
}

void Recompiler::vsldoi(Instruction code)
{
}

void Recompiler::vslh(Instruction code)
{
}

void Recompiler::vslo(Instruction code)
{
}

void Recompiler::vslw(Instruction code)
{
}

void Recompiler::vspltb(Instruction code)
{
}

void Recompiler::vsplth(Instruction code)
{
}

void Recompiler::vspltisb(Instruction code)
{
}

void Recompiler::vspltish(Instruction code)
{
}

void Recompiler::vspltisw(Instruction code)
{
}

void Recompiler::vspltw(Instruction code)
{
}

void Recompiler::vsr(Instruction code)
{
}

void Recompiler::vsrab(Instruction code)
{
}

void Recompiler::vsrah(Instruction code)
{
}

void Recompiler::vsraw(Instruction code)
{
}

void Recompiler::vsrb(Instruction code)
{
}

void Recompiler::vsrh(Instruction code)
{
}

void Recompiler::vsro(Instruction code)
{
}

void Recompiler::vsrw(Instruction code)
{
}

void Recompiler::vsubcuw(Instruction code)
{
}

void Recompiler::vsubfp(Instruction code)
{
}

void Recompiler::vsubsbs(Instruction code)
{
}

void Recompiler::vsubshs(Instruction code)
{
}

void Recompiler::vsubsws(Instruction code)
{
}

void Recompiler::vsububm(Instruction code)
{
}

void Recompiler::vsububs(Instruction code)
{
}

void Recompiler::vsubuhm(Instruction code)
{
}

void Recompiler::vsubuhs(Instruction code)
{
}

void Recompiler::vsubuwm(Instruction code)
{
}

void Recompiler::vsubuws(Instruction code)
{
}

void Recompiler::vsum2sws(Instruction code)
{
}

void Recompiler::vsum4sbs(Instruction code)
{
}

void Recompiler::vsum4shs(Instruction code)
{
}

void Recompiler::vsum4ubs(Instruction code)
{
}

void Recompiler::vsumsws(Instruction code)
{
}

void Recompiler::vupkhpx(Instruction code)
{
}

void Recompiler::vupkhsb(Instruction code)
{
}

void Recompiler::vupkhsh(Instruction code)
{
}

void Recompiler::vupklpx(Instruction code)
{
}

void Recompiler::vupklsb(Instruction code)
{
}

void Recompiler::vupklsh(Instruction code)
{
}

void Recompiler::vxor(Instruction code)
{
    Value<I32, 4> va = getVR<I32>(code.va);
    Value<I32, 4> vb = getVR<I32>(code.vb);
    Value<I32, 4> vd;

    vd = builder.CreateXor(va, vb);

    setVR(code.vd, vd);
}

}  // namespace ppu
}  // namespace cpu
