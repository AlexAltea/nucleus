/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"
#include "nucleus/emulator.h"
#include "nucleus/logger/logger.h"

namespace cpu {
namespace frontend {
namespace ppu {

using namespace cpu::hir;

/**
 * PPC64 Instructions:
 *  - UISA: Processor Control Instructions (Section: 4.2.5)
 *  - VEA: Processor Control Instructions (Section: 4.3.1)
 *  - VEA: Memory Control Instructions (Section: 4.3.3)
 *  - VEA: External Control Instructions (Section: 4.3.4)
 */

void Recompiler::mfocrf(Instruction code)
{
}

void Recompiler::mfspr(Instruction code)
{
    Value* rd;

    const U32 n = (code.spr >> 5) | ((code.spr & 0x1F) << 5);
    switch (n) {
    case 0x001: // XER register
        rd = getXER();
        break;
    case 0x009: // CTR register
        rd = getCTR();
        break;

    // Registers ignored by the recompiler
    case 0x008: // LR register
        rd = builder.getConstantI64(0);
        break;

    default:
        logger.error(LOG_CPU, "Recompiler::mfspr error: Unknown SPR");
    }

    setGPR(code.rd, rd);
}

void Recompiler::mtocrf(Instruction code)
{
}

void Recompiler::mtspr(Instruction code)
{
    Value* rs = getGPR(code.rs);

    const U32 n = (code.spr >> 5) | ((code.spr & 0x1F) << 5);
    switch (n) {
    case 0x001: // XER register
        setXER(rs);
        break;
    case 0x009: // CTR register
        setCTR(rs);
        break;

    // Registers ignored by the recompiler
    case 0x008: // LR register
        break;

    default:
        logger.error(LOG_CPU, "Recompiler::mtspr error: Unknown SPR");
    }
}

void Recompiler::mftb(Instruction code)
{
}

void Recompiler::dcbf(Instruction code)
{
}

void Recompiler::dcbst(Instruction code)
{
}

void Recompiler::dcbt(Instruction code)
{
}

void Recompiler::dcbtst(Instruction code)
{
}

void Recompiler::dcbz(Instruction code)
{
}

void Recompiler::icbi(Instruction code)
{
}

void Recompiler::eciwx(Instruction code)
{
}

void Recompiler::ecowx(Instruction code)
{
}

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
