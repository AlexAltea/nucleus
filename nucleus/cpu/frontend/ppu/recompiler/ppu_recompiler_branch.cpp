/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"
#include "nucleus/core/config.h"
#include "nucleus/cpu/util.h"
#include "nucleus/assert.h"

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
    const U32 targetAddr = code.aa ? (code.li << 2) : (currentAddress + (code.li << 2)) & ~0x3;

    // Unconditional call
    if (code.lk) {
        if (config.ppuTranslator & CPU_TRANSLATOR_IS_JIT) {
            auto* module = static_cast<Module*>(function->parent);
            module->addFunction(targetAddr);
        }
        createFunctionCall(targetAddr);
    }

    // Unconditional branch
    else {
        hir::Block* targetBlock = blocks.at(targetAddr);
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

    Value* ctr_ok = nullptr;
    if (!bo2) {
        Value* ctr = getCTR();
        ctr = builder.createSub(ctr, builder.getConstantI64(1));
        if (!bo3) {
            ctr_ok = builder.createCmpNE(ctr, builder.getConstantI64(0));
        } else {
            ctr_ok = builder.createCmpEQ(ctr, builder.getConstantI64(0));
        }
        setCTR(ctr);
    }

    Value* cond_ok = nullptr;
    if (!bo0) {
        if (!bo1) {
            cond_ok = builder.createXor(getCRBit(code.bi), builder.getConstantI8(1));
        } else {
            cond_ok = getCRBit(code.bi);
        }
    }

    Value* cond = nullptr;
    if (ctr_ok && cond_ok) {
        builder.createAnd(ctr_ok, cond_ok);
    } else if (ctr_ok) {
        cond = ctr_ok;
    }  else if (cond_ok) {
        cond = cond_ok;
    }

    // Unconditional/conditional call
    if (code.lk) {
        if (config.ppuTranslator & CPU_TRANSLATOR_IS_JIT) {
            auto* module = static_cast<Module*>(function->parent);
            module->addFunction(targetAddr);
        }
        createFunctionCall(targetAddr, cond);
    }

    // Unconditional/conditional branch
    else {
        if (cond) {
            builder.createBrCond(cond, blocks.at(targetAddr), blocks.at(nextAddr));
        } else {
            builder.createBr(blocks.at(targetAddr));
        }
    }
}

void Recompiler::bcctrx(Instruction code)
{
    Value* targetAddr = getCTR();
    const U32 nextAddr = (currentAddress + 4) & ~0x3;

    // Check condition
    Value* cond_ok = nullptr;
    const auto bo0 = code.bo & 0x10;
    const auto bo1 = code.bo & 0x08;
    if (!bo0) {
        assert_always("Unimplemented");
        // TODO: Set cond_ok
    }

    // Conditional function call
    if (code.lk) {
        if (config.ppuTranslator & CPU_TRANSLATOR_IS_JIT) {
            hir::Function* proxyFunc = builder.getExternFunction(reinterpret_cast<void*>(nucleusCall));
            if (cond_ok) {
                builder.createCallCond(cond_ok, proxyFunc, {targetAddr}, hir::CALL_EXTERN);
            } else {
                builder.createCall(proxyFunc, {targetAddr}, hir::CALL_EXTERN);
            }
        }
    }

    // Simple conditional branch
    else {
        if (config.ppuTranslator & CPU_TRANSLATOR_IS_JIT) {
            hir::Function* proxyFunc = builder.getExternFunction(reinterpret_cast<void*>(nucleusCall));
            if (cond_ok) {
                builder.createCallCond(cond_ok, proxyFunc, {targetAddr}, hir::CALL_EXTERN);
            } else {
                builder.createCall(proxyFunc, {targetAddr}, hir::CALL_EXTERN);
            }
            builder.createRet();
        }
    }
}

