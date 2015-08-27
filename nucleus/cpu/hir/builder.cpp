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

void Builder::setInsertPoint(Block* block)
{
    builder.SetInsertPoint(block.bb);
}

void Builder::setInsertPoint(Block* block, llvm::BasicBlock::iterator ip)
{
    builder.SetInsertPoint(block.bb, ip);
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
        Value* dest = Value::create(lhs);
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
        Value* dest = Value::create(lhs);
        dest->doSub(rhs);
        return dest;
    }

    Instruction* i;
    i->src1.value = lhs;
    i->src2.value = rhs;
    return i->dest;
}

Value* Builder::createMul(Value* lhs, Value* rhs) {
    ASSERT_TYPE_INTEGER(lhs);
    ASSERT_TYPE_EQUAL(lhs, rhs);

    if (lhs->isConstant() && rhs->isConstant()) {
        Value* dest = Value::create(lhs);
        dest->doMul(rhs);
        return dest;
    }

    Instruction* i;
    i->src1.value = lhs;
    i->src2.value = rhs;
    return i->dest;
}

Value* Builder::createDiv(Value* lhs, Value* rhs) {
    ASSERT_TYPE_INTEGER(lhs);
    ASSERT_TYPE_EQUAL(lhs, rhs);

    if (lhs->isConstant() && rhs->isConstant()) {
        Value* dest = Value::create(lhs);
        dest->doDiv(rhs);
        return dest;
    }

    Instruction* i;
    i->src1.value = lhs;
    i->src2.value = rhs;
    return i->dest;
}


}  // namespace hir
}  // namespace cpu
