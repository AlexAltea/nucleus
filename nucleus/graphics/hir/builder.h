/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include "block.h"
#include "instruction.h"

namespace graphics {
namespace hir {

class Builder {
    // Instruction insert point
    Block* insertionBlock;
    std::list<Instruction*>::iterator insertionIp;

    // Insert instruction and return it
    Instruction* insert(Instruction* instruction) {
        if (insertionBlock) {
            insertionBlock->instructions.insert(insertionIp, instruction);
        }
        return instruction;
    }

public:
    Builder();

    /**
     * HIR insertion
     */
    void setInsertPoint(Block* block);
    void setInsertPoint(Block* block, std::list<Instruction>::iterator ip);

    /**
     * HIR instruction generation
     */

    // Integer arithmetic operations
    template <typename T, int N, int M>
    Value<T, N, M> createIAdd(Value<T, N, M> lhs, Value<T, N, M> rhs) {
        static_assert(std::is_integral<T>::value,
            "Builder::createIAdd accepts only integer values");
        return insert(new Instruction{ OpIAdd, {lhs, rhs} });
    }

    template <typename T, int N, int M>
    Value<T, N, M> createISub(Value<T, N, M> lhs, Value<T, N, M> rhs) {
        static_assert(std::is_integral<T>::value,
            "Builder::createISub accepts only integer values");
        return insert(new Instruction{ OpISub, {lhs, rhs} });
    }

    template <typename T, int N, int M>
    Value<T, N, M> createIMul(Value<T, N, M> lhs, Value<T, N, M> rhs) {
        static_assert(std::is_integral<T>::value,
            "Builder::createIMul accepts only integer values");
        return insert(new Instruction{ OpIMul, {lhs, rhs} });
    }

    template <typename T, int N, int M>
    Value<T, N, M> createIDiv(Value<T, N, M> lhs, Value<T, N, M> rhs) {
        static_assert(std::is_integral<T>::value,
            "Builder::createIDiv accepts only integer values");
        return insert(new Instruction{ OpIDiv, {lhs, rhs} });
    }

    // Floating-point arithmetic operations
    template <typename T, int N, int M>
    Value<T, N, M> createFAdd(Value<T, N, M> lhs, Value<T, N, M> rhs) {
        static_assert(std::is_floating_point<T>::value,
            "Builder::createFAdd accepts only floating-point values");
        return insert(new Instruction{ OpFAdd, {lhs, rhs} });
    }

    template <typename T, int N, int M>
    Value<T, N, M> createFSub(Value<T, N, M> lhs, Value<T, N, M> rhs) {
        static_assert(std::is_floating_point<T>::value,
            "Builder::createFSub accepts only floating-point values");
        return insert(new Instruction{ OpFSub, {lhs, rhs} });
    }

    template <typename T, int N, int M>
    Value<T, N, M> createFMul(Value<T, N, M> lhs, Value<T, N, M> rhs) {
        static_assert(std::is_floating_point<T>::value,
            "Builder::createFMul accepts only floating-point values");
        return insert(new Instruction{ OpFMul, {lhs, rhs} });
    }

    template <typename T, int N, int M>
    Value<T, N, M> createFDiv(Value<T, N, M> lhs, Value<T, N, M> rhs) {
        static_assert(std::is_floating_point<T>::value,
            "Builder::createFDiv accepts only floating-point values");
        return insert(new Instruction{ OpFDiv, {lhs, rhs} });
    }
};

}  // namespace hir
}  // namespace graphics
