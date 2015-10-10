/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/opcodes.h"
#include "nucleus/cpu/hir/type.h"

namespace cpu {
namespace hir {

// Forward declarations
class Function;
class Instruction;

enum ValueFlags {
    VALUE_IS_CONSTANT  = (1 << 0),
    VALUE_IS_ARGUMENT  = (1 << 1),
};

/**
 * Value
 */
class Value {
    // Value ID used in human-readable HIR representations
    S32 id = -1;

public:
    union Parent {
        Function* function;
        Instruction* instruction;
    } parent;

    union Constant {
        S8 i8;
        S16 i16;
        S32 i32;
        S64 i64;
        F32 f32;
        F64 f64;
        V128 v128;
        V256 v256;
    } constant;

    // Counter of instructions using this value as source
    U32 usage;

    // Register index
    U32 reg;

    // Flags
    U32 flags;

    Type type;

    // Get ID of this value
    S32 getId();

    // Type properties
    bool isTypeInteger() const;
    bool isTypeFloat() const;
    bool isTypeVector() const;

    // Constant properties
    bool isConstant() const;
    bool isConstantFalse() const;
    bool isConstantTrue() const;
    bool isConstantZero() const;
    bool isConstantNonzero() const;
    bool isConstantEqual(Constant other) const;

    // Constants setters
    void setConstantI8(U8 c);
    void setConstantI16(U16 c);
    void setConstantI32(U32 c);
    void setConstantI64(U64 c);
    void setConstantF32(F32 c);
    void setConstantF64(F64 c);
    void setConstantV128(V128 c);
    void setConstantV256(V256 c);

    // Constant operations
    void doAdd(Value* rhs);
    void doSub(Value* rhs);
    void doMul(Value* rhs, ArithmeticFlags flags);
    void doMulH(Value* rhs, ArithmeticFlags flags);
    void doDiv(Value* rhs, ArithmeticFlags flags);
    void doNeg();
    void doAnd(Value* rhs);
    void doOr(Value* rhs);
    void doXor(Value* rhs);
    void doNot();
    void doShl(Value* amount);
    void doShr(Value* amount);
    void doShrA(Value* amount);
    void doZExt(Type newType);
    void doSExt(Type newType);
    void doTrunc(Type newType);
    void doCast(Type newType);
    void doConvert(Type newType);
    void doCompare(Value* rhs, CompareFlags flags);
};

}  // namespace hir
}  // namespace cpu
