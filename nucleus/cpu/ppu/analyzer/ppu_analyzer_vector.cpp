/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_analyzer.h"

namespace cpu {
namespace ppu {

/**
 * PPC64 Vector/SIMD Instructions (aka AltiVec):
 *  - Vector UISA Instructions (Section: 4.2.x)
 *  - Vector VEA Instructions (Section: 4.3.x)
 */

void Analyzer::dss(Instruction code)
{
    // TODO: ?
}

void Analyzer::dst(Instruction code)
{
    // TODO: ?
}

void Analyzer::dstst(Instruction code)
{
    // TODO: ?
}

void Analyzer::lvebx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::lvehx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::lvewx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::lvlx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::lvlxl(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::lvrx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::lvrxl(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::lvsl(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::lvsr(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::lvx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::lvxl(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::mfvscr(Instruction code)
{
    setFlag(vscr, REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::mtvscr(Instruction code)
{
    setFlag(vr[code.vb], REG_READ);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::stvebx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(vr[code.vs], REG_READ);
}

void Analyzer::stvehx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(vr[code.vs], REG_READ);
}

void Analyzer::stvewx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(vr[code.vs], REG_READ);
}

void Analyzer::stvlx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(vr[code.vs], REG_READ);
}

void Analyzer::stvlxl(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(vr[code.vs], REG_READ);
}

void Analyzer::stvrx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(vr[code.vs], REG_READ);
}

void Analyzer::stvrxl(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(vr[code.vs], REG_READ);
}

void Analyzer::stvx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(vr[code.vs], REG_READ);
}

void Analyzer::stvxl(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(vr[code.vs], REG_READ);
}

void Analyzer::vaddcuw(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vaddfp(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vaddsbs(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vaddshs(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vaddsws(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vaddubm(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vaddubs(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vadduhm(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vadduhs(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vadduwm(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vadduws(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vand(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vandc(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vavgsb(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vavgsh(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vavgsw(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vavgub(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vavguh(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vavguw(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vcfsx(Instruction code)
{
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vcfux(Instruction code)
{
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vcmpbfp(Instruction code)
{
    setFlag(vscr, REG_READ);
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vcmpbfp_(Instruction code)
{
    vcmpbfp(code);
    setFlag(cr[6], REG_WRITE);
}

void Analyzer::vcmpeqfp(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vcmpeqfp_(Instruction code)
{
    vcmpeqfp(code);
    setFlag(cr[6], REG_WRITE);
}

void Analyzer::vcmpequb(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vcmpequb_(Instruction code)
{
    vcmpequb(code);
    setFlag(cr[6], REG_WRITE);
}

void Analyzer::vcmpequh(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vcmpequh_(Instruction code)
{
    vcmpequh(code);
    setFlag(cr[6], REG_WRITE);
}

void Analyzer::vcmpequw(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vcmpequw_(Instruction code)
{
    vcmpequw(code);
    setFlag(cr[6], REG_WRITE);
}

void Analyzer::vcmpgefp(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vcmpgefp_(Instruction code)
{
    vcmpgefp(code);
    setFlag(cr[6], REG_WRITE);
}

void Analyzer::vcmpgtfp(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vcmpgtfp_(Instruction code)
{
    vcmpgtfp(code);
    setFlag(cr[6], REG_WRITE);
}

void Analyzer::vcmpgtsb(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vcmpgtsb_(Instruction code)
{
    vcmpgtsb(code);
    setFlag(cr[6], REG_WRITE);
}

void Analyzer::vcmpgtsh(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vcmpgtsh_(Instruction code)
{
    vcmpgtsh(code);
    setFlag(cr[6], REG_WRITE);
}

void Analyzer::vcmpgtsw(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vcmpgtsw_(Instruction code)
{
    vcmpgtsw(code);
    setFlag(cr[6], REG_WRITE);
}

void Analyzer::vcmpgtub(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vcmpgtub_(Instruction code)
{
    vcmpgtub(code);
    setFlag(cr[6], REG_WRITE);
}

void Analyzer::vcmpgtuh(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vcmpgtuh_(Instruction code)
{
    vcmpgtuh(code);
    setFlag(cr[6], REG_WRITE);
}

void Analyzer::vcmpgtuw(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vcmpgtuw_(Instruction code)
{
    vcmpgtuw(code);
    setFlag(cr[6], REG_WRITE);
}

void Analyzer::vctsxs(Instruction code)
{
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vctuxs(Instruction code)
{
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vexptefp(Instruction code)
{
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vlogefp(Instruction code)
{
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmaddfp(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vc], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmaxfp(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmaxsb(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmaxsh(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmaxsw(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmaxub(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmaxuh(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmaxuw(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmhaddshs(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vc], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vmhraddshs(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vc], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vminfp(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vminsb(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vminsh(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vminsw(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vminub(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vminuh(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vminuw(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmladduhm(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vc], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmrghb(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmrghh(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmrghw(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmrglb(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmrglh(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmrglw(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmsummbm(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vc], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmsumshm(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vc], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmsumshs(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vc], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vmsumubm(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vc], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmsumuhm(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vc], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmsumuhs(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vc], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vmulesb(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmulesh(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmuleub(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmuleuh(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmulosb(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmulosh(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmuloub(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vmulouh(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vnmsubfp(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vc], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vnor(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vor(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vperm(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vc], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vpkpx(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vpkshss(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vpkshus(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vpkswss(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vpkswus(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vpkuhum(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vpkuhus(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vpkuwum(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vpkuwus(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vrefp(Instruction code)
{
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vrfim(Instruction code)
{
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vrfin(Instruction code)
{
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vrfip(Instruction code)
{
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vrfiz(Instruction code)
{
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vrlb(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vrlh(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vrlw(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vrsqrtefp(Instruction code)
{
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vsel(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vc], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vsl(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vslb(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vsldoi(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vslh(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vslo(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vslw(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vspltb(Instruction code)
{
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vsplth(Instruction code)
{
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vspltisb(Instruction code)
{
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vspltish(Instruction code)
{
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vspltisw(Instruction code)
{
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vspltw(Instruction code)
{
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vsr(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vsrab(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vsrah(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vsraw(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vsrb(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vsrh(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vsro(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vsrw(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vsubcuw(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vsubfp(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vsubsbs(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vsubshs(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vsubsws(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vsububm(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vsububs(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vsubuhm(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vsubuhs(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vsubuwm(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vsubuws(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vsum2sws(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vsum4sbs(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vsum4shs(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vsum4ubs(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vsumsws(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
    setFlag(vscr, REG_WRITE);
}

void Analyzer::vupkhpx(Instruction code)
{
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vupkhsb(Instruction code)
{
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vupkhsh(Instruction code)
{
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vupklpx(Instruction code)
{
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vupklsb(Instruction code)
{
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vupklsh(Instruction code)
{
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

void Analyzer::vxor(Instruction code)
{
    setFlag(vr[code.va], REG_READ);
    setFlag(vr[code.vb], REG_READ);
    setFlag(vr[code.vd], REG_WRITE);
}

}  // namespace ppu
}  // namespace cpu
