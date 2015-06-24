/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_analyzer.h"

namespace cpu {
namespace ppu {

/**
 * PPC64 Instructions:
 *  - UISA: Floating-Point Instructions (Section: 4.2.2)
 */

void Analyzer::fabsx(Instruction code)
{
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::faddx(Instruction code)
{
    setFlag(fpr[code.fra], REG_READ);
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::faddsx(Instruction code)
{
    setFlag(fpr[code.fra], REG_READ);
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fcfidx(Instruction code)
{
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fcmpo(Instruction code)
{
    setFlag(fpr[code.fra], REG_READ);
    setFlag(fpr[code.frb], REG_READ);
    setFlag(cr[code.crfd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);
}

void Analyzer::fcmpu(Instruction code)
{
    setFlag(fpr[code.fra], REG_READ);
    setFlag(fpr[code.frb], REG_READ);
    setFlag(cr[code.crfd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);
}

void Analyzer::fctidx(Instruction code)
{
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_READ);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fctidzx(Instruction code)
{
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_READ);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fctiwx(Instruction code)
{
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_READ);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fctiwzx(Instruction code)
{
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_READ);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fdivx(Instruction code)
{
    setFlag(fpr[code.fra], REG_READ);
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fdivsx(Instruction code)
{
    setFlag(fpr[code.fra], REG_READ);
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fmaddx(Instruction code)
{
    setFlag(fpr[code.fra], REG_READ);
    setFlag(fpr[code.frc], REG_READ);
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fmaddsx(Instruction code)
{
    setFlag(fpr[code.fra], REG_READ);
    setFlag(fpr[code.frc], REG_READ);
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fmrx(Instruction code)
{
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fmsubx(Instruction code)
{
    setFlag(fpr[code.fra], REG_READ);
    setFlag(fpr[code.frc], REG_READ);
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fmsubsx(Instruction code)
{
    setFlag(fpr[code.fra], REG_READ);
    setFlag(fpr[code.frc], REG_READ);
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fmulx(Instruction code)
{
    setFlag(fpr[code.fra], REG_READ);
    setFlag(fpr[code.frc], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fmulsx(Instruction code)
{
    setFlag(fpr[code.fra], REG_READ);
    setFlag(fpr[code.frc], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fnabsx(Instruction code)
{
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fnegx(Instruction code)
{
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fnmaddx(Instruction code)
{
    setFlag(fpr[code.fra], REG_READ);
    setFlag(fpr[code.frc], REG_READ);
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fnmaddsx(Instruction code)
{
    setFlag(fpr[code.fra], REG_READ);
    setFlag(fpr[code.frc], REG_READ);
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fnmsubx(Instruction code)
{
    setFlag(fpr[code.fra], REG_READ);
    setFlag(fpr[code.frc], REG_READ);
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fnmsubsx(Instruction code)
{
    setFlag(fpr[code.fra], REG_READ);
    setFlag(fpr[code.frc], REG_READ);
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fresx(Instruction code)
{
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::frspx(Instruction code)
{
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_READ);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::frsqrtex(Instruction code)
{
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fselx(Instruction code)
{
    setFlag(fpr[code.fra], REG_READ);
    setFlag(fpr[code.frc], REG_READ);
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fsqrtx(Instruction code)
{
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fsqrtsx(Instruction code)
{
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fsubx(Instruction code)
{
    setFlag(fpr[code.fra], REG_READ);
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::fsubsx(Instruction code)
{
    setFlag(fpr[code.fra], REG_READ);
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::mcrfs(Instruction code)
{
    // TODO: Set only the potential modifiable CR fields based on the mask
    for (int i = 0; i < 8; i++) {
        setFlag(cr[i], REG_WRITE);
    }
}

void Analyzer::mffsx(Instruction code)
{
    setFlag(fpscr, REG_READ);
    setFlag(fpr[code.frd], REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::mtfsb0x(Instruction code)
{
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::mtfsb1x(Instruction code)
{
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::mtfsfix(Instruction code)
{
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

void Analyzer::mtfsfx(Instruction code)
{
    setFlag(fpr[code.frb], REG_READ);
    setFlag(fpscr, REG_READ);
    setFlag(fpscr, REG_WRITE);

    if (code.rc) {
        setFlag(cr[1], REG_WRITE);
    }
}

}  // namespace ppu
}  // namespace cpu
