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
 *  - Control Instructions (Chapter 10)
 */

// Control Instructions (Chapter 10)
void Recompiler::dsync(Instruction code)
{
}

void Recompiler::lnop(Instruction code)
{
}

void Recompiler::mfspr(Instruction code)
{
}

void Recompiler::mtspr(Instruction code)
{
}

void Recompiler::nop(Instruction code)
{
}

void Recompiler::stop(Instruction code)
{
}

void Recompiler::stopd(Instruction code)
{
}

void Recompiler::sync(Instruction code)
{
}

}  // namespace spu
}  // namespace cpu
