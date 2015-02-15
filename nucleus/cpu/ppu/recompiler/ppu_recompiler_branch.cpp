/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"

namespace cpu {
namespace ppu {

/**
 * PPC64 Instructions:
 *  - UISA: Branch and Flow Control Instructions (Section: 4.2.4)
 */

void Recompiler::bx(Instruction code)
{
}

void Recompiler::bcx(Instruction code)
{
}

void Recompiler::bcctrx(Instruction code)
{
}

void Recompiler::bclrx(Instruction code)
{
    llvm::Value* ret;

    // Unconditional branch
    if (code.bo == 20 && code.bi == 0) {
        switch (returnType) {
        case FUNCTION_OUT_INTEGER:
            ret = getGPR(3);
            break;
        case FUNCTION_OUT_FLOAT:
            ret = getFPR(1);
            break;
        case FUNCTION_OUT_FLOAT_X2:
            ret = getFPR(1); // TODO
            break;
        case FUNCTION_OUT_FLOAT_X3:
            ret = getFPR(1); // TODO
            break;
        case FUNCTION_OUT_FLOAT_X4:
            ret = getFPR(1); // TODO
            break;
        case FUNCTION_OUT_VECTOR:
            ret = getVR_u32(2);
            break;
        case FUNCTION_OUT_VOID:
            builder.CreateRetVoid();
            return;
        }
        builder.CreateRet(ret);
    } else {
        // TODO
    }
}

void Recompiler::crand(Instruction code)
{
}

void Recompiler::crandc(Instruction code)
{
}

void Recompiler::creqv(Instruction code)
{
}

void Recompiler::crnand(Instruction code)
{
}

void Recompiler::crnor(Instruction code)
{
}

void Recompiler::cror(Instruction code)
{
}

void Recompiler::crorc(Instruction code)
{
}

void Recompiler::crxor(Instruction code)
{
}

void Recompiler::mcrf(Instruction code)
{
}

void Recompiler::sc(Instruction code)
{
}

void Recompiler::td(Instruction code)
{
}

void Recompiler::tdi(Instruction code)
{
}

void Recompiler::tw(Instruction code)
{
}

void Recompiler::twi(Instruction code)
{
}

}  // namespace ppu
}  // namespace cpu
