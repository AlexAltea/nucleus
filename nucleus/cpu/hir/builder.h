/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/instruction.h"
#include "nucleus/cpu/hir/type.h"
#include "nucleus/cpu/hir/value.h"

#include <list>
#include <vector>

namespace cpu {
namespace hir {

class Block;
class Value;

class Builder {
    Block* ib;
    std::list<Instruction*>::iterator ip;

public:
    /**
     * HIR insertion
     */
    void setInsertPoint(Block* block);
    void setInsertPoint(Block* block, std::list<Instruction*>::iterator ip);

    // HIR values
    Value* allocValue(Type type);
    Value* cloneValue(Value* source);

    // HIR constants
    Value* getConstantI8(U8 c);
    Value* getConstantI16(U16 c);
    Value* getConstantI32(U32 c);
    Value* getConstantI64(U64 c);
    Value* getConstantF32(F32 c);
    Value* getConstantF64(F64 c);
    Value* getConstantV128(V128 c);
    Value* getConstantV256(V256 c);
    Value* getConstantString(const std::string& str);

    /**
     * HIR instruction generation
     */
    Instruction* appendInstr(Opcode opcode, OpcodeFlags flags, Value* dest = nullptr);

    // Arithmetic operations
    Value* createAdd(Value* lhs, Value* rhs);
    Value* createSub(Value* lhs, Value* rhs);
    Value* createMul(Value* lhs, Value* rhs, ArithmeticFlags flags = ARITHMETIC_SIGNED);
    Value* createMulH(Value* lhs, Value* rhs, ArithmeticFlags flags = ARITHMETIC_SIGNED);
    Value* createDiv(Value* lhs, Value* rhs, ArithmeticFlags flags = ARITHMETIC_SIGNED);
    Value* createNeg(Value* value);
    Value* createCtlz(Value* value);
    Value* createZExt(Value* value, Type type);
    Value* createSExt(Value* value, Type type);
    Value* createTrunc(Value* value, Type type);
    Value* createSqrt(Value* value);
    Value* createAbs(Value* value);

    // Logical operations
    Value* createAnd(Value* lhs, Value* rhs);
    Value* createAnd(Value* lhs, U64 rhs);
    Value* createOr(Value* lhs, Value* rhs);
    Value* createOr(Value* lhs, U64 rhs);
    Value* createXor(Value* lhs, Value* rhs);
    Value* createXor(Value* lhs, U64 rhs);
    Value* createNot(Value* value);

    // Shifting operations
    Value* createShl(Value* value, Value* amount);
    Value* createShl(Value* value, U64 rhs);
    Value* createShlA(Value* value, Value* amount);
    Value* createShlA(Value* value, U64 rhs);
    Value* createShr(Value* value, Value* amount);
    Value* createShr(Value* value, U64 rhs);
    Value* createShrA(Value* value, Value* amount);
    Value* createShrA(Value* value, U64 rhs);

    // Memory access operations
    Value* createLoad(Value* address, Type type, MemoryFlags flags = ENDIAN_DEFAULT);
    void createStore(Value* address, Value* value, MemoryFlags flags = ENDIAN_DEFAULT);

    // Comparison operations
    Value* createCmpEQ(Value* lhs, Value* rhs);
    Value* createCmpNE(Value* lhs, Value* rhs);
    Value* createCmpSLT(Value* lhs, Value* rhs);
    Value* createCmpSLE(Value* lhs, Value* rhs);
    Value* createCmpSGT(Value* lhs, Value* rhs);
    Value* createCmpSGE(Value* lhs, Value* rhs);
    Value* createCmpULT(Value* lhs, Value* rhs);
    Value* createCmpULE(Value* lhs, Value* rhs);
    Value* createCmpUGT(Value* lhs, Value* rhs);
    Value* createCmpUGE(Value* lhs, Value* rhs);

    // Branching and conditional operations
    Value* createBr(Block* block);
    Value* createBrCond(Value* cond, Block* blockTrue, Block* blockFalse);
    Value* createSelect(Value* cond, Value* valueTrue, Value* valueFalse);
    void createRet(Value* value);
    void createRet();

    // Floating-point operations
    Value* createFAdd(Value* lhs, Value* rhs);
    Value* createFSub(Value* lhs, Value* rhs);
    Value* createFMul(Value* lhs, Value* rhs);
    Value* createFDiv(Value* lhs, Value* rhs);
    Value* createFNeg(Value* value);
    Value* createFExt(Value* value, Type type);
    Value* createFTrunc(Value* value, Type type);

    // Vector operations
    Value* createVAdd(Value* lhs, Value* rhs, Type compType);
    Value* createVSub(Value* lhs, Value* rhs, Type compType);
    Value* createVMul(Value* lhs, Value* rhs, Type compType, ArithmeticFlags flags = ARITHMETIC_SIGNED);
    Value* createVAvg(Value* lhs, Value* rhs, Type compType, ArithmeticFlags flags = ARITHMETIC_SIGNED);
    Value* createVAbs(Value* value, Type compType);
    Value* createVCmpEQ(Value* lhs, Value* rhs, Type compType);
    Value* createVCmpNE(Value* lhs, Value* rhs, Type compType);
    Value* createVCmpFLT(Value* lhs, Value* rhs, Type compType);
    Value* createVCmpFLE(Value* lhs, Value* rhs, Type compType);
    Value* createVCmpFGT(Value* lhs, Value* rhs, Type compType);
    Value* createVCmpFGE(Value* lhs, Value* rhs, Type compType);
    Value* createVCmpSLT(Value* lhs, Value* rhs, Type compType);
    Value* createVCmpSLE(Value* lhs, Value* rhs, Type compType);
    Value* createVCmpSGT(Value* lhs, Value* rhs, Type compType);
    Value* createVCmpSGE(Value* lhs, Value* rhs, Type compType);
    Value* createVCmpULT(Value* lhs, Value* rhs, Type compType);
    Value* createVCmpULE(Value* lhs, Value* rhs, Type compType);
    Value* createVCmpUGT(Value* lhs, Value* rhs, Type compType);
    Value* createVCmpUGE(Value* lhs, Value* rhs, Type compType);
};

}  // namespace hir
}  // namespace cpu
