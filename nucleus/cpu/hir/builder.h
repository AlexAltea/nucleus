/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/block.h"
#include "nucleus/cpu/hir/function.h"
#include "nucleus/cpu/hir/type.h"
#include "nucleus/cpu/hir/value.h"

#include "llvm/IR/IRBuilder.h"

namespace cpu {
namespace hir {

class Builder
{
    llvm::IRBuilder<> builder;

public:
    Builder();

    /**
     * HIR insertion
     */
    void SetInsertPoint(const Block& block);

    /**
     * HIR constants
     */
    template<typename T>
    Value<T> get(typename T::type constant) {
        static_assert(std::is_integral<T::type>::value,
            "Builder::get accepts only integer values.");
        return builder.getIntN(T::size, constant);
    }

    /**
     * HIR instruction generation
     */
    // Arithmetic operations
    template<typename T>
    Value<T> CreateAdd(Value<T> lhs, Value<T> rhs) {
        static_assert(std::is_integral<T::type>::value,
            "Builder::CreateAdd accepts only integer values.");
        return builder.CreateAdd(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateSub(Value<T> lhs, Value<T> rhs) {
        static_assert(std::is_integral<T::type>::value,
            "Builder::CreateSub accepts only integer values.");
        return builder.CreateSub(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateMul(Value<T> lhs, Value<T> rhs) {
        static_assert(std::is_integral<T::type>::value,
            "Builder::CreateMul accepts only integer values.");
        return builder.CreateMul(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateDiv(Value<T> lhs, Value<T> rhs) {
        static_assert(std::is_integral<T::type>::value,
            "Builder::CreateDiv accepts only integer values.");
        return builder.CreateDiv(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateNeg(Value<T> v) {
        static_assert(std::is_integral<T::type>::value,
            "Builder::CreateNot accepts only integer values.");
        return builder.CreateNeg(v.value);
    }

    // Binary operations
    template<typename T>
    Value<T> CreateAnd(Value<T> lhs, Value<T> rhs) {
        static_assert(std::is_integral<T::type>::value,
            "Builder::CreateAnd accepts only integer values.");
        return builder.CreateAnd(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateOr(Value<T> lhs, Value<T> rhs) {
        static_assert(std::is_integral<T::type>::value,
            "Builder::CreateOr accepts only integer values.");
        return builder.CreateOr(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateXor(Value<T> lhs, Value<T> rhs) {
        static_assert(std::is_integral<T::type>::value,
            "Builder::CreateXor accepts only integer values.");
        return builder.CreateXor(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateNot(Value<T> v) {
        static_assert(std::is_integral<T::type>::value,
            "Builder::CreateNot accepts only integer values.");
        return builder.CreateNot(v.value);
    }

    // Floating-point arithmetic operations
    template<typename T>
    Value<T> CreateFAdd(Value<T> lhs, Value<T> rhs) {
        static_assert(std::is_floating_point<T::type>::value,
            "Builder::CreateFAdd accepts only floating-point values.");
        return builder.CreateFAdd(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateFSub(Value<T> lhs, Value<T> rhs) {
        static_assert(std::is_floating_point<T::type>::value,
            "Builder::CreateFSub accepts only floating-point values.");
        return builder.CreateFSub(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateFMul(Value<T> lhs, Value<T> rhs) {
        static_assert(std::is_floating_point<T::type>::value,
            "Builder::CreateFMul accepts only floating-point values.");
        return builder.CreateFMul(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateFDiv(Value<T> lhs, Value<T> rhs) {
        static_assert(std::is_floating_point<T::type>::value,
            "Builder::CreateFDiv accepts only floating-point values.");
        return builder.CreateFDiv(lhs.value, rhs.value);
    }

    template<typename T>
    Value<T> CreateFNeg(Value<T> v) {
        static_assert(std::is_floating_point<T::type>::value,
            "Builder::CreateFNeg accepts only floating-point values.");
        return builder.CreateFNeg(v.value);
    }

    // Conversion operations
    template<typename TO, typename TI>
    Value<TO> CreateTrunc(Value<TI> v) {
        static_assert(std::is_integral<TI::type>::value && std::is_integral<TO::type>::value,
            "Builder::CreateTrunc accepts only integer values.");
        static_assert(TI::size < TO::size,
            "Builder::CreateTrunc converts only to smaller integer types.");
        return builder.CreateTrunc(v, TO::type().type);
    }

    template<typename TO, typename TI>
    Value<TO> CreateSExt(Value<TI> v) {
        static_assert(std::is_integral<TI::type>::value && std::is_integral<TO::type>::value,
            "Builder::CreateSExt accepts only integer values.");
        static_assert(TI::size < TO::size,
            "Builder::CreateSExt converts only to larger integer types.");
        return builder.CreateSExt(v, TO::type().type);
    }

    template<typename TO, typename TI>
    Value<TO> CreateZExt(Value<TI> v) {
        static_assert(std::is_integral<TI::type>::value && std::is_integral<TO::type>::value,
            "Builder::CreateZExt accepts only integer values.");
        static_assert(TI::size < TO::size,
            "Builder::CreateZExt converts only to larger integer types.");
        return builder.CreateZExt(v, TO::type().type);
    }

    template<typename TO, typename TI>
    Value<TO> CreateFPTrunc(Value<TI> v) {
        static_assert(std::is_floating_point<TI::type>::value && std::is_floating_point<TO::type>::value,
            "Builder::CreateFPTrunc accepts only floating-point values.");
        static_assert(TI::size > TO::size,
            "Builder::CreateFPTrunc converts only to smaller floating-point types.");
        return builder.CreateFPTruc(v, TO::type().type);
    }

    template<typename TO, typename TI>
    Value<TO> CreateFPExt(Value<TI> v) {
        static_assert(std::is_floating_point<TI::type>::value && std::is_floating_point<TO::type>::value,
            "Builder::CreateFPExt accepts only floating-point values.");
        static_assert(TI::size < TO::size,
            "Builder::CreateFPExt converts only to larger floating-point types.");
        return builder.CreateFPExt(v, TO::type().type);
    }
    
    template<typename TO, typename TI>
    Value<TO> CreateBitCast(Value<TI> v) {
        static_assert(TI::size == TO::size,
            "Builder::CreateFPExt converts only to larger floating-point types.");
        return builder.CreateBitCast(v, TO::type().type);
    }

    // Comparison operations
    template<typename T>
    typename T::cmp CreateICmpEQ(Value<T> lhs, Value<T> rhs) {
        builder.CreateICmpEQ(lhs.value, rhs.value);
    }
    template<typename T>
    typename T::cmp CreateICmpNE(Value<T> lhs, Value<T> rhs) {
        builder.CreateICmpNE(lhs.value, rhs.value);
    }
    template<typename T>
    typename T::cmp CreateICmpSLT(Value<T> lhs, Value<T> rhs) {
        builder.CreateICmpSLT(lhs.value, rhs.value);
    }
    template<typename T>
    typename T::cmp CreateICmpSLE(Value<T> lhs, Value<T> rhs) {
        builder.CreateICmpSLE(lhs.value, rhs.value);
    }
    template<typename T>
    typename T::cmp CreateICmpSGE(Value<T> lhs, Value<T> rhs) {
        builder.CreateICmpSGE(lhs.value, rhs.value);
    }
    template<typename T>
    typename T::cmp CreateICmpSGT(Value<T> lhs, Value<T> rhs) {
        builder.CreateICmpSGT(lhs.value, rhs.value);
    }
    template<typename T>
    typename T::cmp CreateICmpULT(Value<T> lhs, Value<T> rhs) {
        builder.CreateICmpULT(lhs.value, rhs.value);
    }
    template<typename T>
    typename T::cmp CreateICmpULE(Value<T> lhs, Value<T> rhs) {
        builder.CreateICmpULE(lhs.value, rhs.value);
    }
    template<typename T>
    typename T::cmp CreateICmpUGE(Value<T> lhs, Value<T> rhs) {
        builder.CreateICmpUGE(lhs.value, rhs.value);
    }
    template<typename T>
    typename T::cmp CreateICmpUGT(Value<T> lhs, Value<T> rhs) {
        builder.CreateICmpUGT(lhs.value, rhs.value);
    }
    template<typename T>
    typename T::cmp CreateFCmpOEQ(Value<T> lhs, Value<T> rhs) {
        builder.CreateFCmpOEQ(lhs.value, rhs.value);
    }
    template<typename T>
    typename T::cmp CreateFCmpONE(Value<T> lhs, Value<T> rhs) {
        builder.CreateFCmpONE(lhs.value, rhs.value);
    }
    template<typename T>
    typename T::cmp CreateFCmpOLT(Value<T> lhs, Value<T> rhs) {
        builder.CreateFCmpOLT(lhs.value, rhs.value);
    }
    template<typename T>
    typename T::cmp CreateFCmpOLE(Value<T> lhs, Value<T> rhs) {
        builder.CreateFCmpOLE(lhs.value, rhs.value);
    }
    template<typename T>
    typename T::cmp CreateFCmpOGE(Value<T> lhs, Value<T> rhs) {
        builder.CreateFCmpOGE(lhs.value, rhs.value);
    }
    template<typename T>
    typename T::cmp CreateFCmpOGT(Value<T> lhs, Value<T> rhs) {
        builder.CreateFCmpOGT(lhs.value, rhs.value);
    }
    template<typename T>
    typename T::cmp CreateFCmpUEQ(Value<T> lhs, Value<T> rhs) {
        builder.CreateFCmpUEQ(lhs.value, rhs.value);
    }
    template<typename T>
    typename T::cmp CreateFCmpUNE(Value<T> lhs, Value<T> rhs) {
        builder.CreateFCmpUNE(lhs.value, rhs.value);
    }
    template<typename T>
    typename T::cmp CreateFCmpULT(Value<T> lhs, Value<T> rhs) {
        builder.CreateFCmpULT(lhs.value, rhs.value);
    }
    template<typename T>
    typename T::cmp CreateFCmpULE(Value<T> lhs, Value<T> rhs) {
        builder.CreateFCmpULE(lhs.value, rhs.value);
    }
    template<typename T>
    typename T::cmp CreateFCmpUGE(Value<T> lhs, Value<T> rhs) {
        builder.CreateFCmpUGE(lhs.value, rhs.value);
    }
    template<typename T>
    typename T::cmp CreateFCmpUGT(Value<T> lhs, Value<T> rhs) {
        builder.CreateFCmpUGT(lhs.value, rhs.value);
    }

    // Pointer operations
    template<typename TV, typename TP>
    Value<Pointer<TP>> CreateIntToPtr(Value<TV> v, Pointer<TP> pointer) {
        return builder.CreateIntToPtr(v, pointer::type().type);
    }

    // Function operations
    template<typename TR, typename... TArgs>
    Value<TR> CreateCall(Function func, TArgs... args) {
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

    void CreateCondBr(Value<I1> cond, const Block& blockTrue, const Block& blockFalse) {
        builder.CreateCondBr(cond.value, blockTrue.bb, blockFalse.bb);
    }
};

}  // namespace hir
}  // namespace cpu
