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

#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/IRBuilder.h"

#include <string>
#include <vector>

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
    void SetInsertPoint(Block block);
    void SetInsertPoint(Block block, llvm::BasicBlock::iterator ip);

    /**
     * HIR constants
     */
    template <typename T>
    Value<T> get(typename T::type constant) {
        static_assert(is_integer<T>::value,
            "Builder::get accepts only integer values");
        return builder.getIntN(T::size, constant);
    }

    /**
     * HIR symbol generation
     */
    Value<I8> CreateGlobalString(const std::string& str) {
        return builder.CreateGlobalString(str);
    }

    /**
     * HIR instruction generation
     */
    // Arithmetic operations
    template <typename T, int N>
    Value<T, N> CreateAdd(Value<T, N> lhs, Value<T, N> rhs) {
        static_assert(is_integer<T>::value,
            "Builder::CreateAdd accepts only integer values");
        return builder.CreateAdd(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<T, N> CreateSub(Value<T, N> lhs, Value<T, N> rhs) {
        static_assert(is_integer<T>::value,
            "Builder::CreateSub accepts only integer values");
        return builder.CreateSub(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<T, N> CreateMul(Value<T, N> lhs, Value<T, N> rhs) {
        static_assert(is_integer<T>::value,
            "Builder::CreateMul accepts only integer values");
        return builder.CreateMul(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<T, N> CreateDiv(Value<T, N> lhs, Value<T, N> rhs) {
        static_assert(is_integer<T>::value,
            "Builder::CreateDiv accepts only integer and integer-vector values");
        return builder.CreateDiv(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<T, N> CreateSDiv(Value<T, N> lhs, Value<T, N> rhs) {
        static_assert(is_integer<T>::value,
            "Builder::CreateSDiv accepts only integer and integer-vector values");
        return builder.CreateSDiv(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<T, N> CreateUDiv(Value<T, N> lhs, Value<T, N> rhs) {
        static_assert(is_integer<T>::value,
            "Builder::CreateUDiv accepts only integer and integer-vector values");
        return builder.CreateUDiv(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<T, N> CreateNeg(Value<T, N> v) {
        static_assert(is_integer<T>::value,
            "Builder::CreateNeg accepts only integer and integer-vector values");
        return builder.CreateNeg(v.value);
    }

    // Bitwise Binary Operations
    template <typename T, int N>
    Value<T, N> CreateShl(Value<T, N> val, u64 amount) {
        static_assert(is_integer<T>::value,
            "Builder::CreateShl accepts only integer and integer-vector values");
        return builder.CreateShl(val.value, amount);
    }

    template <typename T, int N>
    Value<T, N> CreateShl(Value<T, N> val, Value<T, N> amount) {
        static_assert(is_integer<T>::value,
            "Builder::CreateShl accepts only integer and integer-vector values");
        return builder.CreateShl(val.value, amount.value);
    }

    template <typename T, int N>
    Value<T, N> CreateLShr(Value<T, N> val, u64 amount) {
        static_assert(is_integer<T>::value,
            "Builder::CreateLShr accepts only integer and integer-vector values");
        return builder.CreateLShr(val.value, amount);
    }

    template <typename T, int N>
    Value<T, N> CreateLShr(Value<T, N> val, Value<T, N> amount) {
        static_assert(is_integer<T>::value,
            "Builder::CreateLShr accepts only integer and integer-vector values");
        return builder.CreateLShr(val.value, amount.value);
    }

    template <typename T, int N>
    Value<T, N> CreateAShr(Value<T, N> val, u64 amount) {
        static_assert(is_integer<T>::value,
            "Builder::CreateAShr accepts only integer and integer-vector values");
        return builder.CreateAShr(val.value, amount);
    }

    template <typename T, int N>
    Value<T, N> CreateAShr(Value<T, N> val, Value<T, N> amount) {
        static_assert(is_integer<T>::value,
            "Builder::CreateAShr accepts only integer and integer-vector values");
        return builder.CreateAShr(val.value, amount.value);
    }

    template <typename T, int N>
    Value<T, N> CreateAnd(Value<T, N> lhs, u64 rhs) {
        static_assert(is_integer<T>::value,
            "Builder::CreateAnd accepts only integer values");
        return builder.CreateAnd(lhs.value, rhs);
    }

    template <typename T, int N>
    Value<T, N> CreateAnd(Value<T, N> lhs, Value<T, N> rhs) {
        static_assert(is_integer<T>::value,
            "Builder::CreateAnd accepts only integer values");
        return builder.CreateAnd(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<T, N> CreateOr(Value<T, N> lhs, Value<T, N> rhs) {
        static_assert(is_integer<T>::value,
            "Builder::CreateOr accepts only integer values");
        return builder.CreateOr(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<T, N> CreateXor(Value<T, N> lhs, Value<T, N> rhs) {
        static_assert(is_integer<T>::value,
            "Builder::CreateXor accepts only integer values");
        return builder.CreateXor(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<T, N> CreateNot(Value<T, N> v) {
        static_assert(is_integer<T>::value,
            "Builder::CreateNot accepts only integer values");
        return builder.CreateNot(v.value);
    }

    // Floating-point arithmetic operations
    template <typename T, int N>
    Value<T, N> CreateFAdd(Value<T, N> lhs, Value<T, N> rhs) {
        static_assert(std::is_floating_point<T::type>::value,
            "Builder::CreateFAdd accepts only floating-point values");
        return builder.CreateFAdd(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<T, N> CreateFSub(Value<T, N> lhs, Value<T, N> rhs) {
        static_assert(std::is_floating_point<T::type>::value,
            "Builder::CreateFSub accepts only floating-point values");
        return builder.CreateFSub(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<T, N> CreateFMul(Value<T, N> lhs, Value<T, N> rhs) {
        static_assert(std::is_floating_point<T::type>::value,
            "Builder::CreateFMul accepts only floating-point values");
        return builder.CreateFMul(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<T, N> CreateFDiv(Value<T, N> lhs, Value<T, N> rhs) {
        static_assert(std::is_floating_point<T::type>::value,
            "Builder::CreateFDiv accepts only floating-point values");
        return builder.CreateFDiv(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<T, N> CreateFNeg(Value<T, N> v) {
        static_assert(std::is_floating_point<T::type>::value,
            "Builder::CreateFNeg accepts only floating-point values");
        return builder.CreateFNeg(v.value);
    }

    // Conversion operations
    template <typename TO, typename TI, int N>
    Value<TO, N> CreateTrunc(Value<TI, N> v) {
        static_assert(is_integer<TI>::value && is_integer<TO>::value,
            "Builder::CreateTrunc accepts only integer values");
        static_assert(TI::size > TO::size,
            "Builder::CreateTrunc converts only to smaller integer types");
        return builder.CreateTrunc(v, TO::getType().type);
    }

    template <typename TO, typename TI, int N>
    Value<TO, N> CreateSExt(Value<TI, N> v) {
        static_assert(is_integer<TI>::value && is_integer<TO>::value,
            "Builder::CreateSExt accepts only integer values");
        static_assert(TI::size < TO::size,
            "Builder::CreateSExt converts only to larger integer types");
        return builder.CreateSExt(v, TO::getType().type);
    }

    template <typename TO, typename TI, int N>
    Value<TO, N> CreateZExt(Value<TI, N> v) {
        static_assert(is_integer<TI>::value && is_integer<TO>::value,
            "Builder::CreateZExt accepts only integer values");
        static_assert(TI::size < TO::size,
            "Builder::CreateZExt converts only to larger integer types");
        return builder.CreateZExt(v, TO::getType().type);
    }

    template <typename TO, typename TI>
    Value<TO> CreateFPTrunc(Value<TI> v) {
        static_assert(std::is_floating_point<TI::type>::value && std::is_floating_point<TO::type>::value,
            "Builder::CreateFPTrunc accepts only floating-point values");
        static_assert(TI::size > TO::size,
            "Builder::CreateFPTrunc converts only to smaller floating-point types");
        return builder.CreateFPTrunc(v, TO::getType().type);
    }

    template <typename TO, typename TI>
    Value<TO> CreateFPExt(Value<TI> v) {
        static_assert(std::is_floating_point<TI::type>::value && std::is_floating_point<TO::type>::value,
            "Builder::CreateFPExt accepts only floating-point values");
        static_assert(TI::size < TO::size,
            "Builder::CreateFPExt converts only to larger floating-point types");
        return builder.CreateFPExt(v, TO::getType().type);
    }
    
    template <typename TO, int NO=1, typename TI, int NI>
    Value<TO, NO> CreateBitCast(Value<TI, NI> v) {
        static_assert(TI::size * NI == TO::size * NO,
            "Builder::CreateBitCast converts only to types of the same size");
        return builder.CreateBitCast(v, TO::getType().type);
    }

    // Vector operations
    template <typename TVec, int N, typename TIdx>
    Value<TVec> CreateExtractElement(Value<TVec, N> vec, Value<TIdx> idx) {
        static_assert(std::is_integral<TIdx::type>::value,
            "Builder::CreateExtractElement accepts only integer indices");
        return builder.CreateExtractElement(vec.value, idx.value);
    }

    // Aggregate operations
    template <int N, typename... TMembers>
    Value<typename std::tuple_element<N, std::tuple<TMembers...>>::type> CreateExtractValue(Value<Struct<TMembers...>> agg) {
        return builder.CreateExtractValue(agg.value, N);
    }

    // Memory access operations
    template <typename T, int N=1>
    Value<T*, N> CreateAlloca(const std::string& name) {
        // TODO: Support vectors (i.e. use the template parameter N)
        return builder.CreateAlloca(T::getType().type, nullptr, name);
    }

    template <typename T>
    Value<T> CreateLoad(Value<T*> ptr, const std::string& name = "") {
        return builder.CreateLoad(ptr.value, name);
    }

    template <typename T>
    void CreateStore(Value<T> val, Value<T*> ptr) {
        builder.CreateStore(val.value, ptr.value);
    }

    template <typename... TMembers>
    llvm::Value* CreateInBoundsGEP(Value<Struct<TMembers...>*> agg, std::vector<llvm::Value*> idxList, const std::string& name = "") {
        return builder.CreateInBoundsGEP(agg.value, idxList, name);
    }

    // Comparison operations
    template <typename T, int N>
    Value<I1, N> CreateICmpEQ(Value<T, N> lhs, Value<T, N> rhs) {
        return builder.CreateICmpEQ(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<I1, N> CreateICmpNE(Value<T, N> lhs, Value<T, N> rhs) {
        return builder.CreateICmpNE(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<I1, N> CreateICmpSLT(Value<T, N> lhs, Value<T, N> rhs) {
        return builder.CreateICmpSLT(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<I1, N> CreateICmpSLE(Value<T, N> lhs, Value<T, N> rhs) {
        return builder.CreateICmpSLE(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<I1, N> CreateICmpSGE(Value<T, N> lhs, Value<T, N> rhs) {
        return builder.CreateICmpSGE(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<I1, N> CreateICmpSGT(Value<T, N> lhs, Value<T, N> rhs) {
        return builder.CreateICmpSGT(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<I1, N> CreateICmpULT(Value<T, N> lhs, Value<T, N> rhs) {
        return builder.CreateICmpULT(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<I1, N> CreateICmpULE(Value<T, N> lhs, Value<T, N> rhs) {
        return builder.CreateICmpULE(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<I1, N> CreateICmpUGE(Value<T, N> lhs, Value<T, N> rhs) {
        return builder.CreateICmpUGE(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<I1, N> CreateICmpUGT(Value<T, N> lhs, Value<T, N> rhs) {
        return builder.CreateICmpUGT(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<I1, N> CreateFCmpOEQ(Value<T, N> lhs, Value<T, N> rhs) {
        return builder.CreateFCmpOEQ(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<I1, N> CreateFCmpONE(Value<T, N> lhs, Value<T, N> rhs) {
        return builder.CreateFCmpONE(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<I1, N> CreateFCmpOLT(Value<T, N> lhs, Value<T, N> rhs) {
        return builder.CreateFCmpOLT(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<I1, N> CreateFCmpOLE(Value<T, N> lhs, Value<T, N> rhs) {
        return builder.CreateFCmpOLE(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<I1, N> CreateFCmpOGE(Value<T, N> lhs, Value<T, N> rhs) {
        return builder.CreateFCmpOGE(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<I1, N> CreateFCmpOGT(Value<T, N> lhs, Value<T, N> rhs) {
        return builder.CreateFCmpOGT(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<I1, N> CreateFCmpUEQ(Value<T, N> lhs, Value<T, N> rhs) {
        return builder.CreateFCmpUEQ(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<I1, N> CreateFCmpUNE(Value<T, N> lhs, Value<T, N> rhs) {
        return builder.CreateFCmpUNE(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<I1, N> CreateFCmpULT(Value<T, N> lhs, Value<T, N> rhs) {
        return builder.CreateFCmpULT(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<I1, N> CreateFCmpULE(Value<T, N> lhs, Value<T, N> rhs) {
        return builder.CreateFCmpULE(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<I1, N> CreateFCmpUGE(Value<T, N> lhs, Value<T, N> rhs) {
        return builder.CreateFCmpUGE(lhs.value, rhs.value);
    }

    template <typename T, int N>
    Value<I1, N> CreateFCmpUGT(Value<T, N> lhs, Value<T, N> rhs) {
        return builder.CreateFCmpUGT(lhs.value, rhs.value);
    }

    // Other operations
    template <typename T, int N>
    Value<T, N> CreateSelect(Value<I1, N> cond, Value<T, N> isTrue, Value<T, N> isFalse) {
        return builder.CreateSelect(cond.value, isTrue.value, isFalse.value);
    }

    // Pointer operations
    template <typename TP, typename TV>
    Value<TP*> CreateIntToPtr(Value<TV> v) {
        return builder.CreateIntToPtr(v.value, llvm::PointerType::get(TP::getType().type, 0));
    }

    // Function operations
    llvm::Value* CreateCall(Function callee, std::vector<llvm::Value*> args = {}) {
        return builder.CreateCall(callee.function, args);
    }

    template <typename T, int N>
    void CreateRet(Value<T, N> v) {
        builder.CreateRet(v.value);
    }

    void CreateRetVoid() {
        builder.CreateRetVoid();
    }

    // Terminator operations
    void CreateBr(const Block& block) {
        builder.CreateBr(block.bb);
    }

    void CreateCondBr(Value<I1> cond, const Block& blockTrue, const Block& blockFalse) {
        builder.CreateCondBr(cond.value, blockTrue.bb, blockFalse.bb);
    }

    template <typename T>
    llvm::SwitchInst* CreateSwitch(Value<T> v, const Block& dest) {
        static_assert(is_integer<T>::value,
            "Builder::CreateSwitch accepts only integer values");
        return builder.CreateSwitch(v.value, dest.bb, 1);
    }

    /**
     * HIR intrinsic generation
     */
    template <typename T>
    Function getIntrinsic(llvm::Intrinsic::ID id) {
        llvm::Module* module = builder.GetInsertBlock()->getParent()->getParent();
        return llvm::Intrinsic::getDeclaration(module, id, T::getType().type);
    }

    // Standard C Library Intrinsics
    template <typename T>
    Value<T> CreateIntrinsic_Fabs(Value<T> val) {
        static_assert(std::is_floating_point<T::type>::value,
            "Builder::CreateIntrinsic_Fabs accepts only floating-point values");
        Function intrinsic = getIntrinsic<T>(llvm::Intrinsic::fabs);
        return CreateCall(intrinsic, {val});
    }

    template <typename T>
    Value<T> CreateIntrinsic_Sqrt(Value<T> val) {
        static_assert(std::is_floating_point<T::type>::value,
            "Builder::CreateIntrinsic_Sqrt accepts only floating-point values");
        Function intrinsic = getIntrinsic<T>(llvm::Intrinsic::sqrt);
        return CreateCall(intrinsic, {val});
    }

    // Bit Manipulation Intrinsics
    template <typename T>
    Value<T> CreateIntrinsic_Bswap(Value<T> val) {
        static_assert(is_integer<T>::value && T::size % 16 == 0,
            "Builder::CreateIntrinsic_Bswap accepts only integer values with even number of bytes");
        Function intrinsic = getIntrinsic<T>(llvm::Intrinsic::bswap);
        return CreateCall(intrinsic, {val});
    }

    template <typename T, int N>
    Value<T, N> CreateIntrinsic_Ctlz(Value<T, N> val, Value<I1> isZeroUndef) {
        static_assert(is_integer<T>::value,
            "Builder::CreateIntrinsic_Ctlz accepts only integer and integer-vector values");
        Function intrinsic = getIntrinsic<T>(llvm::Intrinsic::ctlz);
        return CreateCall(intrinsic, {val, isZeroUndef});
    }

    // Arithmetic with Overflow Intrinsics
    template <typename T>
    Value<Struct<T, I1>> CreateIntrinsic_SaddWithOverflow(Value<T> lhs, Value<T> rhs) {
        static_assert(is_integer<T>::value,
            "Builder::CreateIntrinsic_SaddWithOverflow accepts only integer values");
        Function intrinsic = getIntrinsic<T>(llvm::Intrinsic::sadd_with_overflow);
        return CreateCall(intrinsic, {lhs, rhs});
    }

    template <typename T>
    Value<Struct<T, I1>> CreateIntrinsic_UaddWithOverflow(Value<T> lhs, Value<T> rhs) {
        static_assert(is_integer<T>::value,
            "Builder::CreateIntrinsic_UaddWithOverflow accepts only integer values");
        Function intrinsic = getIntrinsic<T>(llvm::Intrinsic::uadd_with_overflow);
        return CreateCall(intrinsic, {lhs, rhs});
    }

    // Specialised Arithmetic Intrinsics
    template <typename T>
    Value<T> CreateIntrinsic_Fmuladd(Value<T> a, Value<T> b, Value<T> c) {
        static_assert(std::is_floating_point<T::type>::value,
            "Builder::CreateIntrinsic_Fmuladd accepts only floating-point values");
        Function intrinsic = getIntrinsic<T>(llvm::Intrinsic::fmuladd);
        return CreateCall(intrinsic, {a, b, c});
    }
};

}  // namespace hir
}  // namespace cpu
