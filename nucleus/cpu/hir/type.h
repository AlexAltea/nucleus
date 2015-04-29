/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"

namespace cpu {
namespace hir {

class Type
{
public:
    llvm::Type* type;
};

// Integer types
struct I1 {
    static Type type() {
        return Type{ llvm::Type::getInt1Ty(llvm::getGlobalContext()) };
    }
};
struct I8 {
    static Type type() {
        return Type{ llvm::Type::getInt8Ty(llvm::getGlobalContext()) };
    }
};
struct I16 {
    static Type type() {
        return Type{ llvm::Type::getInt16Ty(llvm::getGlobalContext()) };
    }
};
struct I32 {
    static Type type() {
        return Type{ llvm::Type::getInt32Ty(llvm::getGlobalContext()) };
    }
};
struct I64 {
    static Type type() {
        return Type{ llvm::Type::getInt64Ty(llvm::getGlobalContext()) };
    }
};

// Floating-point types
struct F32 {
    static Type type() {
        return Type{ llvm::Type::getFloatTy(llvm::getGlobalContext()) };
    }
};
struct F64 {
    static Type type() {
        return Type{ llvm::Type::getDoubleTy(llvm::getGlobalContext()) };
    }
};

// Aggregate types
template<typename T, int N>
struct Array {
    static Type type() {
        return Type{ llvm::ArrayType::get(T::type().type, N) };
    }
};

template<typename T, int N>
struct Vector {
    static Type type() {
        return Type{ llvm::VectorType::get(T::type().type, N) };
    }
};

template<typename... Ts>
struct Struct {
    static Type type() {
        return Type{ llvm::StructType::get(Ts::type().type) };
    }
};

// Pointer type
template<typename T>
struct Pointer {
    static Type type() {
        return Type{ llvm::PointerType::get(T::type().type, 0) };
    }
};

}  // namespace hir
}  // namespace cpu
