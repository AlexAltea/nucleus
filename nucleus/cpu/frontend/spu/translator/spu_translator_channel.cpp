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
 *  - Channel Instructions (Chapter 11)
 */

// Channel Instructions (Chapter 11)
void Translator::rchcnt(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::rdch(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::wrch(Instruction code)
{
    assert_always("Unimplemented");
}

}  // namespace spu
}  // namespace frontend
}  // namespace cpu
