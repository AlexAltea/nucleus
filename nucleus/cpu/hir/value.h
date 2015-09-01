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

/**
 * Value
 */
class Value {
public:
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

    // Flags
    U32 flags;

    Type type;

    bool isConstant() const;
    bool isConstantFalse() const;
    bool isConstantTrue() const;
    bool isConstantZero() const;
    bool isConstantNonzero() const;
    bool isConstantEqual(Constant other) const;

    // Constant operations
    void doAdd(Value* rhs);
    void doSub(Value* rhs);
    void doMul(Value* rhs, ArithmeticFlags flags);
    void doMulH(Value* rhs, ArithmeticFlags flags);
    void doDiv(Value* rhs, ArithmeticFlags flags);
};

}  // namespace hir
}  // namespace cpu
