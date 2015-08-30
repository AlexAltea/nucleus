/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"

namespace cpu {
namespace frontend {
namespace ppu {

using namespace cpu::hir;

/**
 * PPC64 Instructions:
 *  - UISA: Branch and Flow Control Instructions (Section: 4.2.4)
 */

void Recompiler::bx(Instruction code)
{
    const U32 target = code.aa ? (code.li << 2) : (currentAddress + (code.li << 2)) & ~0x3;

    // Function call
    if (code.lk) {
        auto& targetFunc = static_cast<Function&>(*function->parent->functions.at(target));

        // Generate array of arguments
        int index = 0;
        std::vector<llvm::Value*> arguments;
        for (const auto& param : targetFunc.type_in) {
            switch (param) {
            case FUNCTION_IN_INTEGER:
                arguments.push_back(getGPR(3 + index).value);
                break;
            case FUNCTION_IN_FLOAT:
                arguments.push_back(getFPR(1 + index).value);
                break;
            case FUNCTION_IN_VECTOR:
                arguments.push_back(getVR<I32>(2 + index).value);
                break;
            }
            index += 1;
        }

        Value<Any> result = builder.CreateCall(targetFunc.function, arguments);

        // Save return value
        switch (targetFunc.type_out) {
        case FUNCTION_OUT_INTEGER:
            setGPR(3, Value<I64>{ result.value });
            break;
        case FUNCTION_OUT_FLOAT:
            setFPR(1, Value<F64>{ result.value });
            break;
        case FUNCTION_OUT_FLOAT_X2:
            setFPR(1, Value<F64>{ result.value }); // TODO
            break;
        case FUNCTION_OUT_FLOAT_X3:
            setFPR(1, Value<F64>{ result.value }); // TODO
            break;
        case FUNCTION_OUT_FLOAT_X4:
            setFPR(1, Value<F64>{ result.value }); // TODO
            break;
        case FUNCTION_OUT_VECTOR:
            setVR(2, Value<I128>{ result.value });
            break;
        }
    }

    // Simple unconditional branch
    else {
        hir::Block& targetBlock = blocks.at(target);
        builder.CreateBr(targetBlock);
    }
}

void Recompiler::bcx(Instruction code)
{
    const U32 targetAddr = code.aa ? (code.bd << 2) : (currentAddress + (code.bd << 2)) & ~0x3;
    const U32 nextAddr = (currentAddress + 4) & ~0x3;

    // Check condition
    const U8 bo0 = (code.bo & 0x10) ? 1 : 0;
    const U8 bo1 = (code.bo & 0x08) ? 1 : 0;
    const U8 bo2 = (code.bo & 0x04) ? 1 : 0;
    const U8 bo3 = (code.bo & 0x02) ? 1 : 0;

    if (!bo2) {
        // TODO: Decrement CTR register by 1
    }

    Value<I1> ctr_ok = builder.CreateOr(
        builder.get<I1>(bo2),
        builder.CreateXor(
            builder.CreateICmpNE(getCTR(), builder.get<I64>(0)),
            builder.get<I1>(bo2)));
    Value<I1> cond_ok = builder.CreateOr(
        builder.get<I1>(bo0),
        builder.CreateXor(
            builder.CreateTrunc<I1>(builder.CreateLShr(getCR(code.bi >> 2), builder.get<I8>(code.bi & 0x3))),
            builder.get<I1>(~bo1 & 0x1)));

    Value<I1> cond = builder.CreateAnd(ctr_ok, cond_ok);

    // Conditional function call
    if (code.lk) {
        // TODO
    }

    // Simple conditional branch
    else {
        hir::Block& targetBlock = blocks.at(targetAddr);
        hir::Block& nextBlock = blocks.at(nextAddr);
        builder.CreateCondBr(cond, targetBlock, nextBlock);
    }
}

void Recompiler::bcctrx(Instruction code)
{
}

void Recompiler::bclrx(Instruction code)
{
    // Unconditional return
    if (code.bo == 20 && code.bi == 0) {
        builder.createBr(epilog);
    }

    // Conditional return
    else {
        // TODO: This is wrong
        builder.createBr(epilog);
    }
}

void Recompiler::crand(Instruction code)
{
    Value* crba = getCR(code.crba);
    Value* crbb = getCR(code.crbb);
    Value* crbd;

    crbd = builder.createAnd(crba, crbb);
    setCR(code.crbd, crbd);
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
    // Store parameter registers in the PPU state
    for (int index = 3; index <= 10; index++) {
        Value<I64*> regPointer = builder.CreateInBoundsGEP(state, {
            builder.get<I32>(0),
            builder.get<I32>(0),
            builder.get<I32>(index)});
        builder.CreateStore(getGPR(index), regPointer);
    }

    // Call Nucleus syscall handler
    builder.createCall(static_cast<Segment*>(function->parent)->funcSystemCall); // TODO: Use code.lev fields

    // Load return register from the PPU state
    Value<I64*> regPointer = builder.CreateInBoundsGEP(state, {
        builder.get<I32>(0),
        builder.get<I32>(0),
        builder.get<I32>(3)});
    setGPR(3, builder.CreateLoad(regPointer));
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
}  // namespace frontend
}  // namespace cpu
