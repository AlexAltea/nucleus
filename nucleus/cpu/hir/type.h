/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"

#include <algorithm>

namespace cpu {
namespace hir {

typedef size_t TypeSize;

class Type
{
public:
    llvm::Type* type;
};

/**
 * Type classes
 */
// Integer types
struct I1 {
    using type = bool;
    static const TypeSize size = 1;
    static Type getType() {
        return Type{ llvm::Type::getInt1Ty(llvm::getGlobalContext()) };
    }
};

struct I8 {
    using type = u8;
    static const TypeSize size = 8;
    static Type getType() {
        return Type{ llvm::Type::getInt8Ty(llvm::getGlobalContext()) };
    }
};

struct I16 {
    using type = u16;
    static const TypeSize size = 16;
    static Type getType() {
        return Type{ llvm::Type::getInt16Ty(llvm::getGlobalContext()) };
    }
};

struct I32 {
    using type = u32;
    static const TypeSize size = 32;
    static Type getType() {
        return Type{ llvm::Type::getInt32Ty(llvm::getGlobalContext()) };
    }
};

struct I64 {
    using type = u64;
    static const TypeSize size = 64;
    static Type getType() {
        return Type{ llvm::Type::getInt64Ty(llvm::getGlobalContext()) };
    }
};

struct I128 {
    using type = u128;
    static const TypeSize size = 128;
    static Type getType() {
        return Type{ llvm::Type::getIntNTy(llvm::getGlobalContext(), 128) };
    }
};

// Floating-point types
struct F32 {
    using cmp = I1;
    using type = f32;
    static const TypeSize size = 32;
    static Type getType() {
        return Type{ llvm::Type::getFloatTy(llvm::getGlobalContext()) };
    }
};

struct F64 {
    using type = f64;
    static const TypeSize size = 64;
    static Type getType() {
        return Type{ llvm::Type::getDoubleTy(llvm::getGlobalContext()) };
    }
};

// Aggregate types
template <typename T, int N>
struct Array {
    using type = typename T::type[N];
    static const TypeSize size = N * T::size;
    static Type getType() {
        return Type{ llvm::ArrayType::get(T::getType().type, N) };
    }
};

template <typename T, int N>
struct Vector {
    using type = typename T::type[N];
    static const TypeSize size = N * T::size;
    static Type getType() {
        return Type{ llvm::VectorType::get(T::getType().type, N) };
    }
};

template <typename... Ts>
class Struct {
private:
    template <typename TFirst, typename... TRest>
    struct PartialStruct {
        static const TypeSize size = TFirst::size + PartialStruct<TRest...>::size;
        static std::vector<llvm::Type*> getTypeArray() {
            auto typeArray = PartialStruct<TRest...>::getTypeArray();
            typeArray.push_back(TFirst::getType().type);
            return typeArray;
        }
    };

    template <typename TFirst>
    struct PartialStruct<TFirst> {
        static const TypeSize size = TFirst::size;
        static std::vector<llvm::Type*> getTypeArray() {
            return { TFirst::getType().type };
        }
    };

public:
    static const TypeSize size = PartialStruct<Ts...>::size;
    static Type getType() {
        // TODO: Is there a better way to handle this that doesn't involve reversing the vector?
        auto typeArray = PartialStruct<Ts...>::getTypeArray();
        std::reverse(typeArray.begin(), typeArray.end());
        return Type{ llvm::StructType::get(llvm::getGlobalContext(), typeArray) };
    }
};

// Pointer type
template <typename T>
struct Pointer {
    static const TypeSize size = 64; // TODO: This is only true in some architectures
    static Type getType() {
        return Type{ llvm::PointerType::get(T::getType().type, 0) };
    }
};

// Void type
struct Void {
    static Type getType() {
        return Type{ llvm::Type::getVoidTy(llvm::getGlobalContext()) };
    }
};

// Any type
struct Any {
    static Type getType() {
        // TODO: This should never happen
        return Type{ llvm::Type::getVoidTy(llvm::getGlobalContext()) };
    }
};

/**
 * Type checks
 */
template <typename T>
struct is_integer {
    static const bool value =
        std::is_same<T, I1>::value  ||
        std::is_same<T, I8>::value  ||
        std::is_same<T, I16>::value ||
        std::is_same<T, I32>::value ||
        std::is_same<T, I64>::value ||
        std::is_same<T, I128>::value;
};

template <typename T>
struct is_floating_point {
    static const bool value =
        std::is_same<T, F32>::value ||
        std::is_same<T, F64>::value;
};

}  // namespace hir
}  // namespace cpu
