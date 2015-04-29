/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/block.h"
#include "nucleus/cpu/hir/value.h"

#include "llvm/IR/IRBuilder.h"

namespace cpu {

class Builder
{
    llvm::IRBuilder<> builder;

public:
    Builder();

    /**
     * HIR instruction insertion
     */

    void SetInsertPoint(const Block& block);

    /**
     * HIR instruction generation
     */

    // Arithmetic operations
    template<typename T>
    Value<T> CreateAdd(Value<T> lhs, Value<T> rhs) {
        static_assert(std::is_integral<T>::value, "Builder::CreateAdd accepts only integer values.");
        return builder.CreateAdd(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateSub(Value<T> lhs, Value<T> rhs) {
        static_assert(std::is_integral<T>::value, "Builder::CreateSub accepts only integer values.");
        return builder.CreateSub(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateMul(Value<T> lhs, Value<T> rhs) {
        static_assert(std::is_integral<T>::value, "Builder::CreateMul accepts only integer values.");
        return builder.CreateMul(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateDiv(Value<T> lhs, Value<T> rhs) {
        static_assert(std::is_integral<T>::value, "Builder::CreateDiv accepts only integer values.");
        return builder.CreateDiv(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateNeg(Value<T> v) {
        static_assert(std::is_integral<T>::value, "Builder::CreateNot accepts only integer values.");
        return builder.CreateNeg(v.value);
    }

    // Binary operations
    template<typename T>
    Value<T> CreateAnd(Value<T> lhs, Value<T> rhs) {
        static_assert(std::is_integral<T>::value, "Builder::CreateAnd accepts only integer values.");
        return builder.CreateAdd(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateOr(Value<T> lhs, Value<T> rhs) {
        static_assert(std::is_integral<T>::value, "Builder::CreateOr accepts only integer values.");
        return builder.CreateOr(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateXor(Value<T> lhs, Value<T> rhs) {
        static_assert(std::is_integral<T>::value, "Builder::CreateXor accepts only integer values.");
        return builder.CreateXor(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateNot(Value<T> v) {
        static_assert(std::is_integral<T>::value, "Builder::CreateNot accepts only integer values.");
        return builder.CreateNot(v.value);
    }

    // Floating-point arithmetic operations
    template<typename T>
    Value<T> CreateFAdd(Value<T> lhs, Value<T> rhs) {
        static_assert(std::is_floating_point<T>::value, "Builder::CreateFAdd accepts only floating-point values.");
        return builder.CreateFAdd(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateFSub(Value<T> lhs, Value<T> rhs) {
        static_assert(std::is_floating_point<T>::value, "Builder::CreateFSub accepts only floating-point values.");
        return builder.CreateFSub(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateFMul(Value<T> lhs, Value<T> rhs) {
        static_assert(std::is_floating_point<T>::value, "Builder::CreateFMul accepts only floating-point values.");
        return builder.CreateFMul(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateFDiv(Value<T> lhs, Value<T> rhs) {
        static_assert(std::is_floating_point<T>::value, "Builder::CreateFDiv accepts only floating-point values.");
        return builder.CreateFDiv(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateFNeg(Value<T> v) {
        static_assert(std::is_floating_point<T>::value, "Builder::CreateFNeg accepts only floating-point values.");
        return builder.CreateFNeg(v.value);
    }

    // Function operations
    template<typename TR, typename... TArgs>
    Value<TR> CreateCall(Function<TR, TArgs...>, TArgs... args) {
        return Value<TR>{}; // TODO
    }

    template<typename TR>
    void CreateRet(TR v) {
        if (std::is_same<TR, void>::value) {
            builder.CreateRetVoid();
        } else {
            builder.CreateRet(v.value);
        }
    }

    // Branching operations
    void CreateBr(const Block& block) {
        builder.CreateBr(block.bb);
    }

    void CreateCondBr(Value<u8> cond, const Block& blockTrue, const Block& blockFalse) {
        // TODO: cond should be "Value<u1>"
        builder.CreateCondBr(cond.value, blockTrue.bb, blockFalse.bb);
    }
};

}  // namespace cpu
