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
 *  - Control Instructions (Chapter 10)
 */

// Control Instructions (Chapter 10)
void Translator::dsync(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lnop(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::mfspr(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::mtspr(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::nop(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::stop(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::stopd(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::sync(Instruction code)
{
    assert_always("Unimplemented");
}

}  // namespace spu
}  // namespace frontend
}  // namespace cpu
