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
    const u32 target = code.aa ? (code.li << 2) : (currentAddress + (code.li << 2)) & ~0x3;

    // Function call
    if (code.lk) {
        Function& targetFunc = segment->functions.at(target);

        // Generate array of arguments
        int index = 0;
        std::vector<llvm::Value*> arguments;
        for (const auto& param : targetFunc.type_in) {
            switch (param) {
            case FUNCTION_IN_INTEGER:
                arguments.push_back(getGPR(3 + index));
                break;
            case FUNCTION_IN_FLOAT:
                arguments.push_back(getFPR(1 + index));
                break;
            case FUNCTION_IN_VECTOR:
                arguments.push_back(getVR_u32(2 + index));
                break;
            }
            index += 1;
        }

        llvm::Value* result = builder.CreateCall(targetFunc.function, arguments);

        // Save return value
        switch (targetFunc.type_out) {
        case FUNCTION_OUT_INTEGER:
            setGPR(3, result);
            break;
        case FUNCTION_OUT_FLOAT:
            setFPR(1, result);
            break;
        case FUNCTION_OUT_FLOAT_X2:
            setFPR(1, result); // TODO
            break;
        case FUNCTION_OUT_FLOAT_X3:
            setFPR(1, result); // TODO
            break;
        case FUNCTION_OUT_FLOAT_X4:
            setFPR(1, result); // TODO
            break;
        case FUNCTION_OUT_VECTOR:
            setVR(2, result);
            break;
        }
    }

    // Simple unconditional branch
    else {
        Block& targetBlock = function->blocks.at(target);
        builder.CreateBr(targetBlock.bb);
    }
}

void Recompiler::bcx(Instruction code)
{
    const u32 targetAddr = code.aa ? (code.bd << 2) : (currentAddress + (code.bd << 2)) & ~0x3;
    const u32 nextAddr = (currentAddress + 4) & ~0x3;

    llvm::Value* cond;
    cond = builder.getInt1(true); // TODO: This is wrong

    // Conditional function call
    if (code.lk) {
        // TODO
    }

    // Simple conditional branch
    else {
        Block& targetBlock = function->blocks.at(targetAddr);
        Block& nextBlock = function->blocks.at(nextAddr);
        builder.CreateCondBr(cond, targetBlock.bb, nextBlock.bb);
    }
}

void Recompiler::bcctrx(Instruction code)
{
}

void Recompiler::bclrx(Instruction code)
{
    // Unconditional return
    if (code.bo == 20 && code.bi == 0) {
        createReturn();
    }

    // Conditional return
    else {
        // TODO: This is wrong
        createReturn();
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
