/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "spu_translator.h"
#include "nucleus/assert.h"

namespace cpu {
namespace frontend {
namespace spu {

using namespace cpu::hir;

/**
 * SPU Instructions:
 *  - Compare, Branch and Halt Instructions (Chapter 7)
 *  - Hint-for-Branch Instructions (Chapter 8)
 */

// Compare, Branch and Halt Instructions (Chapter 7)
void Translator::bi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::bihnz(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::bihz(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::binz(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::bisl(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::bisled(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::biz(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::br(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::bra(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::brasl(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::brhnz(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::brhz(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::brnz(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::brsl(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::brz(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::ceq(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::ceqb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::ceqbi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::ceqh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::ceqhi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::ceqi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::cgt(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::cgtb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::cgtbi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::cgth(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::cgthi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::cgti(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::clgt(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::clgtb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::clgtbi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::clgth(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::clgthi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::clgti(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::heq(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::heqi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::hgt(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::hgti(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::hlgt(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::hlgti(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::iret(Instruction code)
{
    assert_always("Unimplemented");
}


// Hint-for-Branch Instructions (Chapter 8)
void Translator::hbr(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::hbra(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::hbrr(Instruction code)
{
    assert_always("Unimplemented");
}

}  // namespace spu
}  // namespace frontend
}  // namespace cpu
