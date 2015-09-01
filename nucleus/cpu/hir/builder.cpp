/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "builder.h"
#include "nucleus/cpu/hir/instruction.h"
#include "nucleus/assert.h"

#define ASSERT_TYPE_EQUAL(value1, value2) \
    assert_true(value1->type == value2->type)

#define ASSERT_TYPE_INTEGER(value) \
    assert_true( \
    value->type == TYPE_I8  || \
    value->type == TYPE_I16 || \
    value->type == TYPE_I32 || \
    value->type == TYPE_I64)

#define ASSERT_TYPE_FLOAT(value) \
    assert_true( \
    value->type == TYPE_F32 || \
    value->type == TYPE_F64)

#define ASSERT_TYPE_VECTOR(value) \
    assert_true( \
    value->type == TYPE_V128 || \
    value->type == TYPE_V256)

namespace cpu {
namespace hir {

void Builder::setInsertPoint(Block* block) {
    // TODO
}

void Builder::setInsertPoint(Block* block, std::list<Instruction*>::iterator ip) {
    // TODO
}

// HIR values
Value* Builder::allocValue(Type type) {
    return nullptr;
}

Value* Builder::cloneValue(Value* source) {
    return nullptr;
}

// HIR constants
Value* Builder::getConstantI8(U8 c) {
    Value* value = allocValue(TYPE_I8);
    value->setConstantI8(c);
    return value;
}
Value* Builder::getConstantI16(U16 c) {
    Value* value = allocValue(TYPE_I16);
    value->setConstantI16(c);
    return value;
}
Value* Builder::getConstantI32(U32 c) {
    Value* value = allocValue(TYPE_I32);
    value->setConstantI32(c);
    return value;
}
Value* Builder::getConstantI64(U64 c) {
    Value* value = allocValue(TYPE_I64);
    value->setConstantI64(c);
    return value;
}
Value* Builder::getConstantF32(F32 c) {
    Value* value = allocValue(TYPE_F32);
    value->setConstantF32(c);
    return value;
}
Value* Builder::getConstantF64(F64 c) {
    Value* value = allocValue(TYPE_F64);
    value->setConstantF64(c);
    return value;
}
Value* Builder::getConstantV128(V128 c) {
    Value* value = allocValue(TYPE_V128);
    value->setConstantV128(c);
    return value;
}
Value* Builder::getConstantV256(V256 c) {
    Value* value = allocValue(TYPE_V256);
    value->setConstantV256(c);
    return value;
}

// Arithmetic operations
Value* Builder::createAdd(Value* lhs, Value* rhs) {
    ASSERT_TYPE_INTEGER(lhs);
    ASSERT_TYPE_EQUAL(lhs, rhs);

    if (lhs->isConstantZero()) {
        return rhs;
    } else if (rhs->isConstantZero()) {
        return lhs;
    } else if (lhs->isConstant() && rhs->isConstant()) {
        Value* dest = cloneValue(lhs);
        dest->doAdd(rhs);
        return dest;
    }

    Instruction* i;
    i->src1.value = lhs;
    i->src2.value = rhs;
    return i->dest;
}

Value* Builder::createSub(Value* lhs, Value* rhs) {
    ASSERT_TYPE_INTEGER(lhs);
    ASSERT_TYPE_EQUAL(lhs, rhs);

    if (rhs->isConstantZero()) {
        return lhs;
    } else if (lhs->isConstant() && rhs->isConstant()) {
        Value* dest = cloneValue(lhs);
        dest->doSub(rhs);
        return dest;
    }

    Instruction* i;
    i->src1.value = lhs;
    i->src2.value = rhs;
    return i->dest;
}

Value* Builder::createMul(Value* lhs, Value* rhs, ArithmeticFlags flags) {
    ASSERT_TYPE_INTEGER(lhs);
    ASSERT_TYPE_EQUAL(lhs, rhs);

    if (lhs->isConstant() && rhs->isConstant()) {
        Value* dest = cloneValue(lhs);
        dest->doMul(rhs, flags);
        return dest;
    }

    Instruction* i;
    i->src1.value = lhs;
    i->src2.value = rhs;
    return i->dest;
}

Value* Builder::createDiv(Value* lhs, Value* rhs, ArithmeticFlags flags) {
    ASSERT_TYPE_INTEGER(lhs);
    ASSERT_TYPE_EQUAL(lhs, rhs);

    if (lhs->isConstant() && rhs->isConstant()) {
        Value* dest = cloneValue(lhs);
        dest->doDiv(rhs, flags);
        return dest;
    }

    Instruction* i;
    i->src1.value = lhs;
    i->src2.value = rhs;
    return i->dest;
}

void Builder::createRet(Value* value) {
}

void Builder::createRet() {
}

}  // namespace hir
}  // namespace cpu
