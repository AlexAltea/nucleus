/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/value.h"

#include "llvm/IR/IRBuilder.h"

namespace cpu {

class Builder
{
public:
    llvm::IRBuilder<> builder;

    /** 
     * Generation of HIR (LLVM IR) instructions
     */

    // Arithmetic operations
    template<typename T>
    Value<T> CreateAdd(Value<T> lhs, Value<T> rhs) {
        return builder.CreateAdd(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateSub(Value<T> lhs, Value<T> rhs) {
        return builder.CreateSub(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateMul(Value<T> lhs, Value<T> rhs) {
        return builder.CreateMul(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateDiv(Value<T> lhs, Value<T> rhs) {
        return builder.CreateDiv(lhs.value, rhs.value);
    }

    // Binary operations
    template<typename T>
    Value<T> CreateAnd(Value<T> lhs, Value<T> rhs) {
        return builder.CreateAdd(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateOr(Value<T> lhs, Value<T> rhs) {
        return builder.CreateOr(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateXor(Value<T> lhs, Value<T> rhs) {
        return builder.CreateXor(lhs.value, rhs.value);
    }

    // Function calls
    template<typename TR, typename... TArgs>
    Value<TR> CreateCall(Function<TR, TArgs...>, TArgs... args) {
        return Value<TR>{}; // TODO
    }
};

}  // namespace cpu
