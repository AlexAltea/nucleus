/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"
#include "nucleus/config.h"

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
        if (config.ppuTranslator & CPU_TRANSLATOR_IS_JIT) {
            createUnknownFunctionCall(target);
        }
        if (config.ppuTranslator & CPU_TRANSLATOR_IS_AOT) {
            createKnownFunctionCall(target);
        }
    }

    // Simple unconditional branch
    else {
        hir::Block* targetBlock = blocks.at(target);
        builder.createBr(targetBlock);
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

    Value* ctr_ok = builder.createOr(
        builder.getConstantI8(bo2),
        builder.createXor(
            builder.createCmpNE(getCTR(), builder.getConstantI64(0)),
            builder.getConstantI8(bo2)));
    Value* cond_ok = builder.createOr(
        builder.getConstantI8(bo0),
        builder.createXor(
            builder.createShr(getCR(code.bi >> 2), builder.getConstantI8(code.bi & 0x3)),
            builder.getConstantI8(~bo1 & 0x1)));

    Value* cond = builder.createAnd(ctr_ok, cond_ok);

    // Conditional function call
    if (code.lk) {
        // TODO
    }

    // Simple conditional branch
    else {
        hir::Block* targetBlock = blocks.at(targetAddr);
        hir::Block* nextBlock = blocks.at(nextAddr);
        builder.createBrCond(cond, targetBlock, nextBlock);
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
    /*// Store parameter registers in the PPU state
    for (int index = 3; index <= 10; index++) {
        Value* regPointer = builder.createInBoundsGEP(state, {
            builder.getConstantI32(0),
            builder.getConstantI32(0),
            builder.getConstantI32(index)});
        builder.createStore(getGPR(index), regPointer);
    }

    // Call Nucleus syscall handler
    builder.createCall(static_cast<Segment*>(function->parent)->funcSystemCall); // TODO: Use code.lev fields

    // Load return register from the PPU state
    Value* regPointer = builder.createInBoundsGEP(state, {
        builder.getConstantI32(0),
        builder.getConstantI32(0),
        builder.getConstantI32(3)});
    setGPR(3, builder.createLoad(regPointer));*/
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
