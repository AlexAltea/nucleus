/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_analyzer.h"

namespace cpu {
namespace frontend {
namespace ppu {

/**
 * PPC64 Instructions:
 *  - UISA: Branch and Flow Control Instructions (Section: 4.2.4)
 */

void Analyzer::bx(Instruction code)
{
    // TODO: ?
}

void Analyzer::bcx(Instruction code)
{
    // TODO: ?
}

void Analyzer::bcctrx(Instruction code)
{
    // TODO: ?
}

void Analyzer::bclrx(Instruction code)
{
    // TODO: ?
}

void Analyzer::crand(Instruction code)
{
    setFlag(cr[code.crba / 4], REG_READ);
    setFlag(cr[code.crbb / 4], REG_READ);
    setFlag(cr[code.crbd / 4], REG_WRITE);
}

void Analyzer::crandc(Instruction code)
{
    setFlag(cr[code.crba / 4], REG_READ);
    setFlag(cr[code.crbb / 4], REG_READ);
    setFlag(cr[code.crbd / 4], REG_WRITE);
}

void Analyzer::creqv(Instruction code)
{
    setFlag(cr[code.crba / 4], REG_READ);
    setFlag(cr[code.crbb / 4], REG_READ);
    setFlag(cr[code.crbd / 4], REG_WRITE);
}

void Analyzer::crnand(Instruction code)
{
    setFlag(cr[code.crba / 4], REG_READ);
    setFlag(cr[code.crbb / 4], REG_READ);
    setFlag(cr[code.crbd / 4], REG_WRITE);
}

void Analyzer::crnor(Instruction code)
{
    setFlag(cr[code.crba / 4], REG_READ);
    setFlag(cr[code.crbb / 4], REG_READ);
    setFlag(cr[code.crbd / 4], REG_WRITE);
}

void Analyzer::cror(Instruction code)
{
    setFlag(cr[code.crba / 4], REG_READ);
    setFlag(cr[code.crbb / 4], REG_READ);
    setFlag(cr[code.crbd / 4], REG_WRITE);
}

void Analyzer::crorc(Instruction code)
{
    setFlag(cr[code.crba / 4], REG_READ);
    setFlag(cr[code.crbb / 4], REG_READ);
    setFlag(cr[code.crbd / 4], REG_WRITE);
}

void Analyzer::crxor(Instruction code)
{
    setFlag(cr[code.crba / 4], REG_READ);
    setFlag(cr[code.crbb / 4], REG_READ);
    setFlag(cr[code.crbd / 4], REG_WRITE);
}

void Analyzer::mcrf(Instruction code)
{
    setFlag(cr[code.crfs], REG_READ);
    setFlag(cr[code.crfd], REG_WRITE);
}

void Analyzer::sc(Instruction code)
{
    switch (code.lev) {
    case 0: // LV2
        setFlag(gpr[11], REG_READ);
        break;
    case 1: // LV1
        setFlag(gpr[11], REG_READ);
        break;
    default:
        break;
    }
}

void Analyzer::td(Instruction code)
{
    // TODO: ?
}

void Analyzer::tdi(Instruction code)
{
    // TODO: ?
}

void Analyzer::tw(Instruction code)
{
    // TODO: ?
}

void Analyzer::twi(Instruction code)
{
    // TODO: ?
}

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
