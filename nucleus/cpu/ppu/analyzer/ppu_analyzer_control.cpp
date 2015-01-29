/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_analyzer.h"

namespace cpu {
namespace ppu {

/**
 * PPC64 Instructions:
 *  - UISA: Processor Control Instructions (Section: 4.2.5)
 *  - VEA: Processor Control Instructions (Section: 4.3.1)
 *  - VEA: Memory Control Instructions (Section: 4.3.3)
 *  - VEA: External Control Instructions (Section: 4.3.4)
 */


void Analyzer::mfocrf(Instruction code)
{
    for (int i = 0; i < 8; i++) {
        setFlag(cr[i], REG_READ);
    }
    setFlag(gpr[code.rd], REG_WRITE);
}

void Analyzer::mfspr(Instruction code)
{
    const u32 n = (code.spr >> 5) | ((code.spr & 0x1F) << 5);
    switch (n) {
    case 0x001: setFlag(xer, REG_READ); break;
    case 0x008: setFlag(lr,  REG_READ); break;
    case 0x009: setFlag(ctr, REG_READ); break;
    }
    setFlag(gpr[code.rd], REG_WRITE);
}

void Analyzer::mtocrf(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    if (code.l11) {
        u32 n = 0, count = 0;
        for (int i = 0; i < 8; i++) {
            if (code.crm & (1 << i)) {
                n = i;
                count++;
            }
        }
        if (count == 1) {
            setFlag(cr[7 - n], REG_WRITE);
        }
        else {
            for (int i = 0; i < 8; i++) {
                setFlag(cr[i], REG_WRITE);
            }
        }
    }
    else {
        for (int i = 0; i < 8; i++) {
            setFlag(cr[i], REG_WRITE);
        }
    }
}

void Analyzer::mtspr(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    const u32 n = (code.spr >> 5) | ((code.spr & 0x1F) << 5);
    switch (n) {
    case 0x001: setFlag(xer, REG_WRITE); break;
    case 0x008: setFlag(lr,  REG_WRITE); break;
    case 0x009: setFlag(ctr, REG_WRITE); break;
    }
}

void Analyzer::mftb(Instruction code)
{
    setFlag(tb, REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
}

void Analyzer::dcbf(Instruction code)
{
    // TODO: ?
}

void Analyzer::dcbst(Instruction code)
{
    // TODO: ?
}

void Analyzer::dcbt(Instruction code)
{
    // TODO: ?
}

void Analyzer::dcbtst(Instruction code)
{
    // TODO: ?
}

void Analyzer::dcbz(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    // TODO: ?
}

void Analyzer::icbi(Instruction code)
{
    // TODO: ?
}

void Analyzer::eciwx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
}

void Analyzer::ecowx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rs], REG_READ);
}

}  // namespace ppu
}  // namespace cpu
