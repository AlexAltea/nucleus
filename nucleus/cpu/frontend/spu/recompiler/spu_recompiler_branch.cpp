/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "spu_recompiler.h"

namespace cpu {
namespace spu {

using namespace cpu::hir;

/**
 * SPU Instructions:
 *  - Compare, Branch and Halt Instructions (Chapter 7)
 *  - Hint-for-Branch Instructions (Chapter 8)
 */

// Compare, Branch and Halt Instructions (Chapter 7)
void Recompiler::bi(Instruction code)
{
}

void Recompiler::bihnz(Instruction code)
{
}

void Recompiler::bihz(Instruction code)
{
}

void Recompiler::binz(Instruction code)
{
}

void Recompiler::bisl(Instruction code)
{
}

void Recompiler::bisled(Instruction code)
{
}

void Recompiler::biz(Instruction code)
{
}

void Recompiler::br(Instruction code)
{
}

void Recompiler::bra(Instruction code)
{
}

void Recompiler::brasl(Instruction code)
{
}

void Recompiler::brhnz(Instruction code)
{
}

void Recompiler::brhz(Instruction code)
{
}

void Recompiler::brnz(Instruction code)
{
}

void Recompiler::brsl(Instruction code)
{
}

void Recompiler::brz(Instruction code)
{
}

void Recompiler::ceq(Instruction code)
{
}

void Recompiler::ceqb(Instruction code)
{
}

void Recompiler::ceqbi(Instruction code)
{
}

void Recompiler::ceqh(Instruction code)
{
}

void Recompiler::ceqhi(Instruction code)
{
}

void Recompiler::ceqi(Instruction code)
{
}

void Recompiler::cgt(Instruction code)
{
}

void Recompiler::cgtb(Instruction code)
{
}

void Recompiler::cgtbi(Instruction code)
{
}

void Recompiler::cgth(Instruction code)
{
}

void Recompiler::cgthi(Instruction code)
{
}

void Recompiler::cgti(Instruction code)
{
}

void Recompiler::clgt(Instruction code)
{
}

void Recompiler::clgtb(Instruction code)
{
}

void Recompiler::clgtbi(Instruction code)
{
}

void Recompiler::clgth(Instruction code)
{
}

void Recompiler::clgthi(Instruction code)
{
}

void Recompiler::clgti(Instruction code)
{
}

void Recompiler::heq(Instruction code)
{
}

void Recompiler::heqi(Instruction code)
{
}

void Recompiler::hgt(Instruction code)
{
}

void Recompiler::hgti(Instruction code)
{
}

void Recompiler::hlgt(Instruction code)
{
}

void Recompiler::hlgti(Instruction code)
{
}

void Recompiler::iret(Instruction code)
{
}


// Hint-for-Branch Instructions (Chapter 8)
void Recompiler::hbr(Instruction code)
{
}

void Recompiler::hbra(Instruction code)
{
}

void Recompiler::hbrr(Instruction code)
{
}

}  // namespace spu
}  // namespace cpu