void Recompiler::bclrx(Instruction code)
{
    // Check condition
    const U8 bo0 = (code.bo & 0x10) ? 1 : 0;
    const U8 bo1 = (code.bo & 0x08) ? 1 : 0;
    const U8 bo2 = (code.bo & 0x04) ? 1 : 0;
    const U8 bo3 = (code.bo & 0x02) ? 1 : 0;

    Value* ctr_ok = nullptr;
    if (!bo2) {
        Value* ctr = getCTR();
        ctr = builder.createSub(ctr, builder.getConstantI64(1));
        if (!bo3) {
            ctr_ok = builder.createCmpNE(ctr, builder.getConstantI64(0));
        } else {
            ctr_ok = builder.createCmpEQ(ctr, builder.getConstantI64(0));
        }
        setCTR(ctr);
    }

    Value* cond_ok = nullptr;
    if (!bo0) {
        if (!bo1) {
            cond_ok = builder.createXor(getCRBit(code.bi), builder.getConstantI8(1));
        } else {
            cond_ok = getCRBit(code.bi);
        }
    }

    Value* cond = nullptr;
    if (ctr_ok && cond_ok) {
        builder.createAnd(ctr_ok, cond_ok);
    } else if (ctr_ok) {
        cond = ctr_ok;
    }  else if (cond_ok) {
        cond = cond_ok;
    }

    // Call the return
    if (code.lk) {
        assert_always("Unimplemented");
    }

    // Just return
    else {
        if (cond_ok) {
            builder.createBrCond(cond_ok, epilog, blocks[currentAddress + 4]);
        } else {
            builder.createBr(epilog);
        }
    }
}

void Recompiler::crand(Instruction code)
{
    Value* crba = getCRBit(code.crba);
    Value* crbb = getCRBit(code.crbb);
    Value* crbd;

    crbd = builder.createAnd(crba, crbb);
    setCRBit(code.crbd, crbd);
}

void Recompiler::crandc(Instruction code)
{
    Value* crba = getCRBit(code.crba);
    Value* crbb = getCRBit(code.crbb);
    Value* crbd;

    crbd = builder.createAnd(crba, builder.createNot(crbb));
    setCRBit(code.crbd, crbd);
}

void Recompiler::creqv(Instruction code)
{
    Value* crba = getCRBit(code.crba);
    Value* crbb = getCRBit(code.crbb);
    Value* crbd;

    crbd = builder.createNot(builder.createXor(crba, crbb));
    setCRBit(code.crbd, crbd);
}

void Recompiler::crnand(Instruction code)
{
    Value* crba = getCRBit(code.crba);
    Value* crbb = getCRBit(code.crbb);
    Value* crbd;

    crbd = builder.createNot(builder.createAnd(crba, crbb));
    setCRBit(code.crbd, crbd);
}

void Recompiler::crnor(Instruction code)
{
    Value* crba = getCRBit(code.crba);
    Value* crbb = getCRBit(code.crbb);
    Value* crbd;

    crbd = builder.createNot(builder.createOr(crba, crbb));
    setCRBit(code.crbd, crbd);
}

void Recompiler::cror(Instruction code)
{
    Value* crba = getCRBit(code.crba);
    Value* crbb = getCRBit(code.crbb);
    Value* crbd;

    crbd = builder.createOr(crba, crbb);
    setCRBit(code.crbd, crbd);
}

void Recompiler::crorc(Instruction code)
{
    Value* crba = getCRBit(code.crba);
    Value* crbb = getCRBit(code.crbb);
    Value* crbd;

    crbd = builder.createOr(crba, builder.createNot(crbb));
    setCRBit(code.crbd, crbd);
}

void Recompiler::crxor(Instruction code)
{
    Value* crba = getCRBit(code.crba);
    Value* crbb = getCRBit(code.crbb);
    Value* crbd;

    crbd = builder.createXor(crba, crbb);
    setCRBit(code.crbd, crbd);
}

void Recompiler::mcrf(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::sc(Instruction code)
{
    hir::Function* syscallFunc = builder.getExternFunction(reinterpret_cast<void*>(nucleusSysCall));

    // TODO: Use code.lev fields
    builder.createCall(syscallFunc, {}, CALL_EXTERN);
}

void Recompiler::td(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::tdi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::tw(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::twi(Instruction code)
{
    assert_always("Unimplemented");
}

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
