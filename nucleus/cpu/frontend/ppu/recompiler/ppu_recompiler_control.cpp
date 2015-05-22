/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"

namespace cpu {
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
}

void Recompiler::mtocrf(Instruction code)
{
}

void Recompiler::mtspr(Instruction code)
{
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
}  // namespace cpu
