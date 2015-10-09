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
 *  - Control Instructions (Chapter 10)
 */

// Control Instructions (Chapter 10)
void Recompiler::dsync(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::lnop(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::mfspr(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::mtspr(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::nop(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::stop(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::stopd(Instruction code)
{
	assert_always("Unimplemented");
}

void Recompiler::sync(Instruction code)
{
	assert_always("Unimplemented");
}

}  // namespace spu
}  // namespace cpu
