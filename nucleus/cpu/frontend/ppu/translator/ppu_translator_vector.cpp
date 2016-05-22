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
 * PPC64 Vector/SIMD Instructions (aka AltiVec):
 *  - Vector UISA Instructions (Section: 4.2.x)
 *  - Vector VEA Instructions (Section: 4.3.x)
 */

void Translator::dss(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::dst(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::dstst(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lvebx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lvehx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lvewx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lvlx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lvlxl(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lvrx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lvrxl(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lvsl(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lvsr(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lvx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* vd;

    Value* addr = rb;
    if (code.ra) {
        addr = builder.createAdd(addr, ra);
    }

    vd = readMemory(addr, TYPE_V128);
    setVR(code.vd, vd);
}

void Translator::lvxl(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::mfvscr(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::mtvscr(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::stvebx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::stvehx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::stvewx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::stvlx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::stvlxl(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::stvrx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::stvrxl(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::stvx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* vs = getGPR(code.vs);

    Value* addr = rb;
    if (code.ra) {
        addr = builder.createAdd(addr, ra);
    }

    writeMemory(addr, vs);
}

void Translator::stvxl(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vaddcuw(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vaddfp(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAdd(va, vb, COMPONENT_F32);

    setVR(code.vd, vd);
}

void Translator::vaddsbs(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd = va; // TODO: This is wrong. Just a temporary fix

    // TODO: ? I8

    setVR(code.vd, vd);
}

void Translator::vaddshs(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd = va; // TODO: This is wrong. Just a temporary fix

    // TODO: ? I16

    setVR(code.vd, vd);
}

void Translator::vaddsws(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd = va; // TODO: This is wrong. Just a temporary fix

    // TODO: ? I32

    setVR(code.vd, vd);
}

void Translator::vaddubm(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAdd(va, vb, COMPONENT_I8);

    setVR(code.vd, vd);
}

void Translator::vaddubs(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd = va; // TODO: This is wrong. Just a temporary fix

    vd = builder.createVAdd(va, vb, COMPONENT_I8 | ARITHMETIC_UNSIGNED | ARITHMETIC_SATURATE);
    // TODO: ? I8

    setVR(code.vd, vd);
}

void Translator::vadduhm(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAdd(va, vb, COMPONENT_I16);

    setVR(code.vd, vd);
}

void Translator::vadduhs(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd = va; // TODO: This is wrong. Just a temporary fix

    // TODO: ? I16

    setVR(code.vd, vd);
}

void Translator::vadduwm(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAdd(va, vb, COMPONENT_I32);

    setVR(code.vd, vd);
}

void Translator::vadduws(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd = va; // TODO: This is wrong. Just a temporary fix

    // TODO: ? I32

    setVR(code.vd, vd);
}

void Translator::vand(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createAnd(va, vb);

    setVR(code.vd, vd);
}

void Translator::vandc(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vb = builder.createNot(vb);
    vd = builder.createAnd(va, vb);

    setVR(code.vd, vd);
}

void Translator::vavgsb(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAvg(va, vb, COMPONENT_I8);

    setVR(code.vd, vd);
}

void Translator::vavgsh(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAvg(va, vb, COMPONENT_I16);

    setVR(code.vd, vd);
}

void Translator::vavgsw(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAvg(va, vb, COMPONENT_I32);

    setVR(code.vd, vd);
}

void Translator::vavgub(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAvg(va, vb, COMPONENT_I8 | ARITHMETIC_UNSIGNED);

    setVR(code.vd, vd);
}

void Translator::vavguh(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAvg(va, vb, COMPONENT_I16 | ARITHMETIC_UNSIGNED);

    setVR(code.vd, vd);
}

void Translator::vavguw(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVAvg(va, vb, COMPONENT_I32 | ARITHMETIC_UNSIGNED);

    setVR(code.vd, vd);
}

void Translator::vcfsx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vcfux(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vcmpbfp(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vcmpbfp_(Instruction code)
{
    vcmpbfp(code);
    // TODO: CR6 update
}

void Translator::vcmpeqfp(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpEQ(va, vb, COMPONENT_F32);
    auto vd = builder.createSExt(result, TYPE_I32);

    setVR(code.vd, vd);
}

void Translator::vcmpeqfp_(Instruction code)
{
    vcmpeqfp(code);
    // TODO: CR6 update
}

void Translator::vcmpequb(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpEQ(va, vb, COMPONENT_I8);
    auto vd = builder.createSExt(result, TYPE_I8);

    setVR(code.vd, vd);
}

void Translator::vcmpequb_(Instruction code)
{
    vcmpequb(code);
    // TODO: CR6 update
}

void Translator::vcmpequh(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpEQ(va, vb, COMPONENT_I16);
    auto vd = builder.createSExt(result, TYPE_I16);

    setVR(code.vd, vd);
}

void Translator::vcmpequh_(Instruction code)
{
    vcmpequh(code);
    // TODO: CR6 update
}

void Translator::vcmpequw(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpEQ(va, vb, COMPONENT_I32);
    auto vd = builder.createSExt(result, TYPE_I32);

    setVR(code.vd, vd);
}

void Translator::vcmpequw_(Instruction code)
{
    vcmpequw(code);
    // TODO: CR6 update
}

void Translator::vcmpgefp(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpFGE(va, vb, COMPONENT_F32);
    auto vd = builder.createSExt(result, TYPE_I32);

    setVR(code.vd, vd);
}

void Translator::vcmpgefp_(Instruction code)
{
    vcmpgefp(code);
    // TODO: CR6 update
}

void Translator::vcmpgtfp(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpFGT(va, vb, COMPONENT_F32);
    auto vd = builder.createSExt(result, TYPE_I32);

    setVR(code.vd, vd);
}

void Translator::vcmpgtfp_(Instruction code)
{
    vcmpgtfp(code);
    // TODO: CR6 update
}

void Translator::vcmpgtsb(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpSGT(va, vb, COMPONENT_I8);
    auto vd = builder.createSExt(result, TYPE_I8);

    setVR(code.vd, vd);
}

void Translator::vcmpgtsb_(Instruction code)
{
    vcmpgtsb(code);
    // TODO: CR6 update
}

void Translator::vcmpgtsh(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpSGT(va, vb, COMPONENT_I16);
    auto vd = builder.createSExt(result, TYPE_I16);

    setVR(code.vd, vd);
}

void Translator::vcmpgtsh_(Instruction code)
{
    vcmpgtsh(code);
    // TODO: CR6 update
}

void Translator::vcmpgtsw(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpSGT(va, vb, COMPONENT_I32);
    auto vd = builder.createSExt(result, TYPE_I32);

    setVR(code.vd, vd);
}

void Translator::vcmpgtsw_(Instruction code)
{
    vcmpgtsw(code);
    // TODO: CR6 update
}

void Translator::vcmpgtub(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpUGT(va, vb, COMPONENT_I8);
    auto vd = builder.createSExt(result, TYPE_I8);

    setVR(code.vd, vd);
}

void Translator::vcmpgtub_(Instruction code)
{
    vcmpgtub(code);
    // TODO: CR6 update
}

void Translator::vcmpgtuh(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpUGT(va, vb, COMPONENT_I16);
    auto vd = builder.createSExt(result, TYPE_I16);

    setVR(code.vd, vd);
}

void Translator::vcmpgtuh_(Instruction code)
{
    vcmpgtuh(code);
    // TODO: CR6 update
}

void Translator::vcmpgtuw(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);

    auto result = builder.createVCmpUGT(va, vb, COMPONENT_I32);
    auto vd = builder.createSExt(result, TYPE_I32);

    setVR(code.vd, vd);
}

void Translator::vcmpgtuw_(Instruction code)
{
    vcmpgtuw(code);
    // TODO: CR6 update
}

void Translator::vctsxs(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vctuxs(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vexptefp(Instruction code)
{
    Value* vb = getVR(code.vb);
    Value* vd = vb; // TODO: This is wrong. Just a temporary fix

    //llvm::Function* exp2 = getIntrinsicFloat(llvm::Intrinsic::exp2); // TODO: Get a 4xf32 intrinsic instead
    //vd = builder.createCall(exp2, vb);

    setVR(code.vd, vd);
}

void Translator::vlogefp(Instruction code)
{
    Value* vb = getVR(code.vb);
    Value* vd = vb; // TODO: This is wrong. Just a temporary fix

    //llvm::Function* log2 = getIntrinsicFloat(llvm::Intrinsic::log2); // TODO: Get a 4xf32 intrinsic instead
    //vd = builder.createCall(log2, vb);

    setVR(code.vd, vd);
}

void Translator::vmaddfp(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vc = getVR(code.vc);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createVMul(va, vc, COMPONENT_F32);
    vd = builder.createVAdd(vd, vb, COMPONENT_F32);

    setVR(code.vd, vd);
}

void Translator::vmaxfp(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmaxsb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmaxsh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmaxsw(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmaxub(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmaxuh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmaxuw(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmhaddshs(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmhraddshs(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vminfp(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vminsb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vminsh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vminsw(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vminub(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vminuh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vminuw(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmladduhm(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmrghb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmrghh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmrghw(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmrglb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmrglh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmrglw(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmsummbm(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmsumshm(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmsumshs(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmsumubm(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmsumuhm(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmsumuhs(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmulesb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmulesh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmuleub(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmuleuh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmulosb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmulosh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmuloub(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vmulouh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vnmsubfp(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vc = getVR(code.vc);
    Value* vb = getVR(code.vb);
    Value* vd;

    // NOTE: vb-(va*vc) = -((va*vc)-vb)
    vd = builder.createVMul(va, vc, COMPONENT_F32);
    vd = builder.createVSub(vb, vd, COMPONENT_F32);

    setVR(code.vd, vd);
}

void Translator::vnor(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createOr(va, vb);
    vd = builder.createNot(vd);

    setVR(code.vd, vd);
}

void Translator::vor(Instruction code)
{
    Value* va = getVR(code.va);
    Value* vb = getVR(code.vb);
    Value* vd;

    vd = builder.createOr(va, vb);

    setVR(code.vd, vd);
}

void Translator::vperm(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vpkpx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vpkshss(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vpkshus(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vpkswss(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vpkswus(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vpkuhum(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vpkuhus(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vpkuwum(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vpkuwus(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vrefp(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vrfim(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vrfin(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vrfip(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vrfiz(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vrlb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vrlh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vrlw(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vrsqrtefp(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsel(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsl(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vslb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsldoi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vslh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vslo(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vslw(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vspltb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsplth(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vspltisb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vspltish(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vspltisw(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vspltw(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsr(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsrab(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsrah(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsraw(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsrb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsrh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsro(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsrw(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsubcuw(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsubfp(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsubsbs(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsubshs(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsubsws(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsububm(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsububs(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsubuhm(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsubuhs(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsubuwm(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsubuws(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsum2sws(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsum4sbs(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsum4shs(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsum4ubs(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vsumsws(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vupkhpx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vupkhsb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vupkhsh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vupklpx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vupklsb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vupklsh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::vxor(Instruction code)
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
