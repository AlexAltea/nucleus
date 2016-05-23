/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
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
 *  - Memory-Load/Store Instructions (Chapter 3)
 */

// Memory-Load/Store Instructions (Chapter 3)
void Translator::cbd(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::cbx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::cdd(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::cdx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::chd(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::chx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::cwd(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::cwx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lqa(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lqd(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lqr(Instruction code)
{
    Value* addr = builder.getConstantI32(((code.i16 << 2) + currentAddress) & ~0xF);
    Value* rt = readMemory(addr, TYPE_V128);
    setGPR(code.rt, rt);
}

void Translator::lqx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::stqa(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::stqd(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::stqr(Instruction code)
{
    Value* rt = getGPR(code.rt);
    Value* addr = builder.getConstantI32(((code.i16 << 2) + currentAddress) & ~0xF);
    writeMemory(addr, rt);
}

void Translator::stqx(Instruction code)
{
    assert_always("Unimplemented");
}

}  // namespace spu
}  // namespace frontend
}  // namespace cpu
