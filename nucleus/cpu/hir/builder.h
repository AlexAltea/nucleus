/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
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

// Forward declarations
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
    Value* getConstantI8(U08 c);
    Value* getConstantI16(U16 c);
    Value* getConstantI32(U32 c);
    Value* getConstantI64(U64 c);
    Value* getConstantF32(F32 c);
    Value* getConstantF64(F64 c);
    Value* getConstantV128(V128 c);
    Value* getConstantV256(V256 c);
    Value* getConstantPointer(void* c);
    Value* getConstantString(const std::string& str);

    // HIR functions
    Function* getExternFunction(void* hostAddr, Type typeOut = TYPE_VOID, const std::vector<Type>& typeIn = {});

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
    Value* createSqrt(Value* value);
    Value* createAbs(Value* value);

    // Conversion operations
    Value* createZExt(Value* value, Type type);
    Value* createSExt(Value* value, Type type);
    Value* createTrunc(Value* value, Type type);
    Value* createCast(Value* value, Type type);
    Value* createConvert(Value* value, Type type);

    // Logical operations
    Value* createNot(Value* value);
    Value* createAnd(Value* lhs, Value* rhs);
    Value* createOr(Value* lhs, Value* rhs);
    Value* createXor(Value* lhs, Value* rhs);

    // Shifting operations
    Value* createShl(Value* value, Value* amount);
    Value* createShl(Value* value, U08 rhs);
    Value* createShr(Value* value, Value* amount);
    Value* createShr(Value* value, U08 rhs);
    Value* createShrA(Value* value, Value* amount);
    Value* createShrA(Value* value, U08 rhs);

    // Memory access and context operations
    Value* createLoad(Value* address, Type type, MemoryFlags flags = ENDIAN_DEFAULT);
    void createStore(Value* address, Value* value, MemoryFlags flags = ENDIAN_DEFAULT);
    Value* createCtxLoad(U32 offset, Type type);
    void createCtxStore(U32 offset, Value* value);
    void createMemFence();

    // Comparison operations
    Value* createCmp(Value* lhs, Value* rhs, CompareFlags flags);
    Value* createCmpEQ(Value* lhs, Value* rhs);
    Value* createCmpNE(Value* lhs, Value* rhs);
    Value* createCmpSLT(Value* lhs, Value* rhs);
    Value* createCmpSLE(Value* lhs, Value* rhs);
    Value* createCmpSGE(Value* lhs, Value* rhs);
    Value* createCmpSGT(Value* lhs, Value* rhs);
    Value* createCmpULT(Value* lhs, Value* rhs);
    Value* createCmpULE(Value* lhs, Value* rhs);
    Value* createCmpUGE(Value* lhs, Value* rhs);
    Value* createCmpUGT(Value* lhs, Value* rhs);

    // Branching and conditional operations
    void createBr(Block* block);
    void createBrCond(Value* cond, Block* blockTrue, Block* blockFalse);
    Value* createCall(Function* function, const std::vector<Value*>& args = {}, CallFlags flags = CALL_INTERN);
    Value* createCallCond(Value* cond, Function* function, const std::vector<Value*>& args = {}, CallFlags flags = CALL_INTERN);
    Value* createSelect(Value* cond, Value* valueTrue, Value* valueFalse);
    void createRet(Value* value);
    void createRet();

    // Floating-point operations
    Value* createFAdd(Value* lhs, Value* rhs);
    Value* createFSub(Value* lhs, Value* rhs);
    Value* createFMul(Value* lhs, Value* rhs);
    Value* createFDiv(Value* lhs, Value* rhs);
    Value* createFNeg(Value* value);

    // Vector operations
    Value* createVAdd(Value* lhs, Value* rhs, OpcodeFlags flags);
    Value* createVSub(Value* lhs, Value* rhs, OpcodeFlags flags);
    Value* createVMul(Value* lhs, Value* rhs, OpcodeFlags flags);
    Value* createVAvg(Value* lhs, Value* rhs, OpcodeFlags flags);
    Value* createVAbs(Value* value, OpcodeFlags flags);
    Value* createVCmpEQ(Value* lhs, Value* rhs, OpcodeFlags flags);
    Value* createVCmpNE(Value* lhs, Value* rhs, OpcodeFlags flags);
    Value* createVCmpFLT(Value* lhs, Value* rhs, OpcodeFlags flags);
    Value* createVCmpFLE(Value* lhs, Value* rhs, OpcodeFlags flags);
    Value* createVCmpFGT(Value* lhs, Value* rhs, OpcodeFlags flags);
    Value* createVCmpFGE(Value* lhs, Value* rhs, OpcodeFlags flags);
    Value* createVCmpSLT(Value* lhs, Value* rhs, OpcodeFlags flags);
    Value* createVCmpSLE(Value* lhs, Value* rhs, OpcodeFlags flags);
    Value* createVCmpSGT(Value* lhs, Value* rhs, OpcodeFlags flags);
    Value* createVCmpSGE(Value* lhs, Value* rhs, OpcodeFlags flags);
    Value* createVCmpULT(Value* lhs, Value* rhs, OpcodeFlags flags);
    Value* createVCmpULE(Value* lhs, Value* rhs, OpcodeFlags flags);
    Value* createVCmpUGT(Value* lhs, Value* rhs, OpcodeFlags flags);
    Value* createVCmpUGE(Value* lhs, Value* rhs, OpcodeFlags flags);
    Value* createExtract(Value* vec, Value* index, Type type);
    Value* createInsert(Value* vec, Value* index, Value* element);
    Value* createShuffle(Value* mask, Value* v1, Value* v2);
};

}  // namespace hir
}  // namespace cpu
