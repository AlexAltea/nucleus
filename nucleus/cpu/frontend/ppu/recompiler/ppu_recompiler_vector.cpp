/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"
#include "nucleus/assert.h"

namespace cpu {
namespace frontend {
namespace ppu {

using namespace cpu::hir;

/**
 * PPC64 Vector/SIMD Instructions (aka AltiVec):
 *  - Vector UISA Instructions (Section: 4.2.x)
 *  - Vector VEA Instructions (Section: 4.3.x)
 */

void Recompiler::dss(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::dst(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::dstst(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::lvebx(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::lvehx(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::lvewx(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::lvlx(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::lvlxl(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::lvrx(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::lvrxl(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::lvsl(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::lvsr(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::lvx(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::lvxl(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::mfvscr(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::mtvscr(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::stvebx(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::stvehx(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::stvewx(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::stvlx(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::stvlxl(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::stvrx(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::stvrxl(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::stvx(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::stvxl(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vaddcuw(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vaddfp(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAdd(va, vb, TYPE_F32);

    setVR(code.vd, vd);
}

void Recompiler::vaddsbs(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd = va; // TODO: This is wrong. Just a temporary fix

    // TODO: ? I8

    setVR(code.vd, vd);
}

void Recompiler::vaddshs(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd = va; // TODO: This is wrong. Just a temporary fix

    // TODO: ? I16

    setVR(code.vd, vd);
}

void Recompiler::vaddsws(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd = va; // TODO: This is wrong. Just a temporary fix

    // TODO: ? I32

    setVR(code.vd, vd);
}

void Recompiler::vaddubm(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAdd(va, vb, TYPE_I8);

    setVR(code.vd, vd);
}

void Recompiler::vaddubs(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd = va; // TODO: This is wrong. Just a temporary fix

    // TODO: ? I8

    setVR(code.vd, vd);
}

void Recompiler::vadduhm(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAdd(va, vb, TYPE_I16);

    setVR(code.vd, vd);
}

void Recompiler::vadduhs(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd = va; // TODO: This is wrong. Just a temporary fix

    // TODO: ? I16

    setVR(code.vd, vd);
}

void Recompiler::vadduwm(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAdd(va, vb, TYPE_I32);

    setVR(code.vd, vd);
}

void Recompiler::vadduws(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd = va; // TODO: This is wrong. Just a temporary fix

    // TODO: ? I32

    setVR(code.vd, vd);
}

void Recompiler::vand(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createAnd(va, vb);

    setVR(code.vd, vd);
}

void Recompiler::vandc(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vb = builder.createNot(vb);
    vd = builder.createAnd(va, vb);

    setVR(code.vd, vd);
}

void Recompiler::vavgsb(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAvg(va, vb, TYPE_I8);

    setVR(code.vd, vd);
}

void Recompiler::vavgsh(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAvg(va, vb, TYPE_I16);

    setVR(code.vd, vd);
}

void Recompiler::vavgsw(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAvg(va, vb, TYPE_I32);

    setVR(code.vd, vd);
}

void Recompiler::vavgub(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAvg(va, vb, TYPE_I8, ARITHMETIC_UNSIGNED);

    setVR(code.vd, vd);
}

void Recompiler::vavguh(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAvg(va, vb, TYPE_I16, ARITHMETIC_UNSIGNED);

    setVR(code.vd, vd);
}

void Recompiler::vavguw(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAvg(va, vb, TYPE_I32, ARITHMETIC_UNSIGNED);

    setVR(code.vd, vd);
}

void Recompiler::vcfsx(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vcfux(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vcmpbfp(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vcmpbfp_(Instruction code)
{
    vcmpbfp(code);
    // TODO: CR6 update
}

void Recompiler::vcmpeqfp(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpEQ(va, vb, TYPE_F32);
    auto vd = builder.createSExt(result, TYPE_I32);

    setVR(code.vd, vd);
}

void Recompiler::vcmpeqfp_(Instruction code)
{
    vcmpeqfp(code);
    // TODO: CR6 update
}

void Recompiler::vcmpequb(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpEQ(va, vb, TYPE_I8);
    auto vd = builder.createSExt(result, TYPE_I8);

    setVR(code.vd, vd);
}

void Recompiler::vcmpequb_(Instruction code)
{
    vcmpequb(code);
    // TODO: CR6 update
}

void Recompiler::vcmpequh(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpEQ(va, vb, TYPE_I16);
    auto vd = builder.createSExt(result, TYPE_I16);

    setVR(code.vd, vd);
}

void Recompiler::vcmpequh_(Instruction code)
{
    vcmpequh(code);
    // TODO: CR6 update
}

void Recompiler::vcmpequw(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpEQ(va, vb, TYPE_I32);
    auto vd = builder.createSExt(result, TYPE_I32);

    setVR(code.vd, vd);
}

void Recompiler::vcmpequw_(Instruction code)
{
    vcmpequw(code);
    // TODO: CR6 update
}

void Recompiler::vcmpgefp(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpFGE(va, vb, TYPE_F32);
    auto vd = builder.createSExt(result, TYPE_I32);

    setVR(code.vd, vd);
}

void Recompiler::vcmpgefp_(Instruction code)
{
    vcmpgefp(code);
    // TODO: CR6 update
}

void Recompiler::vcmpgtfp(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpFGT(va, vb, TYPE_F32);
    auto vd = builder.createSExt(result, TYPE_I32);

    setVR(code.vd, vd);
}

void Recompiler::vcmpgtfp_(Instruction code)
{
    vcmpgtfp(code);
    // TODO: CR6 update
}

void Recompiler::vcmpgtsb(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpSGT(va, vb, TYPE_I8);
    auto vd = builder.createSExt(result, TYPE_I8);

    setVR(code.vd, vd);
}

void Recompiler::vcmpgtsb_(Instruction code)
{
    vcmpgtsb(code);
    // TODO: CR6 update
}

void Recompiler::vcmpgtsh(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpSGT(va, vb, TYPE_I16);
    auto vd = builder.createSExt(result, TYPE_I16);

    setVR(code.vd, vd);
}

void Recompiler::vcmpgtsh_(Instruction code)
{
    vcmpgtsh(code);
    // TODO: CR6 update
}

void Recompiler::vcmpgtsw(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpSGT(va, vb, TYPE_I32);
    auto vd = builder.createSExt(result, TYPE_I32);

    setVR(code.vd, vd);
}

void Recompiler::vcmpgtsw_(Instruction code)
{
    vcmpgtsw(code);
    // TODO: CR6 update
}

void Recompiler::vcmpgtub(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpUGT(va, vb, TYPE_I8);
    auto vd = builder.createSExt(result, TYPE_I8);

    setVR(code.vd, vd);
}

void Recompiler::vcmpgtub_(Instruction code)
{
    vcmpgtub(code);
    // TODO: CR6 update
}

void Recompiler::vcmpgtuh(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpUGT(va, vb, TYPE_I16);
    auto vd = builder.createSExt(result, TYPE_I16);

    setVR(code.vd, vd);
}

void Recompiler::vcmpgtuh_(Instruction code)
{
    vcmpgtuh(code);
    // TODO: CR6 update
}

void Recompiler::vcmpgtuw(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpUGT(va, vb, TYPE_I32);
    auto vd = builder.createSExt(result, TYPE_I32);

    setVR(code.vd, vd);
}

void Recompiler::vcmpgtuw_(Instruction code)
{
    vcmpgtuw(code);
    // TODO: CR6 update
}

void Recompiler::vctsxs(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vctuxs(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vexptefp(Instruction code)
{
    Value* vb = getVR(code.vb);
    Value* vd = vb; // TODO: This is wrong. Just a temporary fix

    //llvm::Function* exp2 = getIntrinsicFloat(llvm::Intrinsic::exp2); // TODO: Get a 4xf32 intrinsic instead
    //vd = builder.createCall(exp2, vb);

    setVR(code.vd, vd);
}

void Recompiler::vlogefp(Instruction code)
{
    Value* vb = getVR(code.vb);
    Value* vd = vb; // TODO: This is wrong. Just a temporary fix

    //llvm::Function* log2 = getIntrinsicFloat(llvm::Intrinsic::log2); // TODO: Get a 4xf32 intrinsic instead
    //vd = builder.createCall(log2, vb);

    setVR(code.vd, vd);
}

void Recompiler::vmaddfp(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vc = getVR(code.vc);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVMul(va, vc, TYPE_F32);
    vd = builder.createVAdd(vd, vb, TYPE_F32);

    setVR(code.vd, vd);
}

void Recompiler::vmaxfp(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmaxsb(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmaxsh(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmaxsw(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmaxub(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmaxuh(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmaxuw(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmhaddshs(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmhraddshs(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vminfp(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vminsb(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vminsh(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vminsw(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vminub(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vminuh(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vminuw(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmladduhm(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmrghb(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmrghh(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmrghw(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmrglb(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmrglh(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmrglw(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmsummbm(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmsumshm(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmsumshs(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmsumubm(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmsumuhm(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmsumuhs(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmulesb(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmulesh(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmuleub(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmuleuh(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmulosb(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmulosh(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmuloub(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vmulouh(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vnmsubfp(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vc = getVR(code.vc);
    Value* vb = getVR(code.vb);
    Value* vd;

    // NOTE: vb-(va*vc) = -((va*vc)-vb)
    vd = builder.createVMul(va, vc, TYPE_F32);
    vd = builder.createVSub(vb, vd, TYPE_F32);

    setVR(code.vd, vd);
}

void Recompiler::vnor(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createOr(va, vb);
    vd = builder.createNot(vd);

    setVR(code.vd, vd);
}

void Recompiler::vor(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createOr(va, vb);

    setVR(code.vd, vd);
}

void Recompiler::vperm(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vpkpx(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vpkshss(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vpkshus(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vpkswss(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vpkswus(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vpkuhum(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vpkuhus(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vpkuwum(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vpkuwus(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vrefp(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vrfim(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vrfin(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vrfip(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vrfiz(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vrlb(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vrlh(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vrlw(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vrsqrtefp(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsel(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsl(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vslb(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsldoi(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vslh(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vslo(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vslw(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vspltb(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsplth(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vspltisb(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vspltish(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vspltisw(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vspltw(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsr(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsrab(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsrah(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsraw(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsrb(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsrh(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsro(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsrw(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsubcuw(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsubfp(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsubsbs(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsubshs(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsubsws(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsububm(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsububs(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsubuhm(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsubuhs(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsubuwm(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsubuws(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsum2sws(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsum4sbs(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsum4shs(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsum4ubs(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vsumsws(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vupkhpx(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vupkhsb(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vupkhsh(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vupklpx(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vupklsb(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vupklsh(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::vxor(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createXor(va, vb);

    setVR(code.vd, vd);
}

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
