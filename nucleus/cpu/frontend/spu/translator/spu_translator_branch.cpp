/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "spu_recompiler.h"
#include "nucleus/assert.h"

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
    assert_always("Unimplemented");
}

void Recompiler::bihnz(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::bihz(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::binz(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::bisl(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::bisled(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::biz(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::br(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::bra(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::brasl(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::brhnz(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::brhz(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::brnz(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::brsl(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::brz(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::ceq(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::ceqb(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::ceqbi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::ceqh(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::ceqhi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::ceqi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::cgt(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::cgtb(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::cgtbi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::cgth(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::cgthi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::cgti(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::clgt(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::clgtb(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::clgtbi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::clgth(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::clgthi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::clgti(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::heq(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::heqi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::hgt(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::hgti(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::hlgt(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::hlgti(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::iret(Instruction code)
{
    assert_always("Unimplemented");
}


// Hint-for-Branch Instructions (Chapter 8)
void Recompiler::hbr(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::hbra(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::hbrr(Instruction code)
{
    assert_always("Unimplemented");
}

}  // namespace spu
}  // namespace cpu
