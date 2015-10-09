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
 *  - Channel Instructions (Chapter 11)
 */

// Channel Instructions (Chapter 11)
void Recompiler::rchcnt(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::rdch(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::wrch(Instruction code)
{
	assert_always("Unimplemented");
}

}  // namespace spu
}  // namespace cpu
