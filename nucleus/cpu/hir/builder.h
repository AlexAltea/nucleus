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
public:
    /**
     * HIR insertion
     */
    void setInsertPoint(Block* block);
    void setInsertPoint(Block* block, std::list<Instruction*>::iterator ip);

    /**
     * HIR constants
     */
    Value* alloc(Type type);
    Value* getConstantI8(U8 v);
    Value* getConstantI16(U16 v);
    Value* getConstantI32(U32 v);
    Value* getConstantI64(U64 v);
    Value* getConstantF32(F32 v);
    Value* getConstantF64(F64 v);
    Value* getConstantV128(V128 v);
    Value* getConstantString(const std::string& str);

    /**
     * HIR instruction generation
     */
    // Arithmetic operations
    Value* createAdd(Value* lhs, Value* rhs);
    Value* createSub(Value* lhs, Value* rhs);
    Value* createMul(Value* lhs, Value* rhs, ArithmeticFlags flags = ARITHMETIC_SIGNED);
    Value* createMulH(Value* lhs, Value* rhs, ArithmeticFlags flags = ARITHMETIC_SIGNED);
    Value* createDiv(Value* lhs, Value* rhs, ArithmeticFlags flags = ARITHMETIC_SIGNED);
    Value* createNeg(Value* value);
    Value* createSqrt(Value* value);
    Value* createCtlz(Value* value);

    // Logical operations
    Value* createAnd(Value* lhs, Value* rhs);
    Value* createAnd(Value* lhs, U64 rhs);
    Value* createOr(Value* lhs, Value* rhs);
    Value* createOr(Value* lhs, U64 rhs);
    Value* createXor(Value* lhs, Value* rhs);
    Value* createXor(Value* lhs, U64 rhs);
    Value* createNot(Value* value);

    // Shifting
    Value* createShl(Value* value, Value* amount);
    Value* createShl(Value* value, U64 rhs);
    Value* createShlA(Value* value, Value* amount);
    Value* createShlA(Value* value, U64 rhs);
    Value* createShr(Value* value, Value* amount);
    Value* createShr(Value* value, U64 rhs);
    Value* createShrA(Value* value, Value* amount);
    Value* createShrA(Value* value, U64 rhs);

    Value* createZExt(Value* value, Type type);
    Value* createSExt(Value* value, Type type);
    Value* createTrunc(Value* value, Type type);

    // Memory access operations
    Value* createLoad(Value* address, Type type, MemoryFlags flags = ENDIAN_DEFAULT);
    void createStore(Value* address, Value* value, MemoryFlags flags = ENDIAN_DEFAULT);

    // Comparison operations
    Value* createCmpEQ(Value* lhs, Value* rhs);
    Value* createCmpNE(Value* lhs, Value* rhs);
    Value* createCmpULT(Value* lhs, Value* rhs);
    Value* createCmpULE(Value* lhs, Value* rhs);
    Value* createCmpUGT(Value* lhs, Value* rhs);
    Value* createCmpUGE(Value* lhs, Value* rhs);
    Value* createCmpSLT(Value* lhs, Value* rhs);
    Value* createCmpSLE(Value* lhs, Value* rhs);
    Value* createCmpSGT(Value* lhs, Value* rhs);
    Value* createCmpSGE(Value* lhs, Value* rhs);

    // Branching and conditionals
    Value* createSelect(Value* cond, Value* valueTrue, Value* valueFalse);
    void createRet(Value* value);
};

}  // namespace hir
}  // namespace cpu
